#include "MainWindow.hpp"

#include <QApplication>
#include <QFont>

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};

    QFont font{"JetBrainsMono Nerd Font", 11};
    font.setHintingPreference(QFont::PreferNoHinting);
    app.setFont(font);

    app.setApplicationName("hyprwelcome");
    app.setApplicationVersion(APP_VERSION);

    MainWindow window;
    window.show();

    return app.exec();
}