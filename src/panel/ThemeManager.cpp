#include "ThemeManager.hpp"
#include "ConfigLoader.hpp"

#include <toml++/toml.hpp>
#include <QProcess>
#include <QDir>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <regex>
#include <cstdlib>

QString ThemeColors::resolve(const QString& key) const {
    if (accents.contains(key))
        return accents[key];

    if (key == "background")      return background;
    if (key == "background_alt")  return backgroundAlt;
    if (key == "foreground")      return foreground;
    if (key == "border_active")   return borderActive;

    return buttonBackground;
}

// ─── Helpers privés ───────────────────────────────────────────────────────

fs::path ThemeManager::themesDir() {
    return ConfigLoader::configDir() / "themes";
}

QString ThemeManager::expandHome(const QString& path) {
    if (!path.startsWith('~'))
        return path;

    const char* home = std::getenv("HOME");
    if (!home) return path;

    return QString::fromUtf8(home) + path.sliced(1);
}

QString ThemeManager::renderTemplate(const std::string& tmpl, const ThemeColors& colors, const std::string& wallpaperAbsString) {
    std::regex  pattern{R"(\{\{(\w+)(?:\|(\w+))?\}\})"};

    std::string out;
    auto begin = std::sregex_iterator(tmpl.begin(), tmpl.end(), pattern);
    auto end   = std::sregex_iterator{};

    std::size_t lastPos = 0;
    for (auto it = begin; it != end; ++it) {
        const auto& match = *it;
        out += tmpl.substr(lastPos, match.position() - lastPos);

        const QString key      = QString::fromStdString(match[1].str());
        const QString modifier = QString::fromStdString(match[2].str());
        QString       value    = colors.resolve(key);

        if (key == "wallpaper_path") {
            value = QString::fromStdString(wallpaperAbsString);
        }

        if (modifier == "raw" && value.startsWith('#'))
            value = value.sliced(1);  // retire le #

        out += value.toStdString();
        lastPos = match.position() + match.length();
    }
    out += tmpl.substr(lastPos);
    return QString::fromStdString(out);
}

void ThemeManager::execReload(const QString& cmd) {
    if (cmd.isEmpty()) return;
    QProcess::startDetached("sh", {"-c", cmd});
}

void ThemeManager::setWallpaper(const fs::path& themeDir, const QString& wallpaperFile) {
    if (wallpaperFile.isEmpty()) return;
    const auto wp = themeDir / wallpaperFile.toStdString();
    if (!fs::exists(wp)) return;

    const QString cmd = QString{"hyprctl hyprpaper wallpaper \",\""}
                        .replace(",", QString::fromStdString(wp.string()));
    QProcess::startDetached("sh", {"-c", cmd});
}

ThemeColors ThemeManager::loadTheme(const QString& name) {
    const auto colorsPath = themesDir() / name.toStdString() / "colors.toml";

    if (!fs::exists(colorsPath))
        throw std::runtime_error{"Thème introuvable : " + name.toStdString()};

    const auto root = toml::parse_file(colorsPath.string());

    ThemeColors tc;

    if (const auto meta = root["meta"].as_table()) {
        if (auto v = meta->get_as<std::string>("name"))
            tc.name = QString::fromStdString(v->get());
        if (auto v = meta->get_as<std::string>("variant"))
            tc.variant = QString::fromStdString(v->get());
        if (auto v = meta->get_as<std::string>("wallpaper"))
            tc.wallpaper = QString::fromStdString(v->get());
    }

    if (const auto colors = root["colors"].as_table()) {
        auto str = [&](const char* key, QString& target) {
            if (auto v = colors->get_as<std::string>(key))
                target = QString::fromStdString(v->get());
        };
        str("background",     tc.background);
        str("background_alt", tc.backgroundAlt);
        str("foreground",     tc.foreground);
        str("foreground_dim", tc.foregroundDim);
        str("border",         tc.border);
        str("border_active",  tc.borderActive);

        for (const auto& [k, v] : *colors) {
            const auto key = QString::fromStdString(std::string{k});
            if (key.startsWith("accent_") or key.startsWith("color_")) { // Les 16 couleurs de base sont regroupées avec les accents par commodité
                if (const auto sv = v.as_string())
                    tc.accents[key] = QString::fromStdString(sv->get());
            }
        }
    }

    if (const auto btn = root["button"].as_table()) {
        auto str = [&](const char* key, QString& target) {
            if (auto v = btn->get_as<std::string>(key))
                target = QString::fromStdString(v->get());
        };
        str("background",          tc.buttonBackground);
        str("background_hover",    tc.buttonBackgroundHover);
        str("background_pressed",  tc.buttonBackgroundPressed);
        str("text_color",          tc.buttonTextColor);
        str("icon_color",          tc.buttonIconColor);

        tc.buttonBorderRadius = btn->get_as<int64_t>("border_radius")->value_or(10);
        tc.buttonBorderWidth  = btn->get_as<int64_t>("border_width")->value_or(1);
    }

    return tc;
}

