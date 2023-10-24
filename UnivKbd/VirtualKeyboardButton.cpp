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

#include "VirtualKeyboardButton.h"

#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>
#include <QMainWindow>
#include <QDockWidget>
#include <QPainter>

#include <unordered_set>

UnivKbd::VirtualKeyboardButton::VirtualKeyboardButton(const Key &key, std::shared_ptr<QFont> font, QWidget *parent) : QAbstractButton(parent), mKey(key), mFont(font) {

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

    connect(this, &QAbstractButton::pressed, this, &VirtualKeyboardButton::virtualButtonPressed);

    setCurrentKey(0);

    // set minimum height
    setMinimumHeight(50);


    QString svgPath = ":/" + QString::fromStdString(std::to_string((int)mKey.getType())) + ".svg";
    QFile svgFile(svgPath);

    if (svgFile.open(QIODevice::ReadOnly)) {
        mPixmap = QPixmap(svgPath);
    }

}

UnivKbd::VirtualKeyboardButton::~VirtualKeyboardButton() {

}

void UnivKbd::VirtualKeyboardButton::virtualButtonPressed() {
    emit virtualKeyPressed(*this, mKey);
}

void UnivKbd::VirtualKeyboardButton::setCurrentKey(int index) {
    int maxIndex = 0;
    for (; maxIndex < 2; maxIndex++) {
        if (mKeyString[maxIndex + 1] == "") {
            break;
        }
    }

    mCurrentKey = std::min(maxIndex, index);
    setText(mKeyString[index]);
}

void UnivKbd::VirtualKeyboardButton::paintFromParent(QPainter &painter, bool fromParent) {

    // get rect from parent
    QRect rect;
    if (fromParent) {
        rect = geometry();
    } else {
        qDebug() << "Drawing keyboard button from inside";
        rect = QRect(0, 0, width(), height());
    }

    // qdebug the key and the rect
    qDebug() << "Drawing keyboard button";
    qDebug() << mKey.toString() << " " << rect;

    // Draw the background according to the button's state (hover, pressed, checked, etc.)
    if (isDown())
    {
        painter.fillRect(rect, QColor(0xCC, 0xCC, 0xCC));
    }
    else if (isChecked())
    {
        painter.fillRect(rect, QColor(0xFF, 0xCC, 0x99));
    }
    else if (underMouse())
    {
        painter.fillRect(rect, QColor(0xCC, 0xCC, 0xCC));
    }
    else
    {
        painter.fillRect(rect, QColor(0xFF, 0xFF, 0xFF));
    }

    if (mPixmap.isNull()) {
        painter.setPen(Qt::black);
        painter.drawText(rect, Qt::AlignCenter, text());
    } else {
        QRect svgSize = mPixmap.rect();
        qreal svgHeight = mFont->pointSizeF() * 0.8;
        qreal svgWidth = svgHeight * svgSize.width() / svgSize.height();

        QRect svgRect(rect.center().x() - svgWidth / 2, rect.center().y() - svgHeight / 2, svgWidth, svgHeight);
        painter.drawPixmap(svgRect, mPixmap);
    }

}