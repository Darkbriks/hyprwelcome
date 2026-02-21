#pragma once

#include "TodoStore.hpp"
#include "../panel/ThemeManager.hpp"
#include "../panel/ConfigLoader.hpp"

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QKeyEvent>

class TodoWindow : public QWidget {
    Q_OBJECT

public:
    explicit TodoWindow(QWidget* parent = nullptr);

private:
    TodoStore*   m_store{nullptr};
    ThemeColors  m_colors;

    QLabel*      m_decoration{nullptr};
    QScrollArea* m_scrollArea{nullptr};
    QWidget*     m_listWidget{nullptr};
    QVBoxLayout* m_listLayout{nullptr};
    QLineEdit*   m_input{nullptr};
    QDateEdit*   m_dateEdit{nullptr};
    QCheckBox*   m_dateCheck{nullptr};
    QPushButton* m_addBtn{nullptr};

    void buildUI();
    void rebuildList();
    void applyTheme();
    void addFromInput();
    void keyPressEvent(QKeyEvent* event) override;

    static QString themeDecoration(const ThemeColors& colors);
};