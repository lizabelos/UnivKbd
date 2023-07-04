#include "Keyboard.h"

#include <iostream>
#include <qDebug>
#include <Windows.h>

namespace UniQKey {

QString keyboardNameToHexadecimal(QString layout) {
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
        case KeyType::FUNCTION:
            return VK_F1;  // Placeholder for lack of a direct mapping
        case KeyType::TAB:
            return VK_TAB;
        case KeyType::BACKSPACE:
            return VK_BACK;
        case KeyType::CAPS_LOCK:
            return VK_CAPITAL;
        case KeyType::SPACE:
            return VK_SPACE;
        case KeyType::ENTER:
            return VK_RETURN;
        default:
            return 0;
    }
}

bool convertLayout(Key &key, QChar &output, HKL inputLayout, HKL outputLayout, bool shift, bool altGr) {

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

bool convertLayout(Key &key, HKL inputLayout, HKL outputLayout) {
    if (key.getType() != KeyType::REGULAR) {
        return true;
    }

    QChar character;
    QString newCharacters;

    qDebug() << "Converting key " << character << " from " << inputLayout << " to " << outputLayout;

    if (convertLayout(key, character, inputLayout, outputLayout, false, false)) {
        newCharacters += character;
    }

    if (convertLayout(key, character, inputLayout, outputLayout, true, false)) {
        newCharacters += character;
    }

    if (convertLayout(key, character, inputLayout, outputLayout, false, true)) {
        newCharacters += character;
    }

    qDebug() << "Converted key " << character << " to " << newCharacters;

    if (newCharacters.isEmpty()) {
        return false;
    }
    key = Key(newCharacters, key.getXSpan(), key.getYSpan());
    return true;
}

std::vector<std::vector<Key>> convertLayout(std::vector<std::vector<Key>> &keys, HKL inputLayout, HKL outputLayout) {
    std::vector<std::vector<Key>> newLayout;
    for (auto& row : keys) {
        std::vector<Key> newRow;
        for (auto& key : row) {
            if (convertLayout(key, inputLayout, outputLayout)) {
                newRow.push_back(key);
            }
        }
        newLayout.push_back(newRow);
    }
    return newLayout;
}

std::vector<std::vector<Key>> generateLayout(HKL outputLayout) {

    std::vector<std::vector<Key>> frenchKeys = {
        {{"²", 1, 1}, {"&1", 1, 1}, {"é2~", 1, 1}, {"\"3#", 1, 1}, {"'4{", 1, 1}, {"(5[", 1, 1}, {"-6|", 1, 1}, {"è7`", 1, 1}, {"_8\\", 1, 1}, {"ç9^", 1, 1}, {"à0@", 1, 1}, {")°]", 1, 1}, {"=+}", 1, 1}, {KeyType::BACKSPACE, 3, 1}},
        {{KeyType::TAB, 1.25f, 1}, {"aA", 1, 1}, {"zZ", 1, 1}, {"eE", 1, 1}, {"rR", 1, 1}, {"tT", 1, 1}, {"yY", 1, 1}, {"uU", 1, 1}, {"iI", 1, 1}, {"oO", 1, 1}, {"pP", 1, 1}, {"^¨", 1, 1}, {"$£", 1, 1}, {"*", 1, 1}, {KeyType::ENTER, 1.75f, 1}},
        {{KeyType::CAPS_LOCK, 1.75f, 1}, {"qQ", 1, 1}, {"sS", 1, 1}, {"dD", 1, 1}, {"fF", 1, 1}, {"gG", 1, 1}, {"hH", 1, 1}, {"jJ", 1, 1}, {"kK", 1, 1}, {"lL", 1, 1}, {"mM", 1, 1}, {"ù%", 1, 1}, {"µ", 1, 1}, {KeyType::ENTER, 2.25f, 1}},
        {{KeyType::SHIFT, 2, 1}, {"wW", 1, 1}, {"xX", 1, 1}, {"cC", 1, 1}, {"vV", 1, 1}, {"bB", 1, 1}, {"nN", 1, 1}, {",?", 1, 1}, {".:", 1, 1}, {";/", 1, 1}, {":!", 1, 1}, {"§", 1, 1}, {KeyType::SHIFT, 1, 1}},
        {{KeyType::CTRL, 1.25f, 1}, {KeyType::ALT, 1.25f, 1}, {KeyType::SPACE, 5.25f, 1}, {KeyType::ALT, 1.25f, 1}, {KeyType::CTRL, 1.25f, 1}}
    };

    QString frenchLayoutName = keyboardNameToHexadecimal("French");
    HKL frenchLayout = LoadKeyboardLayout(frenchLayoutName.toStdWString().c_str(), 0);

    std::vector<std::vector<Key>> result = convertLayout(frenchKeys, frenchLayout, outputLayout);

    UnloadKeyboardLayout(frenchLayout);

    return result;
}

}

QList<QString> UniQKey::Keyboard::getOperatingSystemKeyboards() {
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

UniQKey::Keyboard UniQKey::Keyboard::getKeyboardFromOperatingSystem(const QString &layout) {
   
    QString hexadecimalLayout = keyboardNameToHexadecimal(layout);
    if (hexadecimalLayout == "") {
        throw std::runtime_error("Could not find layout");
    }

    HKL layoutHandle = LoadKeyboardLayout((LPCWSTR)hexadecimalLayout.utf16(), 0);

    if (layoutHandle == NULL) {
        throw std::runtime_error("Could not load layout");
    }

    std::vector<std::vector<Key>> rows = generateLayout(layoutHandle);

    UnloadKeyboardLayout(layoutHandle);
    

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

UniQKey::Keyboard UniQKey::Keyboard::getDefaultKeyboardFromOperatingSystem() {
    return getKeyboardFromOperatingSystem("US");
}
