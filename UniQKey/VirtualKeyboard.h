/*
* --------------------------------------------------------------
* Project: UniQKey
* Author: Liza Belos
* Year: 2023
* 
* Copyright (c) 2023. All rights reserved.
* This work is licensed under the terms of the MIT License.
* For a copy, see <https://opensource.org/licenses/MIT>.
* --------------------------------------------------------------
*
* NOTICE:
* This file is part of the original distribution of the UniQKey project. 
* All changes and redistributions of this file must retain this notice, 
* the list of contributors, and the entire copyright notice including the
* MIT License information.
* 
* DISCLAIMER:
* This software is provided 'as-is', without any express or implied warranty. 
* In no event will the authors be held liable for any damages arising from 
* the use of this software.
*/

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

    /**
     * @class VirtualKeyboard
     *
     * @brief Represents a virtual keyboard that can be used as an input method for widgets.
     *
     * The VirtualKeyboard class provides a customizable on-screen keyboard that can be used
     * as an input method for widgets in Qt applications. It can be used as a floating keyboard
     * that shows up when a widget is focused, or it can be added directly to a widget's layout.
     *
     * To create a floating keyboard that shows up when a widget is focused, you can use the
     * following code:
     *
     * ```cpp
     * #include <UniQKey/UniQKey>
     *
     * QWidget *anyWidget = ...;
     * UniQKey::VirtualKeyboard *keyboard = new VirtualKeyboard(anyWidget);
     * ```
     *
     * If you don't want the keyboard to float, you can add it to your widget's layout
     * using the `addWidget()` function:
     *
     * ```cpp
     * anyWidget->layout()->addWidget(keyboard);
     * ```
     */
    class VirtualKeyboard : public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor for the VirtualKeyboard class.
         *
         * @param parent The parent widget with which the keyboard will be associated. When this widget will be in focus, the keyboard will be shown. automatically
         */
        VirtualKeyboard(QWidget *parent = nullptr);

        /**
         * @brief Returns the Qt keyboard modifiers.
         *
         * @return The keyboard modifiers indicating which modifier keys are currently pressed (Ctrl, Alt, Shift).
         */
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
        /**
         * @brief Sets the enabled state of the virtual keyboard.
         *
         * @param enabled The enabled state of the virtual keyboard.
         */
        void setEnabled(bool enabled);

        /**
         * @brief Triggers the setEnabled() slot to enable or disable the virtual keyboard.
         */
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