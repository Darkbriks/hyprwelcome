#pragma once

#include "TodoItem.hpp"
#include "../panel/ThemeManager.hpp"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class TodoRow : public QWidget {
    Q_OBJECT

public:
    explicit TodoRow(const TodoItem& item,
                     const ThemeColors& colors,
                     QWidget* parent = nullptr);

    signals:
        void completionToggled(int id, bool completed);
    void deleteRequested(int id);

private:
    int          m_id;
    QCheckBox*   m_check{nullptr};
    QLabel*      m_text{nullptr};
    QLabel*      m_dueLabel{nullptr};
    QPushButton* m_deleteBtn{nullptr};

    void applyUrgencyStyle(const QString& urgencyClass, const ThemeColors& colors);
};