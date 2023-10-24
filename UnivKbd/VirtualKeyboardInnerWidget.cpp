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

#include "VirtualKeyboardInnerWidget.h"

#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>
#include <QMainWindow>
#include <QDockWidget>
#include <QPainter>

#include <unordered_set>

UnivKbd::VirtualKeyboardInnerWidget::VirtualKeyboardInnerWidget() {
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    mMainLayout = new QStackedLayout();
    setLayout(mMainLayout);

    mKeyboardWithSuggestionsLayout = new QVBoxLayout();
    mKeyboardWithSuggestionsLayout->setSpacing(2);

    mSuggestionsLayout = new QHBoxLayout();
    mSuggestionsLayout->setSpacing(2);
    for (int i = 0; i < 10; i++) {
        mSuggestionButtons[i] = new QPushButton();
        mSuggestionsLayout->addWidget(mSuggestionButtons[i]);
        connect(mSuggestionButtons[i], &QPushButton::clicked, [=]() {
            if (mSuggestionButtons[i]->text() != "") {
                int numCharToRemove = std::min(mSuggestionButtons[i]->text().length(), mCurrentWord.length());
                QString newWord = mCurrentWord.right(numCharToRemove);

                emit suggestionPressed(mSuggestionButtons[i]->text(), newWord);

            }
        });
    }

    mKeyboardWithSuggestionsLayout->addLayout(mSuggestionsLayout);

    mKeyboardLayout = new QGridLayout();
    mKeyboardLayout->setSpacing(2);

    mKeyboardWithSuggestionsLayout->addLayout(mKeyboardLayout);

    mKeyboardWidget = new QWidget();
    mKeyboardWidget->setLayout(mKeyboardWithSuggestionsLayout);

    mMainLayout->addWidget(mKeyboardWidget);

    mConfigurationWidget = new VirtualKeyboardConfigurationWidget();
    mMainLayout->addWidget(mConfigurationWidget);
    connect(mConfigurationWidget, &VirtualKeyboardConfigurationWidget::requestKeyboard, [=](const QString &country, const QString &layout) {
        Keyboard keyboard = Keyboard::importKeyboard(country, layout);
        loadLayoutFromKeyboard(keyboard);
    });
    connect(mConfigurationWidget, &VirtualKeyboardConfigurationWidget::close, [=]() {
        mMainLayout->setCurrentWidget(mKeyboardWidget);
    });

    mOpenButton = new QPushButton();
    mOpenButton->setText("Open Keyboard");
    mMainLayout->addWidget(mOpenButton);
    connect(mOpenButton, &QPushButton::clicked, [=]() {
        mMainLayout->setCurrentWidget(mKeyboardWidget);
    });

    mMainLayout->setCurrentWidget(mKeyboardWidget);

    Keyboard keyboard = Keyboard::importKeyboard("US", "qwertyuiopasdfghjklzxcvbnm");
    loadLayoutFromKeyboard(keyboard);

    // fill mDictionary with work from :/dictionary.txt
    QFile dictionaryFile(":/dictionary.txt");
    if (dictionaryFile.open(QIODevice::ReadOnly)) {
        QTextStream in(&dictionaryFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            mDictionary << line;
        }
    } else {
        qDebug() << "Could not open dictionary file";
        exit(1);
    }

}

bool UnivKbd::VirtualKeyboardInnerWidget::loadLayoutFromKeyboard(const Keyboard& keyboard) {

    // empty the layout
    for (const auto& button : mButtons) {
        mKeyboardLayout->removeWidget(button);
        delete button;
    }
    mButtons.clear();

    for (const auto& key : keyboard.getKeys()) {
        addButtonFromKey(key);
    }

    return true;
}

void UnivKbd::VirtualKeyboardInnerWidget::addButtonFromKey(const Key &key) {

    const int spanResolution = 4;

    int x = key.getX() * spanResolution;
    int y = key.getY() * spanResolution;
    int spanx = key.getXSpan() * spanResolution;
    int spany = key.getYSpan() * spanResolution;

    VirtualKeyboardButton *btn = new VirtualKeyboardButton(key, nullptr, this);
    // fit the button to the size of the layout
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(btn, &VirtualKeyboardButton::virtualKeyPressed, this, &VirtualKeyboardInnerWidget::onVirtualKeyPressed);
    connect(btn, &VirtualKeyboardButton::specialKeyPressed, this, &VirtualKeyboardInnerWidget::onSpecialKeyPressed);
    mKeyboardLayout->addWidget(btn, y, x, spany, spanx);
    mButtons.append(btn);
}

