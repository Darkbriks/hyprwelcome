#pragma once

#include "ConfigLoader.hpp"
#include "ThemeManager.hpp"
#include "ActionButton.hpp"

#include <QWidget>
#include <QGridLayout>
#include <QList>

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    AppConfig           m_config;
    ThemeColors         m_colors;
    QGridLayout*        m_grid{nullptr};
    QList<ActionButton*> m_buttons;

    void buildUI();
    void applyTheme(const ThemeColors& colors);
    void cycleTheme();

    void keyPressEvent(QKeyEvent* event) override;
};