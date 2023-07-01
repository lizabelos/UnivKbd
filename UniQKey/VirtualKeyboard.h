#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QLabel>

#include "Keyboard.h"

namespace UniQKey {

    class VirtualKeyboardButton : public QPushButton {
    Q_OBJECT

    public:
        VirtualKeyboardButton(const Key &key);
        ~VirtualKeyboardButton() override;

        void setCurrentKey(int index);

    signals:
        void virtualKeyPressed(QChar key);

    private slots:
        void virtualButtonPressed();

    private:
        Key mKey;
        QString mKeyString[3];

        QPointer<QGridLayout> mGridLayout;
        QPointer<QLabel> mLabels[3];

        QFont mCurrentFont, mDefaultFont;
        QColor mCurrentColor, mDefaultColor;

        int mCurrentKey;

    };

    class VirtualKeyboard : public QWidget {
    Q_OBJECT

    public:
        VirtualKeyboard(QWidget *parent = nullptr);

    private slots:

        void parentTakeFocus();

        void parentLooseFocus();

        void virtualKeyPressed(QChar key);

    private:
        bool loadLayoutFromKeyboard(const Keyboard &keyboard);

        void addButtonFromKey(const Key &key);

    private:
        QWidget *mParent;
        QList<QPointer<VirtualKeyboardButton>> mButtons;
        QPointer<QGridLayout> mGridLayout;

        bool mIsShiftPressed = false;
        bool mIsAltGrPressed = false;
        bool mIsCtrlPressed = false;
    };

}

#endif