/*
* --------------------------------------------------------------
* Project: UnivKbd
* Author: Liza Belos
* Year: 2023
* 
* Copyright (c) 2023. All rights reserved.
* This work is licensed under the terms of the MIT License.
* For a copy, see <https://opensource.org/licenses/MIT>.
* --------------------------------------------------------------
*
* NOTICE:
* This file is part of the original distribution of the UnivKbd project.
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
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPointer>
#include <QLabel>
#include <QFont>
#include <QColor>
#include <QList>
#include <QComboBox>
#include <QDockWidget>

#include <unordered_set>

#include "VirtualKeyboardButton.h"
#include "VirtualKeyboardInnerWidget.h"
#include "Keyboard.h"
#include "CustomDockWidget.h"

namespace UnivKbd {


    enum class VirtualKeyboardAttachMode {
        Auto,
        Floating,
        Docked
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
     * #include <UnivKbd/UnivKbd>
     *
     * QWidget *anyWidget = ...;
     * UnivKbd::VirtualKeyboard *keyboard = new VirtualKeyboard(anyWidget);
     * ```
     *
     * If you don't want the keyboard to float, you can add it to your widget's layout
     * using the `addWidget()` function:
     *
     * ```cpp
     * anyWidget->layout()->addWidget(keyboard);
     * ```
     *
     * During typing, show your own suggestions on top of the keyboard using the `setSuggestions()`
     * ```cpp
     * keyboard->setSuggestions(QStringList() << "suggestion1" << "suggestion2");
     * connect(keyboard, &VirtualKeyboard::suggestionPressed, this, &MyWidget::onSuggestionPressed);
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
        VirtualKeyboard(QWidget *parent = nullptr, VirtualKeyboardAttachMode attachMode = VirtualKeyboardAttachMode::Auto);

        /**
         * @brief Returns the Qt keyboard modifiers.
         *
         * @return The keyboard modifiers indicating which modifier keys are currently pressed (Ctrl, Alt, Shift).
         */
        inline Qt::KeyboardModifiers getModifiers() const {
            return gInnerWidget->getModifiers();
        }

        /**
         * @brief Automatically attaches the keyboard to the current window as a dock widget.
         */
        void attachToCurrentWindowAsDockWidget();

        /**
         * @brief Set the suggestions words to be displayed on top of the keyboard.
         */
        inline void setSuggestions(const QStringList &suggestions) {
            gCurrentKeyboard->setSuggestions(suggestions);
        }

    signals:
        /**
         * @brief This signal is emitted when a suggestion is pressed on the virtual keyboard.
         *
         * @param suggestion The suggestion that was pressed.
         */
        void suggestionPressed(const QString &suggestion);

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

    protected:
        void findWindowAndAttachDockWidget();

    private slots:
        void parentTakeFocus();

        void parentLooseFocus();

        void onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key);

        void onSpecialKeyPressed(VirtualKeyboardButton &button, const Key &key, const QString &special);

        void onAppFocusChanged(QObject *old, QObject *now);

    private:
        QWidget *mParent;
        VirtualKeyboardAttachMode mAttachMode;

        static QPointer<VirtualKeyboardInnerWidget> gInnerWidget;
        static QPointer<CustomDockWidget> gDockWidget;
        static QPointer<VirtualKeyboard> gCurrentKeyboard;
        static QMainWindow *gCurrentWindow;

        unsigned long mKeyModifier = 0;
        QKeySequence mKeySequence;

        bool mIsEnabled = true;
    };

}

#endif