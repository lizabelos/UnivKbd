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

#include "Keyboard.h"


QList<QString> UniQKey::Keyboard::getOperatingSystemKeyboards() {
    return QList<QString>();
}

UniQKey::Keyboard UniQKey::Keyboard::getKeyboardFromOperatingSystem(const QString &country, const QString &layout) {
    throw std::runtime_error("OS Not supported");
}

UniQKey::Keyboard UniQKey::Keyboard::getDefaultKeyboardFromOperatingSystem() {
    throw std::runtime_error("OS Not supported");
}

