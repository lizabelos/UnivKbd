#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QPointer>
#include <QLabel>

namespace UniQKey {

    class VirtualKeyboardButton : public QPushButton {
    Q_OBJECT

    public:
        VirtualKeyboardButton(QChar mainkey, QChar shiftkey, QChar altgrkey);

        VirtualKeyboardButton(Qt::KeyboardModifier modifier);

        void updateModifiers(bool shift, bool altgr, bool isctrl);

    signals:

        void virtualKeyPressed(QChar key);

        void virtualModifierKeyPressed(Qt::KeyboardModifier modifier);

    private slots:

        void virtualButtonPressed();

    private:
        QChar mMainkey, mShiftkey, mAltgrkey, mCurrentKey;
        bool mIsModifier;
        Qt::KeyboardModifier mModifier;

        QPointer<QGridLayout> gridLayout;
        QPointer<QLabel> mainkeyLabel;
        QPointer<QLabel> shiftkeyLabel;
        QPointer<QLabel> altgrkeyLabel;

    };

    class VirtualKeyboard : public QWidget {
    Q_OBJECT

    public:
        VirtualKeyboard(QWidget *parent = nullptr);

    private slots:

        void parentTakeFocus();

        void parentLooseFocus();

        void virtualKeyPressed(QChar key);

        void virtualModifierKeyPressed(Qt::KeyboardModifier modifier);

    private:
        bool loadLayoutFromFile(const QString &fileName);

        void addButton(int x, int y, QChar mainkey, QChar shiftkey, QChar altgrkey, int spanx = 1, int spany = 1);

        void addButton(int x, int y, Qt::KeyboardModifier modifier, int spanx = 1, int spany = 1);

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