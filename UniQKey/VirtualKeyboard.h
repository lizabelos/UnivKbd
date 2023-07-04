#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPointer>
#include <QLabel>
#include <QFont>
#include <QColor>
#include <QList>
#include <QComboBox>

#include <unordered_set>

#include "Keyboard.h"

namespace UniQKey {

    class VirtualKeyboardButton;

    class VirtualKeyboardButton : public QPushButton {
    Q_OBJECT

    public:
        VirtualKeyboardButton(const Key &key);
        ~VirtualKeyboardButton() override;

        void setCurrentKey(int index);

        inline int getCurrentKey() const {
            return mCurrentKey;
        }

    signals:
        void virtualKeyPressed(VirtualKeyboardButton &button, const Key &key);

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

        void onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key);

        void onVirtualRegularKeyPressed(VirtualKeyboardButton &button, const QChar &key);

    private:
        bool loadLayoutFromKeyboard(const Keyboard &keyboard);

        void addButtonFromKey(const Key &key);

    private:
        QWidget *mParent;
        QList<QPointer<VirtualKeyboardButton>> mButtons;

        QPointer<QVBoxLayout> mMainLayout;
        QPointer<QGridLayout> mKeyboardLayout;

        QPointer<QComboBox> mKeyboardSelector;

        unsigned char mKeyModifier = 0;
    };

}

#endif