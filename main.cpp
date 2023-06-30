#include <QApplication>
#include "UniQKey/UniQKey"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    UniQKey::SimpleTextEditor editor;
    editor.show();

    return app.exec();
}
