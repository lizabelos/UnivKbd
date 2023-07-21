#include <QApplication>
#include "UniQKey/UniQKey"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    //UniQKey::Keyboard::exportOperatingSystemKeyboards();

    qDebug() << "Launching UniQKey SimpleTextEditor...";
    UniQKey::SimpleTextEditor editor;

    qDebug() << "Showing UniQKey SimpleTextEditor...";
    editor.show();

    qDebug() << "Running UniQKey SimpleTextEditor...";
    return app.exec();
}