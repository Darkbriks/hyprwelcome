#include "MainWindow.hpp"

#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QApplication>
#include <stdexcept>

MainWindow::MainWindow(QWidget* parent)
    : QWidget{parent}
{
    try {
        m_config = ConfigLoader::load();
    } catch (const std::exception& e) {
        m_config.window.title = "HyprWelcome";
    }

    try {
        m_colors = ThemeManager::loadTheme(m_config.currentTheme);
    } catch (const std::exception&) {
        m_colors.background          = "#1E2832";
        m_colors.backgroundAlt       = "#2C3E50";
        m_colors.foreground          = "#E8E4DF";
        m_colors.border              = "#3D4E5C";
        m_colors.borderActive        = "#86BBD8";
        m_colors.buttonBackground    = "#2C3E50";
        m_colors.buttonBackgroundHover    = "#3D4E5C";
        m_colors.buttonBackgroundPressed  = "#1E2832";
        m_colors.buttonTextColor     = "#E8E4DF";
        m_colors.buttonBorderRadius  = 10;
        m_colors.buttonBorderWidth   = 1;
    }

    setWindowTitle(m_config.window.title);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setObjectName("MainWidget");
    resize(m_config.window.width, m_config.window.height);

    const auto* screen = QApplication::primaryScreen();
    if (screen) {
        const auto geo = screen->availableGeometry();
        move(geo.center() - rect().center());
    }

    buildUI();
    applyTheme(m_colors);
}

void MainWindow::buildUI() {
    auto* root = new QVBoxLayout{this};
    root->setContentsMargins(m_config.window.padding,
                             m_config.window.padding,
                             m_config.window.padding,
                             m_config.window.padding);
    root->setSpacing(m_config.window.spacing);

    auto* title = new QLabel{m_config.window.title, this};
    title->setObjectName("TitleLabel");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    m_grid = new QGridLayout{};
    m_grid->setSpacing(m_config.window.spacing);

    const int cols = m_config.window.columns;
    int row = 0, col = 0;

    for (const auto& action : m_config.actions) {
        auto* btn = new ActionButton{action, m_colors, this, m_config.subtitleRefreshIntervalMs, m_config.subtitleCmdTimeoutMs};
        btn->setMinimumHeight(m_config.window.buttonHeight);

        if (action.command == "__theme_cycle__")
            connect(btn, &QPushButton::clicked, this, &MainWindow::cycleTheme);

        m_grid->addWidget(btn, row, col);
        m_buttons.push_back(btn);

        if (++col >= cols) { col = 0; ++row; }
    }

    root->addLayout(m_grid);
}

void MainWindow::applyTheme(const ThemeColors& colors) {
    setStyleSheet(QString{R"(
        QWidget#MainWidget {
            background-color: %1;
            border-radius: 14px;
            border: 1px solid %2;
        }
        QLabel#TitleLabel {
            color: %3;
            font-size: 15px;
            font-weight: bold;
            padding-bottom: 4px;
        }
    )"}
    .arg(colors.background)
    .arg(colors.borderActive)
    .arg(colors.foreground));

    for (auto* btn : m_buttons)
        btn->applyTheme(colors);
}

void MainWindow::cycleTheme() {
    const auto themes = ThemeManager::availableThemes();
    if (themes.isEmpty()) return;

    int idx = themes.indexOf(m_config.currentTheme);
    idx = (idx + 1) % themes.size();

    const QString next = themes[idx];
    try {
        ThemeManager::applyTheme(next);
        m_config.currentTheme = next;
        m_colors = ThemeManager::loadTheme(next);
        applyTheme(m_colors);
    } catch (const std::exception&) {
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(event);
}