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

#ifndef VirtualKeyboardInnerWidget_H
#define VirtualKeyboardInnerWidget_H

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPointer>
#include <QLabel>
#include <QFont>
#include <QColor>
#include <QList>
#include <QComboBox>
#include <QStackedLayout>

#include <unordered_set>

#include "VirtualKeyboardButton.h"
#include "Keyboard.h"
#include "VirtualKeyboardConfigurationWidget.h"

namespace UniQKey {

    /**
     * @class VirtualKeyboardInnerWidget
     *
     * @brief Represents the inner widget of the virtual keyboard.
     *
     * @see VirtualKeyboard
     */
    class VirtualKeyboardInnerWidget : public QWidget {
    Q_OBJECT

    public:
        /**
         * @brief Constructor for the VirtualKeyboardInnerWidget class.
         *
         * @param parent The parent widget with which the keyboard will be associated. When this widget will be in focus, the keyboard will be shown. automatically
         */
        VirtualKeyboardInnerWidget();

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
            if (isModifierPressed(KeyType::SHIFT) || isModifierPressed(KeyType::CAPS_LOCK)) {
                modifiers |= Qt::ShiftModifier;
            }
            return modifiers;
        }

        /**
         * @brief Set the suggestions words to be displayed on top of the keyboard.
         */
        inline void setSuggestions(const QStringList &suggestions) {
            for (int i = 0; i < std::min((int)3, (int)suggestions.size()); i++) {
                mSuggestionButtons[i]->setText(suggestions[i]);
            }
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

    signals:
        /**
         * @brief This signal is emitted when a key is pressed on the virtual keyboard.
         *
         * @param button The button that was pressed.
         * @param key The associated key to the button that was pressed.
         */
        void virtualKeyPressed(VirtualKeyboardButton &button, const Key &key);

        /**
         * @brief This signal is emitted when a suggestion is pressed on the virtual keyboard.
         *
         * @param suggestion The suggestion that was pressed.
         */
        void suggestionPressed(const QString &suggestion);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private slots:
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

        inline unsigned long currentKeyType() const {
            unsigned long type = 0;
            if (isModifierPressed(KeyType::SHIFT) || isModifierPressed(KeyType::CAPS_LOCK)) {
                type += (unsigned long)KeyType::SHIFT + 1;
            }
            if (isModifierPressed(KeyType::ALT)) {
                type += (unsigned long)KeyType::ALT + 1;
            }
            return type;
        }

        void refreshModifiers(QObject *toIgnore = nullptr);

    private:
        QList<QPointer<VirtualKeyboardButton>> mButtons;

        QPointer<QStackedLayout> mMainLayout;
        QPointer<QWidget> mKeyboardWidget;
        QPointer<QVBoxLayout> mKeyboardWithSuggestionsLayout;
        QPointer<QGridLayout> mKeyboardLayout;
        QPointer<VirtualKeyboardConfigurationWidget> mConfigurationWidget;
        QPointer<QPushButton> mOpenButton;
        QPointer<QHBoxLayout> mSuggestionsLayout;
        QPointer<QPushButton> mSuggestionButtons[3];

        unsigned long mKeyModifier = 0;
        QKeySequence mKeySequence;

        bool mIsEnabled = true;
    };

}

#endif