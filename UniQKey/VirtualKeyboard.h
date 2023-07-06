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

        inline const Key &getKey() const {
            return mKey;
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

        inline Qt::KeyboardModifiers getModifiers() const {
            Qt::KeyboardModifiers modifiers = Qt::NoModifier;
            if (isModifierPressed(KeyType::CTRL)) {
                modifiers |= Qt::ControlModifier;
            }
            if (isModifierPressed(KeyType::ALT)) {
                modifiers |= Qt::AltModifier;
            }
            if (isModifierPressed(KeyType::SHIFT)) {
                modifiers |= Qt::ShiftModifier;
            }
            return modifiers;
        }

    public slots:
        void setEnabled(bool enabled);

        void triggerSetEnabled();

    private slots:
    
        void parentTakeFocus();

        void parentLooseFocus();

        void onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key);

    private:
        bool loadLayoutFromKeyboard(const Keyboard &keyboard);

        void addButtonFromKey(const Key &key);

        inline void pressModifier(const Key &key) {
            mKeyModifier ^= (unsigned long)1 << (int)key.getType();
        }

        inline bool isModifierPressed(const Key &key) const {
            return (mKeyModifier & ((unsigned long)1 << (int)key.getType())) != 0;
        }

        inline int currentKeyType() const {
            return (mKeyModifier & ((unsigned long)1 << (int)KeyType::SHIFT)) + (mKeyModifier & ((unsigned long)1 << (int)KeyType::ALT));
        }

    private:
        QWidget *mParent;
        QList<QPointer<VirtualKeyboardButton>> mButtons;

        QPointer<QVBoxLayout> mMainLayout;
        QPointer<QGridLayout> mKeyboardLayout;

        QPointer<QComboBox> mCountrySelector;
        QPointer<QComboBox> mLayoutSelector;

        QPointer<QPushButton> mOpenCloseButton;

        unsigned long mKeyModifier = 0;
        QKeySequence mKeySequence;

        bool mIsEnabled = true;
    };

}

#endif