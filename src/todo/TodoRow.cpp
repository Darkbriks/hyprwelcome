#include "TodoRow.hpp"

TodoRow::TodoRow(const TodoItem& item,
                 const ThemeColors& colors,
                 QWidget* parent)
    : QWidget{parent}
    , m_id{item.id}
{
    auto* layout = new QHBoxLayout{this};
    layout->setContentsMargins(8, 4, 8, 4);
    layout->setSpacing(8);

    m_check = new QCheckBox{this};
    m_check->setChecked(item.completed);
    layout->addWidget(m_check);

    m_text = new QLabel{item.text, this};
    m_text->setWordWrap(true);
    m_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    if (item.completed) {
        // Texte barré si complété
        QFont f = m_text->font();
        f.setStrikeOut(true);
        m_text->setFont(f);
        m_text->setStyleSheet(QString{"color: %1;"}.arg(colors.foregroundDim));
    }

    layout->addWidget(m_text);

    if (item.dueDate.has_value()) {
        const QString dateStr = item.dueDate->toString("dd/MM");
        m_dueLabel = new QLabel{dateStr, this};
        m_dueLabel->setObjectName("DueLabel");
        m_dueLabel->setFixedWidth(44);
        m_dueLabel->setAlignment(Qt::AlignCenter);
        applyUrgencyStyle(item.urgencyClass(), colors);
        layout->addWidget(m_dueLabel);
    }

    m_deleteBtn = new QPushButton{"󰅖", this};
    m_deleteBtn->setFixedSize(24, 24);
    m_deleteBtn->setFlat(true);
    m_deleteBtn->setObjectName("DeleteBtn");
    m_deleteBtn->setStyleSheet(QString{R"(
        QPushButton#DeleteBtn {
            color: %1;
            border: none;
            background: transparent;
            font-size: 14px;
        }
        QPushButton#DeleteBtn:hover {
            color: %2;
        }
    )"}.arg(colors.foregroundDim).arg(colors.accents.value("accent_danger", "#F4A6B8")));
    layout->addWidget(m_deleteBtn);

    connect(m_check, &QCheckBox::toggled, this, [this](bool checked) {
        emit completionToggled(m_id, checked);
    });
    connect(m_deleteBtn, &QPushButton::clicked, this, [this]() {
        emit deleteRequested(m_id);
    });
}

void TodoRow::applyUrgencyStyle(const QString& urgencyClass,
                                const ThemeColors& colors) {
    if (!m_dueLabel) return;

    QString color;
    if      (urgencyClass == "overdue") color = colors.accents.value("accent_danger",    "#F4A6B8");
    else if (urgencyClass == "urgent")  color = colors.accents.value("accent_danger",    "#F4A6B8");
    else if (urgencyClass == "soon")    color = colors.accents.value("accent_warning",   "#F4D58D");
    else                                color = colors.accents.value("accent_success",   "#A8D5BA");

    m_dueLabel->setStyleSheet(QString{R"(
        QLabel#DueLabel {
            color: %1;
            font-size: 10px;
            border: 1px solid %1;
            border-radius: 4px;
            padding: 1px 3px;
        }
    )"}.arg(color));
}