QStringList ThemeManager::availableThemes() {
    QStringList themes;
    const auto dir = themesDir();
    if (!fs::exists(dir)) return themes;

    for (const auto& entry : fs::directory_iterator(dir)) {
        if (!entry.is_directory()) continue;
        if (fs::exists(entry.path() / "colors.toml"))
            themes << QString::fromStdString(entry.path().filename().string());
    }
    themes.sort();
    return themes;
}

void ThemeManager::applyTheme(const QString& name) {
    const auto themeDir  = themesDir() / name.toStdString();
    const auto colorsPath = themeDir / "colors.toml";

    if (!fs::exists(colorsPath))
        throw std::runtime_error{"Thème introuvable : " + name.toStdString()};

    const auto colors = loadTheme(name);
    const auto root   = toml::parse_file(colorsPath.string());

    const fs::path wallpaperAbs = themeDir / colors.wallpaper.toStdString();

    if (const auto reloadArr = root["reload"].as_array()) {
        for (const auto& node : *reloadArr) {
            const auto tbl = node.as_table();
            if (!tbl) continue;

            auto getStr = [&](const char* key) -> std::string {
                if (auto v = tbl->get_as<std::string>(key))
                    return v->get();
                return {};
            };

            const fs::path tmplPath   = themeDir / getStr("template");
            const QString  destRaw    = expandHome(QString::fromStdString(getStr("destination")));
            const QString  reloadCmd  = QString::fromStdString(getStr("reload_cmd"));

            if (!fs::exists(tmplPath)) continue;

            std::ifstream in{tmplPath};
            const std::string tmplContent{std::istreambuf_iterator<char>(in),
                                          std::istreambuf_iterator<char>()};

            const QString rendered = renderTemplate(tmplContent, colors, wallpaperAbs.string());

            const fs::path destPath{destRaw.toStdString()};
            fs::create_directories(destPath.parent_path());

            std::ofstream out{destPath};
            out << rendered.toStdString();
            out.close();

            execReload(reloadCmd);
        }
    }

    setWallpaper(themeDir, colors.wallpaper);

    ConfigLoader::saveCurrentTheme(name);
}

QString ThemeManager::generateStyleSheet(const ThemeColors& c,
                                          int borderRadius,
                                          int borderWidth) {
    return QString{R"(
        QWidget#MainWidget {
            background-color: %1;
            border-radius: %2px;
        }
        QLabel#TitleLabel {
            color: %3;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton.ActionButton {
            background-color: %4;
            color: %5;
            border: %6px solid %7;
            border-radius: %2px;
            font-size: 13px;
            padding: 8px;
        }
        QPushButton.ActionButton:hover {
            background-color: %8;
            border-color: %9;
        }
        QPushButton.ActionButton:pressed {
            background-color: %10;
        }
    )"}
    .arg(c.background)
    .arg(borderRadius)
    .arg(c.foreground)
    .arg(c.buttonBackground)
    .arg(c.buttonTextColor)
    .arg(borderWidth)
    .arg(c.border)
    .arg(c.buttonBackgroundHover)
    .arg(c.borderActive)
    .arg(c.buttonBackgroundPressed);
}