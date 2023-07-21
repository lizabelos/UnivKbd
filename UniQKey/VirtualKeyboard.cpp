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

QPointer<UniQKey::VirtualKeyboardInnerWidget> UniQKey::VirtualKeyboard::gInnerWidget;
QPointer<UniQKey::CustomDockWidget> UniQKey::VirtualKeyboard::gDockWidget;
QPointer<UniQKey::VirtualKeyboard> UniQKey::VirtualKeyboard::gCurrentKeyboard;
QMainWindow *UniQKey::VirtualKeyboard::gCurrentWindow = nullptr;

UniQKey::VirtualKeyboard::VirtualKeyboard(QWidget *parent, VirtualKeyboardAttachMode attachMode) : mParent(parent), mAttachMode(attachMode) {

    if (gInnerWidget.isNull()) {
        gInnerWidget = new VirtualKeyboardInnerWidget;
    }

    connect(gInnerWidget, &VirtualKeyboardInnerWidget::virtualKeyPressed, this, &VirtualKeyboard::onVirtualKeyPressed, Qt::DirectConnection);
    connect(qApp, &QApplication::focusChanged, this, &VirtualKeyboard::onAppFocusChanged);

    // forward the signal gCurrentKeyboard suggestionPressed to a lambda that emit suggestionPressed of this class
    connect(gCurrentKeyboard, &VirtualKeyboard::suggestionPressed, [=](const QString &suggestion) {
        emit suggestionPressed(suggestion);
    });


}

void UniQKey::VirtualKeyboard::onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key) {

    if (gCurrentKeyboard != this) {
        return;
    }

    QKeyEvent *event;

    switch (key.getType()) {
        
    case KeyType::SHIFT:
    case KeyType::ALT:
    case KeyType::CTRL:
        event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), "");
        break;

    default:
        if (key.getCharacters().size() == 0) {
            event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), "");
        } else {
            event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), key.getCharacters()[button.getCurrentKey()]);
        }
        break;

    }

    QCoreApplication::postEvent(mParent, event);
}


void UniQKey::VirtualKeyboard::parentTakeFocus() {
    qDebug() << "VirtualKeyboard::parentTakeFocus()";
    if (gCurrentKeyboard != nullptr && gCurrentKeyboard != this) {
        gCurrentKeyboard->parentLooseFocus();
    }
    gCurrentKeyboard = this;
    switch (mAttachMode) {
        case VirtualKeyboardAttachMode::Auto:
            if (gDockWidget.isNull()) {
                show();
            } else {
                gDockWidget->show();
            }
            break;
        case VirtualKeyboardAttachMode::Floating:
            show();
            break;
        case VirtualKeyboardAttachMode::Docked:
            findWindowAndAttachDockWidget();
            gDockWidget->show();
            break;
    }
}

void UniQKey::VirtualKeyboard::parentLooseFocus() {
    gCurrentKeyboard = nullptr;
    qDebug() << "VirtualKeyboard::parentLooseFocus()";
    switch (mAttachMode) {
        case VirtualKeyboardAttachMode::Auto:
            if (gDockWidget.isNull()) {
                hide();
            } else {
                gDockWidget->hide();
            }
        case VirtualKeyboardAttachMode::Floating:
            hide();
            break;
        case VirtualKeyboardAttachMode::Docked:
            gDockWidget->hide();
            break;
    }
}

void UniQKey::VirtualKeyboard::setEnabled(bool enabled) {
    gInnerWidget->setEnabled(enabled);
}

void UniQKey::VirtualKeyboard::triggerSetEnabled() {
    gInnerWidget->triggerSetEnabled();
}

void UniQKey::VirtualKeyboard::findWindowAndAttachDockWidget() {

    if (gDockWidget.isNull()) {
        gDockWidget = new CustomDockWidget();
        gDockWidget->setCustomWidget(gInnerWidget);
    }

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

    if (gCurrentWindow == window) {
        return;
    }

    if (gCurrentWindow != nullptr) {
        gCurrentWindow->removeDockWidget(gDockWidget);
    }

    window->addDockWidget(Qt::BottomDockWidgetArea, gDockWidget);
    gCurrentWindow = window;

}

void UniQKey::VirtualKeyboard::onAppFocusChanged(QObject *old, QObject *now) {

    if (now == nullptr) {
        return;
    }

    bool oldIsThis = old == mParent;
    bool nowIsThis = now == mParent;

    if (gCurrentKeyboard == this) {
        oldIsThis = oldIsThis || old == gInnerWidget;
        nowIsThis = nowIsThis || now == gInnerWidget;

        std::unordered_set<QObject*> children;
        children.insert(gInnerWidget);

        if (gDockWidget != nullptr) {
            children.insert(gDockWidget);
        }

        while (children.size() > 0) {
            QObject *child = *children.begin();
            children.erase(children.begin());
            for (auto subchild : child->children()) {
                children.insert(subchild);
            }
            oldIsThis = oldIsThis || old == child;
            nowIsThis = nowIsThis || now == child;
        }
    }

    if (gCurrentKeyboard != this && nowIsThis) {
        parentTakeFocus();
    }
    if (gCurrentKeyboard == this && !nowIsThis) {
        parentLooseFocus();
    }

}

void UniQKey::VirtualKeyboard::attachToCurrentWindowAsDockWidget() {
    if (mAttachMode == VirtualKeyboardAttachMode::Docked) {
        return;
    }
    if (gCurrentKeyboard != nullptr) {
        gCurrentKeyboard->parentLooseFocus();
    }
    mAttachMode = VirtualKeyboardAttachMode::Docked;
    if (gCurrentKeyboard != nullptr) {
        gCurrentKeyboard->parentTakeFocus();
    }
}


