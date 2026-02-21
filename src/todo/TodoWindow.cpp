#include "TodoWindow.hpp"

#include <QApplication>
#include <QScreen>

#include "TodoRow.hpp"

static const QMap<QString, QStringList> kThemeDecorations = {
    {"celeste",   {"🍓", "🏔️", "❄️", "🌟", "💜"}},
    {"default",   {"✦", "◆", "◇", "✧", "⋆"}},
};

static QString pickDecoration(const ThemeColors& colors) {
    const QString name = colors.name.toLower();

    for (const auto& [key, symbols] : kThemeDecorations.asKeyValueRange()) {
        if (name.contains(key)) {
            return symbols[QDateTime::currentMSecsSinceEpoch() % symbols.size()];
        }
    }
    return kThemeDecorations["default"][0];
}

TodoWindow::TodoWindow(QWidget* parent) : QWidget{parent} {
    try {
        const auto cfg = ConfigLoader::load();
        m_colors = ThemeManager::loadTheme(cfg.currentTheme);
    } catch (...) {
        m_colors.background         = "#1E2832";
        m_colors.backgroundAlt      = "#2C3E50";
        m_colors.foreground         = "#E8E4DF";
        m_colors.foregroundDim      = "#B8C5D0";
        m_colors.border             = "#3D4E5C";
        m_colors.borderActive       = "#86BBD8";
        m_colors.buttonBackground   = "#2C3E50";
        m_colors.buttonBackgroundHover   = "#3D4E5C";
        m_colors.buttonBackgroundPressed = "#1E2832";
        m_colors.buttonTextColor    = "#E8E4DF";
        m_colors.buttonBorderRadius = 10;
        m_colors.buttonBorderWidth  = 1;
        m_colors.name               = "default";
    }

    m_store = new TodoStore{this};

    setWindowTitle("HyprTodo");
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("TodoWindow");
    resize(380, 520);

    const auto* screen = QApplication::primaryScreen();
    if (screen) {
        const auto geo = screen->availableGeometry();
        move(geo.center() - rect().center());
    }

    buildUI();
    applyTheme();
    rebuildList();

    connect(m_store, &TodoStore::dataChanged, this, &TodoWindow::rebuildList);
}

void TodoWindow::buildUI() {
    auto* root = new QVBoxLayout{this};
    root->setContentsMargins(16, 14, 16, 14);
    root->setSpacing(10);

    auto* header = new QHBoxLayout{};

    m_decoration = new QLabel{pickDecoration(m_colors), this};
    m_decoration->setObjectName("Decoration");
    m_decoration->setFixedWidth(28);

    auto* title = new QLabel{"Todo", this};
    title->setObjectName("TodoTitle");

    auto* deco2 = new QLabel{pickDecoration(m_colors), this};
    deco2->setObjectName("Decoration");
    deco2->setFixedWidth(28);
    deco2->setAlignment(Qt::AlignRight);

    header->addWidget(m_decoration);
    header->addWidget(title, 1, Qt::AlignCenter);
    header->addWidget(deco2);
    root->addLayout(header);

    m_scrollArea = new QScrollArea{this};
    m_scrollArea->setObjectName("TodoScroll");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_listWidget = new QWidget{};
    m_listWidget->setObjectName("TodoList");
    m_listLayout = new QVBoxLayout{m_listWidget};
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(2);
    m_listLayout->addStretch();

    m_scrollArea->setWidget(m_listWidget);
    root->addWidget(m_scrollArea, 1);

    auto* inputRow = new QHBoxLayout{};

    m_input = new QLineEdit{this};
    m_input->setObjectName("TodoInput");
    m_input->setPlaceholderText("Nouvelle tâche…");
    inputRow->addWidget(m_input, 1);

    m_addBtn = new QPushButton{"󰐕", this};
    m_addBtn->setObjectName("AddBtn");
    m_addBtn->setFixedSize(32, 32);
    m_addBtn->setToolTip("Ajouter");
    inputRow->addWidget(m_addBtn);

    root->addLayout(inputRow);

    auto* dateRow = new QHBoxLayout{};

    m_dateCheck = new QCheckBox{"Date d'échéance", this};
    m_dateCheck->setObjectName("DateCheck");
    dateRow->addWidget(m_dateCheck);

    m_dateEdit = new QDateEdit{QDate::currentDate().addDays(1), this};
    m_dateEdit->setObjectName("DateEdit");
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("dd/MM/yyyy");
    m_dateEdit->setEnabled(false);
    dateRow->addWidget(m_dateEdit, 1);

    root->addLayout(dateRow);

    connect(m_dateCheck, &QCheckBox::toggled, m_dateEdit, &QDateEdit::setEnabled);
    connect(m_addBtn, &QPushButton::clicked, this, &TodoWindow::addFromInput);
    connect(m_input, &QLineEdit::returnPressed, this, &TodoWindow::addFromInput);
}

