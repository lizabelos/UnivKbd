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

#include "Keyboard.h"

#include <iostream>
#include <qDebug>
#include <Windows.h>

namespace UnivKbd {

QString keyboardCountryToHexadecimal(QString layout) {
    HKEY hKey;
    WCHAR data[255];
    DWORD dataSize;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return "";
    }

    for (DWORD i = 0; ; i++) {
        WCHAR valueName[255];
        DWORD valueNameSize = sizeof(valueName) / sizeof(WCHAR);

        if (RegEnumKeyExW(hKey, i, valueName, &valueNameSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
            break;
        }

        HKEY hSubKey;
        if (RegOpenKeyExW(hKey, valueName, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) {
            continue;
        }

        dataSize = sizeof(data);
        if (RegQueryValueExW(hSubKey, L"Layout Text", NULL, NULL, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
            if (QString::fromWCharArray(data) == layout) {
                RegCloseKey(hSubKey);
                RegCloseKey(hKey);
                return QString::fromWCharArray(valueName);
            }
        }

        RegCloseKey(hSubKey);
    }

    RegCloseKey(hKey);

    return "";
}    

UINT keyTypeToVirtualKeyCode(KeyType keyType) {
    switch (keyType) {
        case KeyType::SHIFT:
            return VK_SHIFT;
        case KeyType::ALT:
            return VK_MENU;
        case KeyType::CTRL:
            return VK_CONTROL;
        case KeyType::FN1:
            return VK_F1;
        case KeyType::FN2:
            return VK_F2;
        case KeyType::FN3:
            return VK_F3;
        case KeyType::FN4:
            return VK_F4;
        case KeyType::FN5:
            return VK_F5;
        case KeyType::FN6:
            return VK_F6;
        case KeyType::FN7:
            return VK_F7;
        case KeyType::FN8:
            return VK_F8;
        case KeyType::FN9:
            return VK_F9;
        case KeyType::FN10:
            return VK_F10;
        case KeyType::FN11:
            return VK_F11;
        case KeyType::FN12:
            return VK_F12;
        case KeyType::TAB:
            return VK_TAB;
        case KeyType::BACKSPACE:
            return VK_BACK;
        case KeyType::SPACE:
            return VK_SPACE;
        case KeyType::ENTER:
            return VK_RETURN;
        default:
            return 0;
    }
}

bool convertKeyboardToCountry(Key &key, QChar &output, HKL inputLayout, HKL outputLayout, bool shift, bool altGr) {

    UINT virtualKeyCode = -1;
    int i = 0;
    while (virtualKeyCode == -1 && i < key.getCharacters().size()) {
        virtualKeyCode = VkKeyScanEx(key.getCharacters()[i].unicode(), inputLayout);
        i++;
    }

    if (virtualKeyCode == -1) {
        qDebug() << "Could not translate key " << key.getCharacters();
        return false;
    }

    /*UINT scanCode = MapVirtualKeyEx(virtualKeyCode, MAPVK_VK_TO_VSC, inputLayout);
    if (scanCode == 0) {
        qDebug() << "Could not retrieve scan code for key " << key;
        return false;
    }*/

    BYTE bState[256] = {0};
    if (shift) {
        bState[VK_SHIFT] = 0x80;
    }
    if (altGr) {
        bState[VK_MENU] = 0x80;
    }

    WCHAR buffer[5] = { 0 };
    int result = ToUnicodeEx(virtualKeyCode, 0, bState, buffer, 5, 0, outputLayout);

    if (result == 0) {
        qDebug() << "Could not translate virtual key " << virtualKeyCode;
        return false;
    }

    if (result < 0) {
        qDebug() << "Could not translate virtual key " << virtualKeyCode << " (dead key)";
        return false;
    }
        
    qDebug() << "Translated virtual key " << key.getCharacters() << "(" << virtualKeyCode << ") to " << QString::fromWCharArray(buffer, result);
    output = QString::fromWCharArray(buffer, result).at(0);
    
    return true;
}

bool convertKeyboardToCountry(Key &key, HKL inputLayout, HKL outputLayout) {
    if (key.getType() != KeyType::REGULAR) {
        return true;
    }

    QChar character;
    QString newCharacters;

    qDebug() << "Converting key " << character << " from " << inputLayout << " to " << outputLayout;

    if (convertKeyboardToCountry(key, character, inputLayout, outputLayout, false, false)) {
        newCharacters += character;
    }

    if (convertKeyboardToCountry(key, character, inputLayout, outputLayout, true, false)) {
        newCharacters += character;
    }

    if (convertKeyboardToCountry(key, character, inputLayout, outputLayout, false, true)) {
        newCharacters += character;
    }

    if (convertKeyboardToCountry(key, character, inputLayout, outputLayout, true, true)) {
        newCharacters += character;
    }

    qDebug() << "Converted key " << character << " to " << newCharacters;

    if (newCharacters.isEmpty()) {
        return false;
    }
    key = Key(newCharacters, key.getXSpan(), key.getYSpan());
    return true;
}

std::vector<std::vector<Key>> convertKeyboardToCountry(std::vector<std::vector<Key>> &keys, HKL inputLayout, HKL outputLayout) {
    std::vector<std::vector<Key>> newLayout;
    for (auto& row : keys) {
        std::vector<Key> newRow;
        for (auto& key : row) {
            if (convertKeyboardToCountry(key, inputLayout, outputLayout)) {
                newRow.push_back(key);
            }
        }
        newLayout.push_back(newRow);
    }
    return newLayout;
}

std::vector<std::vector<Key>> convertKeyboardToLayout(std::vector<std::vector<Key>> &keys, const QString &inputLayout, const QString &outputLayout) {
    
    qDebug() << "Converting keyboard from " << inputLayout << " to " << outputLayout;

    // create a QChar map from inputLayout to outputLayout
    std::map<QChar, QChar> charMap;
    for (int i = 0; i < inputLayout.size(); i++) {
        charMap[inputLayout.at(i)] = outputLayout.at(i);
    }

    // add the uppercase characters
    for (int i = 0; i < inputLayout.size(); i++) {
        charMap[inputLayout.at(i).toUpper()] = outputLayout.at(i).toUpper();
    }

    // map the keys
    std::vector<std::vector<Key>> newKeys;
    for (auto& row : keys) {
        std::vector<Key> newRow;
        for (auto& key : row) {
            if (key.getType() == KeyType::REGULAR) {
                QString newCharacters;
                for (auto& character : key.getCharacters()) {
                    if (charMap.find(character) == charMap.end()) {
                        newCharacters += character;
                    } else {
                        newCharacters += charMap[character];
                    }
                }
                qDebug() << "Converted key " << key.getCharacters() << " to " << newCharacters;
                newRow.push_back(Key(newCharacters, key.getXSpan(), key.getYSpan()));
            } else {
                newRow.push_back(key);
            }
        }
        newKeys.push_back(newRow);
    }


    return newKeys;

}

std::vector<std::vector<Key>> generateLayout(HKL outputLayout, const QString &layout) {

    qDebug() << "Loading default french keys";
    std::vector<std::vector<Key>> frenchKeys = {
            {{KeyType::ESC, 1, 1}, {KeyType::FN1, 1, 1}, {KeyType::FN2, 1, 1}, {KeyType::FN3, 1, 1}, {KeyType::FN4, 1, 1}, {KeyType::FN5, 1, 1}, {KeyType::FN6, 1, 1}, {KeyType::FN7, 1, 1}, {KeyType::FN8, 1, 1}, {KeyType::FN9, 1, 1}, {KeyType::FN10, 1, 1}, {KeyType::FN11, 1, 1}, {KeyType::FN12, 1, 1}, {KeyType::CONFIG, 1, 1}, {KeyType::OPENCLOSE, 1, 1}},
            {{"²", 1, 1}, {"1", 1, 1}, {"2", 1, 1}, {"3", 1, 1}, {"4", 1, 1}, {"5", 1, 1}, {"6", 1, 1}, {"7", 1, 1}, {"8", 1, 1}, {"9", 1, 1}, {")°]", 1, 1}, {"=+}", 1, 1}, {KeyType::BACKSPACE, 3.25f, 1}},
            {{KeyType::TAB, 1.25f, 1}, {"a", 1, 1}, {"z", 1, 1}, {"e", 1, 1}, {"r", 1, 1}, {"t", 1, 1}, {"y", 1, 1}, {"u", 1, 1}, {"i", 1, 1}, {"o", 1, 1}, {"p", 1, 1}, {"^¨", 1, 1}, {"$£¤", 1, 1}, {KeyType::ENTER, 2, 2}},
            {{KeyType::CAPS_LOCK, 1.75f, 1}, {"q", 1, 1}, {"s", 1, 1}, {"d", 1, 1}, {"f", 1, 1}, {"g", 1, 1}, {"h", 1, 1}, {"j", 1, 1}, {"k", 1, 1}, {"l", 1, 1}, {"m", 1, 1}, {"ù%", 1, 1}, {"*µ", 1, 1}},
            {{KeyType::SHIFT, 2, 1}, {"<>", 1, 1}, {"w", 1, 1}, {"x", 1, 1}, {"c", 1, 1}, {"v", 1, 1}, {"b", 1, 1}, {"n", 1, 1}, {",?", 1, 1}, {";.", 1, 1}, {":/", 1, 1}, {"!§", 1, 1}, {KeyType::UP, 3.25f, 1}},
            {{KeyType::CTRL, 1.75f, 1}, {KeyType::ALT, 1.75f, 1}, {KeyType::SPACE, 5.25f, 1}, {KeyType::ALT, 1.75f, 1}, {KeyType::CTRL, 1.75f, 1}, {KeyType::LEFT, 1, 1}, {KeyType::DOWN, 1, 1}, {KeyType::RIGHT, 1, 1}}
    };


    qDebug() << "Loading french layout";
    QString frenchLayoutName = keyboardCountryToHexadecimal("French");
    HKL frenchLayout = LoadKeyboardLayout((LPCWSTR)frenchLayoutName.utf16(), 0);

    qDebug() << "Start converting french keyboard to output country layout";
    std::vector<std::vector<Key>> result = convertKeyboardToCountry(frenchKeys, frenchLayout, outputLayout);

    qDebug() << "Start converting output country layout to output layout";
    result = convertKeyboardToLayout(result, "azertyuiopqsdfghjklmwxcvbn", layout);

    qDebug() << "Unloading french layout";
    UnloadKeyboardLayout(frenchLayout);

    return result;
}

}

QList<QString> UnivKbd::Keyboard::getOperatingSystemKeyboards() {
    QList<QString> layouts;
    HKEY hKey;
    WCHAR data[255];
    DWORD dataSize;

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts", 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return layouts;
    }

    for (DWORD i = 0; ; i++) {
        WCHAR valueName[255];
        DWORD valueNameSize = sizeof(valueName) / sizeof(WCHAR);

        if (RegEnumKeyExW(hKey, i, valueName, &valueNameSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
            break;
        }

        HKEY hSubKey;
        if (RegOpenKeyExW(hKey, valueName, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) {
            continue;
        }

        dataSize = sizeof(data);
        if (RegQueryValueExW(hSubKey, L"Layout Text", NULL, NULL, (LPBYTE)data, &dataSize) == ERROR_SUCCESS) {
            layouts.push_back(QString::fromWCharArray(data));
        }

        RegCloseKey(hSubKey);
    }

    RegCloseKey(hKey);

    // sort the layouts
    std::sort(layouts.begin(), layouts.end());

    return layouts;
}

UnivKbd::Keyboard UnivKbd::Keyboard::getKeyboardFromOperatingSystem(const QString &country, const QString &layout) {
   
    qDebug() << "Converting country " << country << " to hexadecimal layout";
    QString hexadecimalLayout = keyboardCountryToHexadecimal(country);
    if (hexadecimalLayout == "") {
        throw std::runtime_error("Could not find layout");
    }

    qDebug() << "Loading layout " << hexadecimalLayout;
    HKL layoutHandle = LoadKeyboardLayout((LPCWSTR)hexadecimalLayout.utf16(), 0);

    if (layoutHandle == NULL) {
        throw std::runtime_error("Could not load layout");
    }

    qDebug() << "Generating layout";
    std::vector<std::vector<Key>> rows = generateLayout(layoutHandle, layout);

    qDebug() << "Unloading layout";
    UnloadKeyboardLayout(layoutHandle);
    

    qDebug() << "Creating keyboard";
    Keyboard keyboard;

    float x = 0;
    float y = 0;

    for (auto& row : rows) {       

        x = 0;

        for (auto& key : row) {

            key.setX(x);
            key.setY(y);

            x = x + key.getXSpan();

            keyboard.mKeys.push_back(key);

        }

        y = y + 1;

    }

    return keyboard;

}

UnivKbd::Keyboard UnivKbd::Keyboard::getDefaultKeyboardFromOperatingSystem() {
    return getKeyboardFromOperatingSystem("US", getKeyboardLayouts()[0]);
}
