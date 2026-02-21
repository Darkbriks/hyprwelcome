#pragma once

#include <QMap>
#include <QStringList>
#include <filesystem>

namespace fs = std::filesystem;

struct LockscreenConfig {
    QString wallpaper{"lockscreen.png"};
    int     timePosX{0},    timePosY{-180};
    int     datePosX{0},    datePosY{-110};
    int     inputPosX{0},   inputPosY{80};
    int     loginPosX{0},   loginPosY{140};
    int     timeFontSize{72};
    int     dateFontSize{18};
    int     labelFontSize{13};
    float   backgroundOpacity{0.6f};
};

struct ThemeColors {
    QString background;
    QString backgroundAlt;
    QString foreground;
    QString foregroundDim;
    QString border;
    QString borderActive;

    QMap<QString, QString> accents;

    QString buttonBackground;
    QString buttonBackgroundHover;
    QString buttonBackgroundPressed;
    QString buttonTextColor;
    QString buttonIconColor;
    int     buttonBorderRadius{10};
    int     buttonBorderWidth{1};

    LockscreenConfig lockscreen;

    QString name;
    QString variant;    // "dark" | "colored" | "light"
    QString wallpaper;
    QString absolutePath; // Absolute path to the theme directory

    [[nodiscard]] QString resolve(const QString& key) const;
};

struct ReloadEntry {
    fs::path templatePath;
    fs::path destination;
    QString  reloadCmd;
};

class ThemeManager {
    friend struct ThemeColors;
public:
    static ThemeColors loadTheme(const QString& name);

    static QStringList availableThemes();

    static void applyTheme(const QString& name);

    static QString generateStyleSheet(const ThemeColors& colors, int borderRadius = 10, int borderWidth  = 1);

private:
    static fs::path themesDir();
    static QString  expandHome(const QString& path);
    static QString  renderTemplate(const std::string& tmpl, const ThemeColors& colors, const std::string& wallpaperAbsString);
    static void execReload(const QString& cmd);
    static void setWallpaper(const fs::path& themeDir, const QString& wallpaperFile);
};