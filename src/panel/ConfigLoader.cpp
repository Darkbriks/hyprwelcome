#include "ConfigLoader.hpp"

#include <toml++/toml.hpp>
#include <fstream>
#include <stdexcept>
#include <cstdlib>

fs::path ConfigLoader::configDir() {
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"); xdg && *xdg)
        return fs::path{xdg} / "hyprwelcome";

    if (const char* home = std::getenv("HOME"); home && *home)
        return fs::path{home} / ".config" / "hyprwelcome";

    throw std::runtime_error{"Impossible de déterminer le répertoire de config (HOME non défini)"};
}

AppConfig ConfigLoader::load() {
    const auto configPath = configDir() / "config.toml";

    if (!fs::exists(configPath))
        throw std::runtime_error{"Config introuvable : " + configPath.string()};

    const auto root = toml::parse_file(configPath.string());

    AppConfig cfg;

    if (const auto app = root["app"].as_table()) {
        cfg.subtitleRefreshIntervalMs = app->get_as<int64_t>("subtitle_refresh")->value_or(5) * 1000;  // convertit en ms
        cfg.subtitleCmdTimeoutMs = app->get_as<int64_t>("subtitle_cmd_timeout")->value_or(4) * 1000;  // convertit en ms
    }

    if (const auto win = root["window"].as_table()) {
        cfg.window.width        = win->get_as<int64_t>("width")->value_or(640);
        cfg.window.height       = win->get_as<int64_t>("height")->value_or(480);
        cfg.window.columns      = win->get_as<int64_t>("columns")->value_or(3);
        cfg.window.spacing      = win->get_as<int64_t>("spacing")->value_or(12);
        cfg.window.padding      = win->get_as<int64_t>("padding")->value_or(20);
        cfg.window.buttonHeight = win->get_as<int64_t>("button_height")->value_or(90);

        if (auto t = win->get_as<std::string>("title"))
            cfg.window.title = QString::fromStdString(t->get());
    }

    if (const auto theme = root["theme"].as_table()) {
        if (auto current = theme->get_as<std::string>("current"))
            cfg.currentTheme = QString::fromStdString(current->get());
    }

    if (const auto actions = root["actions"].as_array()) {
        for (const auto& node : *actions) {
            if (const auto tbl = node.as_table())
                cfg.actions.push_back(parseAction(*tbl));
        }
    }

    return cfg;
}

ActionConfig ConfigLoader::parseAction(const toml::table& tbl) {
    ActionConfig action;

    if (auto v = tbl.get_as<std::string>("label"))
        action.label = QString::fromStdString(v->get());

    if (auto v = tbl.get_as<std::string>("icon"))
        action.icon = QString::fromStdString(v->get());

    if (auto v = tbl.get_as<std::string>("command"))
        action.command = QString::fromStdString(v->get());

    if (auto v = tbl.get_as<std::string>("color"))
        action.color = QString::fromStdString(v->get());

    if (auto v = tbl.get_as<std::string>("subtitle"))
        action.subtitle = QString::fromStdString(v->get());

    if (auto v = tbl.get_as<bool>("confirm"))
        action.confirm = v->get();

    return action;
}

void ConfigLoader::saveCurrentTheme(const QString& themeName) {
    const auto configPath = configDir() / "config.toml";

    if (!fs::exists(configPath))
        return;

    std::ifstream in{configPath};
    std::string content{std::istreambuf_iterator<char>(in),
                        std::istreambuf_iterator<char>()};
    in.close();

    const std::string marker{"current = \""};
    auto pos = content.find(marker);
    if (pos != std::string::npos) {
        auto end = content.find('"', pos + marker.size());
        if (end != std::string::npos)
            content.replace(pos + marker.size(),
                            end - pos - marker.size(),
                            themeName.toStdString());
    }

    std::ofstream out{configPath};
    out << content;
}