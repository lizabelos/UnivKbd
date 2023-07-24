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

#ifndef VIRTUALKEYBOARDBUTTON_H
#define VIRTUALKEYBOARDBUTTON_H

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

#include <unordered_set>

#include "Keyboard.h"

namespace UnivKbd {

    class VirtualKeyboardButton : public QAbstractButton {
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

        qreal recommendedTextSize() const;

        inline void setTextSize(float size) {
            mTextSize = size;
        }


    protected:
        void paintEvent(QPaintEvent *event) override;

    signals:
        void virtualKeyPressed(VirtualKeyboardButton &button, const Key &key);

    private slots:
        void virtualButtonPressed();

    private:
        Key mKey;
        QString mKeyString[3];

        int mCurrentKey;

        QPixmap mPixmap;

        float mTextSize = 0;

    };

}

#endif