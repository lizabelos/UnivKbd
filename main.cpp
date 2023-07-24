#include <QApplication>
#include "UnivKbd/UnivKbd"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // high dpi support on qt 5
#if QT_VERSION >= 0x050600 && QT_VERSION < 0x060000
    qDebug() << "Enabling high dpi support...";
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    //UnivKbd::Keyboard::exportOperatingSystemKeyboards();

    qDebug() << "Launching UnivKbd SimpleTextEditor...";
    UnivKbd::SimpleTextEditor editor;

    qDebug() << "Showing UnivKbd SimpleTextEditor...";
    editor.show();

    qDebug() << "Running UnivKbd SimpleTextEditor...";
    return app.exec();
}