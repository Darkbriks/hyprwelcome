#pragma once

#include <QString>
#include <QList>
#include <optional>
#include <filesystem>
#include <toml++/impl/table.hpp>

namespace fs = std::filesystem;

struct ActionConfig {
    QString label;
    QString icon;
    QString command;
    QString color;
    QString subtitle;
    bool    confirm{false};
};

struct WindowConfig {
    int     width{640};
    int     height{480};
    int     columns{3};
    int     spacing{12};
    int     padding{20};
    int     buttonHeight{90};
    QString title{"HyprWelcome"};
};

struct AppConfig {
    WindowConfig        window;
    QString             currentTheme;
    QList<ActionConfig> actions;
    int subtitleRefreshIntervalMs{5000};
    int subtitleCmdTimeoutMs{4000};
};

class ConfigLoader {
public:
    static fs::path configDir();

    static AppConfig load();

    static void saveCurrentTheme(const QString& themeName);

private:
    static ActionConfig parseAction(const toml::v3::table& tbl);
    static WindowConfig parseWindow(const auto& node);
};