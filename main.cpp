#include <QApplication>
#include "UniQKey/UniQKey"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // high dpi support on qt 5
#if QT_VERSION >= 0x050600 && QT_VERSION < 0x060000
    qDebug() << "Enabling high dpi support...";
    app.setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    //UniQKey::Keyboard::exportOperatingSystemKeyboards();

    qDebug() << "Launching UniQKey SimpleTextEditor...";
    UniQKey::SimpleTextEditor editor;

    qDebug() << "Showing UniQKey SimpleTextEditor...";
    editor.show();

    qDebug() << "Running UniQKey SimpleTextEditor...";
    return app.exec();
}