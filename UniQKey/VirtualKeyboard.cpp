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

#include "VirtualKeyboard.h"

#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>
#include <QMainWindow>
#include <QDockWidget>

#include <unordered_set>

UniQKey::VirtualKeyboardButton::VirtualKeyboardButton(const Key &key) : mKey(key) {

    switch (key.getType()) {

        case KeyType::REGULAR:
            mKeyString[0] = key.toString(0);
            mKeyString[1] = key.toString(1);
            mKeyString[2] = key.toString(2);

            break;

        default:
            mKeyString[0] = key.toString();
            mKeyString[1] = key.toString();
            mKeyString[2] = key.toString();
            setCheckable(true);
            break;

    }

    connect(this, &QPushButton::pressed, this, &VirtualKeyboardButton::virtualButtonPressed);

    setCurrentKey(0);

    // Set the default style for the button
    setStyleSheet("QPushButton {"
                  "background-color: #FFFFFF;"
                  "border: 1px solid #CCCCCC;"
                  // remove every margin and padding
                    "margin: 0px;"
                    "padding: 0px;"
                  "}"
                  "QPushButton:pressed {"
                  "background-color: #CCCCCC;"
                  "}"
                  "QPushButton:checked {"
                  "background-color: #FFCC99;"
                  "}"
                  "QPushButton:hover {"
                  "background-color: #CCCCCC;"
                  "}"
                  "QPushButton:checked:hover {"
                    "background-color: #AACC99;"
                  "}"
                  "QPushButton:disabled {"
                  "background-color: #FFFFFF;"
                  "}"
                  "QPushButton:disabled:hover {"
                  "background-color: #FFFFFF;"
                  "}"
                  "QPushButton:disabled:pressed {"
                  "background-color: #FFFFFF;"
                  "}"
                  "QPushButton:disabled:checked {"
                  "background-color: #FFFFFF;"
                  "}"
    );
}

UniQKey::VirtualKeyboardButton::~VirtualKeyboardButton() {

}

void UniQKey::VirtualKeyboardButton::virtualButtonPressed() {
    emit virtualKeyPressed(*this, mKey);
}

void UniQKey::VirtualKeyboardButton::setCurrentKey(int index) {
    int maxIndex = 0;
    for (; maxIndex < 2; maxIndex++) {
        if (mLabels[maxIndex + 1] == nullptr) {
            break;
        }
    }

    mCurrentKey = std::min(maxIndex, index);
    setText(mKeyString[index]);
}

void UniQKey::VirtualKeyboardButton::resizeEvent(QResizeEvent *event) {
    // Change the current and default font size to fit the button
    int fontSize = event->size().height() / 3;
    QFont font;
    font.setPixelSize(fontSize);
    setFont(font);

    QPushButton::resizeEvent(event);
}

UniQKey::VirtualKeyboard::VirtualKeyboard(QWidget *parent) : mParent(parent) {
    // This widget is always on top of the other widgets.
    // But it never gets focus.
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    mMainLayout = new QVBoxLayout();
    setLayout(mMainLayout);

    mFunctionsLayout = new QHBoxLayout();
    mMainLayout->addLayout(mFunctionsLayout);

    QList<QString> keyboardLayouts = Keyboard::listExportedKeyboards();
    if (keyboardLayouts.size() == 0) {
        throw std::runtime_error("No keyboard layouts found");
    }
    // add a combobox to select the keyboard layout
    mCountrySelector = new QComboBox();
    mFunctionsLayout->addWidget(mCountrySelector);
    for (auto keyboardLayout : keyboardLayouts) {
        mCountrySelector->addItem(keyboardLayout);
    }
    // set to English (United States) as default
    mCountrySelector->setCurrentText("US");


    mLayoutSelector = new QComboBox();
    mFunctionsLayout->addWidget(mLayoutSelector);
    for (auto layout : getKeyboardLayouts()) {
        mLayoutSelector->addItem(layout);
    }
    mLayoutSelector->setCurrentText(getKeyboardLayouts()[0]);

    connect(mCountrySelector, &QComboBox::currentTextChanged, [=](const QString &text) {
        Keyboard keyboard = Keyboard::importKeyboard(mCountrySelector->currentText(), mLayoutSelector->currentText());
        if (!loadLayoutFromKeyboard(keyboard)) {
            throw std::runtime_error("Failed to load the keyboard layout file.");
        }
    });

    connect(mLayoutSelector, &QComboBox::currentTextChanged, [=](const QString &text) {
        Keyboard keyboard = Keyboard::importKeyboard(mCountrySelector->currentText(), mLayoutSelector->currentText());
        if (!loadLayoutFromKeyboard(keyboard)) {
            throw std::runtime_error("Failed to load the keyboard layout file.");
        }
    });
    

    mKeyboardLayout = new QGridLayout();
    mKeyboardLayout->setSpacing(0);
    mMainLayout->addLayout(mKeyboardLayout);

    mOpenCloseButton = new QPushButton("Open/Close", this);
    mFunctionsLayout->addWidget(mOpenCloseButton);
    
    // connect to slot triggerSetEnabled
    connect(mOpenCloseButton, &QPushButton::clicked, this, &VirtualKeyboard::triggerSetEnabled);


    connect(qApp, &QApplication::focusChanged, [=](QWidget *old, QWidget *now) {
        if (now == this || old == this || now == mCountrySelector || now == mLayoutSelector) {
            return;
        }
        for (auto button : mButtons) {
            if (now == button || old == button) {
                return;
            }
        }
        if (now == mParent) {
            parentTakeFocus();
        } else if (old == mParent) {
            parentLooseFocus();
        }
    });

    Keyboard keyboard = Keyboard::importKeyboard(mCountrySelector->currentText(), mLayoutSelector->currentText());
    if (!loadLayoutFromKeyboard(keyboard)) {
        throw std::runtime_error("Failed to load the keyboard layout file.");
    }
}

