#ifndef SIMPLETEXTEDITOR_H
#define SIMPLETEXTEDITOR_H

#include <QApplication>
#include <QLabel>
#include <QTextEdit>
#include <QMainWindow>
#include <QPointer>
#include <QAction>
#include <QTextDocument>
#include <QGridLayout>
#include <QSpacerItem>
#include <QMenuBar>
#include <QToolBar>
#include <QTextCharFormat>
#include <QStatusBar>
#include <QSlider>
#include <QWheelEvent>
#include <QScrollArea>


namespace UniQKey {

    class SimpleTextEditor : public QMainWindow {
    Q_OBJECT

    public:
        SimpleTextEditor(QWidget *parent = 0);

    protected:
        void wheelEvent(QWheelEvent *event) override;

    private slots:

        void makeTextBold();

        void makeTextItalic();

        void makeTextUnderlined();

        void updateFormatActions(const QTextCharFormat &format);

        void zoom(int level);

        void zoomIn();

        void zoomOut();

    private:
        QPointer<QTextEdit> mTextEditor;
        QPointer<QTextDocument> mTextDocument;

        QPointer<QAction> boldAction;
        QPointer<QAction> italicAction;
        QPointer<QAction> underlineAction;

        qreal mZoomFactor = 1.0;

    };

}


#endif // SIMPLETEXTEDITOR_H