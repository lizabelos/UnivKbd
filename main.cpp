#include <QApplication>

#include "SimpleTextEditor.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    SimpleTextEditor editor;
    editor.show();

    return app.exec();
}
