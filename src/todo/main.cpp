#include "TodoWindow.hpp"

#include <QApplication>
#include <QFont>

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};

    QFont font{"JetBrainsMono Nerd Font", 11};
    font.setHintingPreference(QFont::PreferNoHinting);
    app.setFont(font);

    app.setApplicationName("hyprtodo");
    app.setApplicationVersion(APP_VERSION);

    TodoWindow window;
    window.show();

    return app.exec();
}