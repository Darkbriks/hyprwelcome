#include "ActionButton.hpp"

#include <QProcess>
#include <QHBoxLayout>

static int kSubtitleCmdTimeoutMs = 4000;

ActionButton::ActionButton(const ActionConfig& action,
                            const ThemeColors&  colors,
                            QWidget* parent,
                            const int subtitleRefreshIntervalMs,
                            const int subtitleCmdTimeoutMs)
    : QPushButton{parent}
    , m_action{action}
{
    kSubtitleCmdTimeoutMs = subtitleCmdTimeoutMs;

    setProperty("class", "ActionButton");
    setObjectName("ActionButton");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setCursor(Qt::PointingHandCursor);

    auto* layout = new QVBoxLayout{this};
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(4);

    m_iconLabel = new QLabel{resolveIcon(action.icon), this};
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setObjectName("ButtonIcon");

    m_textLabel = new QLabel{action.label, this};
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setObjectName("ButtonText");
    m_textLabel->setWordWrap(true);

    layout->addWidget(m_iconLabel);
    layout->addWidget(m_textLabel);

    if (!action.subtitle.isEmpty()) {
        m_subtitleLabel = new QLabel{this};
        m_subtitleLabel->setAlignment(Qt::AlignCenter);
        m_subtitleLabel->setObjectName("ButtonSubtitle");
        m_subtitleLabel->setWordWrap(true);
        layout->addWidget(m_subtitleLabel);

        if (isSubtitleCmd()) {
            refreshSubtitle();

            m_refreshTimer = new QTimer{this};
            m_refreshTimer->setInterval(subtitleRefreshIntervalMs);
            connect(m_refreshTimer, &QTimer::timeout,
                    this, &ActionButton::refreshSubtitle);
            m_refreshTimer->start();
        } else {
            m_subtitleLabel->setText(action.subtitle);
        }
    }

    applyTheme(colors);

    connect(this, &QPushButton::clicked, this, &ActionButton::onClicked);
}

void ActionButton::refreshSubtitle() {
    if (!m_subtitleLabel || !isSubtitleCmd()) return;

    auto* proc = new QProcess{this};
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc, &QProcess::finished, this,
            [this, proc](int exitCode, QProcess::ExitStatus) {
                Q_UNUSED(exitCode)
                const QString output = QString::fromUtf8(proc->readAll()).trimmed();
                if (m_subtitleLabel && !output.isEmpty())
                    m_subtitleLabel->setText(output);
                proc->deleteLater();
            });

    proc->start("sh", {"-c", subtitleCmd()});

    QTimer::singleShot(kSubtitleCmdTimeoutMs, proc, [proc]() {
        if (proc->state() != QProcess::NotRunning)
            proc->kill();
    });
}

void ActionButton::applyTheme(const ThemeColors& colors) {
    const QString accent = accentColor(colors);
    const int r = colors.buttonBorderRadius;
    const int w = colors.buttonBorderWidth;

    setStyleSheet(QString{R"(
        QPushButton#ActionButton {
            background-color: %1;
            border: %2px solid %3;
            border-left: 4px solid %4;
            border-radius: %5px;
        }
        QPushButton#ActionButton:hover {
            background-color: %6;
            border-color: %4;
            border-left: 4px solid %4;
        }
        QPushButton#ActionButton:pressed {
            background-color: %7;
        }
        QLabel#ButtonIcon {
            color: %4;
            font-size: 24px;
            background: transparent;
            border: none;
        }
        QLabel#ButtonText {
            color: %8;
            font-size: 12px;
            font-weight: bold;
            background: transparent;
            border: none;
        }
        QLabel#ButtonSubtitle {
            color: %9;
            font-size: 10px;
            background: transparent;
            border: none;
        }
    )"}
    .arg(colors.buttonBackground)
    .arg(w)
    .arg(colors.border)
    .arg(accent)
    .arg(r)
    .arg(colors.buttonBackgroundHover)
    .arg(colors.buttonBackgroundPressed)
    .arg(colors.buttonTextColor)
    .arg(colors.foregroundDim));
}

void ActionButton::onClicked() {
    if (m_action.command == "__theme_cycle__")
        return;

    if (m_action.confirm) {
        QMessageBox dlg{this};
        dlg.setWindowTitle("Confirmation");
        dlg.setText(QString{"Exécuter : %1 ?"}.arg(m_action.label));
        dlg.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        dlg.setDefaultButton(QMessageBox::Cancel);
        if (dlg.exec() != QMessageBox::Yes)
            return;
    }

    QProcess::startDetached("sh", {"-c", m_action.command});

    if (isSubtitleCmd())
        QTimer::singleShot(1500, this, &ActionButton::refreshSubtitle);
}

bool ActionButton::isSubtitleCmd() const {
    return m_action.subtitle.startsWith("cmd:");
}

QString ActionButton::subtitleCmd() const {
    return m_action.subtitle.sliced(4);  // retire "cmd:"
}

QString ActionButton::accentColor(const ThemeColors& colors) const {
    if (!m_action.color.isEmpty())
        return colors.resolve(m_action.color);
    return colors.borderActive;
}

QString ActionButton::resolveIcon(const QString& icon) const {
    if (icon.startsWith("nf:"))
        return icon.sliced(3);
    return icon;
}