void TodoWindow::rebuildList() {
    while (m_listLayout->count() > 1) {
        auto* item = m_listLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    QList<TodoItem> active, done;
    for (const auto& item : m_store->items()) {
        if (item.completed) done.push_back(item);
        else                active.push_back(item);
    }

    auto addRow = [this](const TodoItem& item) {
        auto* row = new TodoRow{item, m_colors, m_listWidget};
        connect(row, &TodoRow::completionToggled, m_store, &TodoStore::setCompleted);
        connect(row, &TodoRow::deleteRequested,   m_store, &TodoStore::removeItem);
        m_listLayout->insertWidget(m_listLayout->count() - 1, row);
    };

    for (const auto& item : active) addRow(item);

    if (!done.isEmpty() && !active.isEmpty()) {
        auto* sep = new QFrame{m_listWidget};
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet(QString{"color: %1;"}.arg(m_colors.border));
        m_listLayout->insertWidget(m_listLayout->count() - 1, sep);
    }

    for (const auto& item : done) addRow(item);
}

void TodoWindow::applyTheme() {
    const auto& c = m_colors;
    const int   r = c.buttonBorderRadius;

    setStyleSheet(QString{R"(
        QWidget#TodoWindow {
            background-color: %1;
            border-radius: 14px;
            border: 1px solid %2;
        }
        QLabel#TodoTitle {
            color: %3;
            font-size: 15px;
            font-weight: bold;
        }
        QLabel#Decoration {
            font-size: 18px;
        }
        QWidget#TodoList {
            background: transparent;
        }
        QScrollArea#TodoScroll {
            background: transparent;
        }
        QScrollBar:vertical {
            background: %4;
            width: 6px;
            border-radius: 3px;
        }
        QScrollBar::handle:vertical {
            background: %2;
            border-radius: 3px;
        }
        QCheckBox#DateCheck {
            color: %5;
            font-size: 11px;
        }
        QLineEdit#TodoInput {
            background: %4;
            color: %3;
            border: 1px solid %2;
            border-radius: %6px;
            padding: 6px 10px;
            font-size: 13px;
        }
        QLineEdit#TodoInput:focus {
            border-color: %7;
        }
        QDateEdit#DateEdit {
            background: %4;
            color: %3;
            border: 1px solid %2;
            border-radius: %6px;
            padding: 4px 8px;
            font-size: 12px;
        }
        QPushButton#AddBtn {
            background: %7;
            color: %1;
            border: none;
            border-radius: %6px;
            font-size: 18px;
        }
        QPushButton#AddBtn:hover {
            background: %8;
        }
        QWidget {
            color: %3;
            background: transparent;
        }
        QCheckBox {
            color: %3;
        }
    )"}
    .arg(c.background)
    .arg(c.border)
    .arg(c.foreground)
    .arg(c.backgroundAlt)
    .arg(c.foregroundDim)
    .arg(r)
    .arg(c.accents.value("accent_primary",   "#86BBD8"))
    .arg(c.accents.value("accent_secondary", "#C2A1D3")));
}

void TodoWindow::addFromInput() {
    const QString text = m_input->text().trimmed();
    if (text.isEmpty()) return;

    std::optional<QDate> due;
    if (m_dateCheck->isChecked())
        due = m_dateEdit->date();

    m_store->addItem(text, due);
    m_input->clear();
    m_dateCheck->setChecked(false);
}

void TodoWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(event);
}