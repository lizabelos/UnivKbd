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