bool UniQKey::VirtualKeyboard::loadLayoutFromKeyboard(const Keyboard& keyboard) {

    // empty the layout
    for (auto button : mButtons) {
        mKeyboardLayout->removeWidget(button);
        delete button;
    }
    mButtons.clear();

    for (const auto& key : keyboard.getKeys()) {
        addButtonFromKey(key);
    }
    return true;
}

void UniQKey::VirtualKeyboard::addButtonFromKey(const Key &key) {

    const int spanResolution = 4;

    int x = key.getX() * spanResolution;
    int y = key.getY() * spanResolution;
    int spanx = key.getXSpan() * spanResolution;
    int spany = key.getYSpan() * spanResolution;

    VirtualKeyboardButton *btn = new VirtualKeyboardButton(key);
    // fit the button to the size of the layout
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(btn, &VirtualKeyboardButton::virtualKeyPressed, this, &VirtualKeyboard::onVirtualKeyPressed);
    mKeyboardLayout->addWidget(btn, y, x, spany, spanx);
    mButtons.append(btn);
}

void UniQKey::VirtualKeyboard::onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key) {

    QKeyEvent *event;
    //QKeySequence mKeySequence;

    switch (key.getType()) {
        
    case KeyType::SHIFT:
    case KeyType::ALT:
    case KeyType::CTRL:
        pressModifier(key);
        refreshModifiers(&button);
        event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), "");
        //mKeySequence = QKeySequence(QKeyCombination(getModifiers()));
        break;

    default:
        if (key.getCharacters().size() == 0) {
            event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), key.toString());
        } else {
            event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), key.getCharacters()[button.getCurrentKey()]);
            mKeyModifier = 0;
            refreshModifiers(&button);
        }
        //mKeySequence = QKeySequence(QKeyCombination(getModifiers(), key.toQtKey()));
        break;

    }

    //QShortcutEvent *shortcutEvent = new QShortcutEvent(mKeySequence, 0, false);
    //QCoreApplication::postEvent(mParent, shortcutEvent);
    QCoreApplication::postEvent(mParent, event);

}


void UniQKey::VirtualKeyboard::parentTakeFocus() {
    //show();
}

void UniQKey::VirtualKeyboard::parentLooseFocus() {
    //hide();
}

void UniQKey::VirtualKeyboard::setEnabled(bool enabled) {
    mIsEnabled = enabled;
    if (enabled) {
        mOpenCloseButton->setText("Close");

        mMainLayout->removeWidget(mOpenCloseButton); // to keep the button at the bottom

        // add the selectors and the keyboard layout
        mMainLayout->addWidget(mCountrySelector);
        mMainLayout->addWidget(mLayoutSelector);
        mMainLayout->addLayout(mKeyboardLayout);
        mMainLayout->addWidget(mOpenCloseButton);

    } else {
        mOpenCloseButton->setText("Open");


        // hide the selectors and the keyboard layout. for that we are going to revmoe them from the layout
        mMainLayout->removeWidget(mCountrySelector);
        mMainLayout->removeWidget(mLayoutSelector);
        mMainLayout->removeItem(mKeyboardLayout);


    }
}

void UniQKey::VirtualKeyboard::triggerSetEnabled() {
    setEnabled(!mIsEnabled);
}

void UniQKey::VirtualKeyboard::attachToCurrentWindowAsDockWidget() {

    QTimer::singleShot(2000, [this]() {
        QObject *widget = mParent;
        QMainWindow *window = nullptr;
        qDebug() << "Looking for Main Window...";
        while (true) {
            if (widget == nullptr) {
                return;
            }
            window = qobject_cast<QMainWindow*>(widget);
            qDebug() << "Testing " << widget;
            if (window != nullptr) {
                qDebug() << "Found it !";
                break;
            }
            widget = widget->parent();
        }

        QDockWidget *keyboardDock = new QDockWidget;
        keyboardDock->setWidget(this);
        keyboardDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        keyboardDock->setWindowTitle("UniQKey Virtual Keyboard");
        window->addDockWidget(Qt::BottomDockWidgetArea, keyboardDock);
    });
}


void UniQKey::VirtualKeyboard::refreshModifiers(QObject *toIgnore) {
    for (auto button : mButtons) {
        if (button->getKey().getType() == KeyType::REGULAR) {
            button->setCurrentKey(currentKeyType());
        } else if (button != toIgnore) {
            button->setChecked(isModifierPressed(button->getKey()));
        }
    }
}

void UniQKey::VirtualKeyboard::resizeEvent(QResizeEvent *event) {
    // if the size is too small, hide the first line of the keyboard
    bool firstLineHidden = event->size().height() < 400;
    for (auto button : mButtons) {
        if (button->getKey().getY() != 0) {
            continue;
        }
        if (firstLineHidden) {
            // remove the button from the layout
            mKeyboardLayout->removeWidget(button);
        } else {
            // add the button to the layout
            mKeyboardLayout->addWidget(button, 0, button->getKey().getX() * 4, 4, button->getKey().getXSpan() * 4);
        }
    }
}