void UnivKbd::VirtualKeyboardInnerWidget::onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key) {

    switch (key.getType()) {

        case KeyType::CONFIG:
            mMainLayout->setCurrentWidget(mConfigurationWidget);
            return;

        case KeyType::OPENCLOSE:
            triggerSetEnabled();
            return;

        case KeyType::SHIFT:
        case KeyType::ALT:
        case KeyType::CTRL:
            pressModifier(key);
            refreshModifiers(&button);
            break;

        default:
            break;
    }

    emit virtualKeyPressed(button, key); // admitting there is a direct connection

    // if the key is a character, add it to the current word
    if (key.getCharacters().size() > 0 && key.getCharacters()[0] > 'a' && key.getCharacters()[0] < 'z') {
        mCurrentWord += key.getCharacters()[0];
    } else {
        mCurrentWord = "";
    }

    QStringList suggestions = key.getSpecials(0);

    if (mCurrentWord != "") {
        for (const auto& word : mDictionary) {
            if (word.startsWith(mCurrentWord)) {
                suggestions << word;
            }
        }
    }

    setSuggestions(suggestions);


    switch (key.getType()) {

        case KeyType::SHIFT:
        case KeyType::ALT:
        case KeyType::CTRL:
            break;

        default:
            if (key.getCharacters().size() == 0) {
            } else {
                mKeyModifier = 0;
                refreshModifiers(&button);
            }
            break;
    }

    update();
}

void UnivKbd::VirtualKeyboardInnerWidget::onSpecialKeyPressed(VirtualKeyboardButton &button, const Key &key, const QString &special) {
    emit specialKeyPressed(button, key, special);
    mKeyModifier = 0;
    refreshModifiers(&button);
}

void UnivKbd::VirtualKeyboardInnerWidget::setEnabled(bool enabled) {
    mIsEnabled = enabled;
    if (enabled) {
        mMainLayout->setCurrentWidget(mKeyboardWidget);
    } else {
        mMainLayout->setCurrentWidget(mOpenButton);
    }
}

void UnivKbd::VirtualKeyboardInnerWidget::triggerSetEnabled() {
    setEnabled(!mIsEnabled);
}

void UnivKbd::VirtualKeyboardInnerWidget::refreshModifiers(QObject *toIgnore) {
    for (auto button : mButtons) {
        if (button->getKey().getType() == KeyType::REGULAR) {
            button->setCurrentKey(currentKeyType());
        } else if (button != toIgnore) {
            button->setChecked(isModifierPressed(button->getKey()));
        }
    }
}

void UnivKbd::VirtualKeyboardInnerWidget::paintEvent(QPaintEvent *event) {
    (void)event;

    // Compute minimal rect with and height
    int minWidth = std::numeric_limits<int>::max();
    int minHeight = std::numeric_limits<int>::max();

    for (const auto& button : mButtons) {
        minWidth = std::min(minWidth, button->rect().width());
        minHeight = std::min(minHeight, button->rect().height());
    }


    // Calculate the font size based on the button's dimensions and text width
    std::shared_ptr<QFont> font = std::make_shared<QFont>();
    qreal maxFontSize = qMin(0.6 * minWidth, 0.6 * minHeight); // Maximum font size based on button width and height
    qreal fontPointSize = 0.8 * maxFontSize; // Initial font size based on the maximum font size
    font->setPointSizeF(fontPointSize);

    QFontMetrics fontMetrics(*font);
    qreal textWidth = fontMetrics.horizontalAdvance("F10"); // Width of the button's text

    // Adjust the font size to fit the text within the button's width
    if (textWidth > 0.6 * minWidth)
    {
        fontPointSize *= (0.6 * minWidth) / textWidth;
    }

    // Adjust the font size to fit the text within the button's height
    if (fontPointSize > maxFontSize)
    {
        fontPointSize = maxFontSize;
    }

    font->setPointSizeF(fontPointSize);

    for (auto button : mButtons) {
        button->setFont(font);
    }

    // draw each button from here
    //QPainter painter(this);
    //painter.setFont(*font);
    //for (const auto& button : mButtons) {
    //    button->paintFromParent(painter);
    //}
    
}