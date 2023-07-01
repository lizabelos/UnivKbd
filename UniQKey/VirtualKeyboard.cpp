#include "VirtualKeyboard.h"
#include <QFile>
#include <QKeyEvent>
#include <QLabel>

#include <unordered_set>

namespace UniQKey {
    bool gShiftPressed = false;
    bool gAltPressed = false;
    std::unordered_set<VirtualKeyboardButton*> gVirtualKeyboardButtons;

    void pressShift() {
        gShiftPressed = !gShiftPressed;
        int index = gShiftPressed * 1 + gAltPressed * 2;
        for (auto button : gVirtualKeyboardButtons) {
            button->setCurrentKey(index);
        }
    }

    void pressAlt() {
        gAltPressed = !gAltPressed;
        int index = gShiftPressed * 1 + gAltPressed * 2;
        for (auto button : gVirtualKeyboardButtons) {
            button->setCurrentKey(index);
        }
    }

}

UniQKey::VirtualKeyboardButton::VirtualKeyboardButton(const Key &key) : mKey(key) {

    switch (key.getType()) {

        case KeyType::REGULAR:
            mKeyString[0] = key.getCharacters()[0];
            mKeyString[1] = key.getCharacters()[1];
            mKeyString[2] = key.getCharacters()[2];
            break;

        case KeyType::SHIFT:
            mKeyString[0] = "Shift";
            break;

        case KeyType::CTRL:
            mKeyString[0] = "Ctrl";
            break;

        case KeyType::ALT:
            mKeyString[0] = "Alt";
            break;

        case KeyType::FUNCTION:
            mKeyString[0] = key.getCharacters();
            break;

        case KeyType::TAB:
            mKeyString[0] = "Tab";
            break;

        case KeyType::BACKSPACE:
            mKeyString[0] = "Backspace";
            break;

        case KeyType::CAPS_LOCK:
            mKeyString[0] = "Caps Lock";
            break;

        case KeyType::SPACE:
            mKeyString[0] = "Space";
            break;

        case KeyType::ENTER:
            mKeyString[0] = "Enter";
            break;

        default:
            break;

    }

    mGridLayout = new QGridLayout(this);
    setMinimumSize(60, 70);
    setLayout(mGridLayout);

    mCurrentFont.setPointSize(14);
    mCurrentFont.setBold(true);
    mCurrentColor = QColor(0, 0, 0);

    mDefaultFont.setPointSize(10);
    mDefaultColor = QColor(100, 100, 100);

    if (mKeyString[1] == "" && mKeyString[2] == "") {
        mLabels[0] = new QLabel(mKeyString[0], this);
        mLabels[0]->setAlignment(Qt::AlignCenter);
        mGridLayout->addWidget(mLabels[0], 0, 0, 1, 2);
    }

    if (mKeyString[1] != "" && mKeyString[2] == "") {
        mLabels[0] = new QLabel(mKeyString[0], this);
        mLabels[1] = new QLabel(mKeyString[1], this);

        mLabels[0]->setAlignment(Qt::AlignCenter);
        mLabels[1]->setAlignment(Qt::AlignCenter);

        mGridLayout->addWidget(mLabels[0], 0, 0, 1, 2);
        mGridLayout->addWidget(mLabels[1], 2, 1);
    }

    if (mKeyString[1] != "" && mKeyString[2] != "") {
        mLabels[0] = new QLabel(mKeyString[0], this);
        mLabels[1] = new QLabel(mKeyString[1], this);
        mLabels[2] = new QLabel(mKeyString[2], this);

        mLabels[0]->setAlignment(Qt::AlignCenter);
        mLabels[1]->setAlignment(Qt::AlignCenter);
        mLabels[2]->setAlignment(Qt::AlignCenter);

        mGridLayout->addWidget(mLabels[0], 0, 0, 1, 2);
        mGridLayout->addWidget(mLabels[1], 1, 0);
        mGridLayout->addWidget(mLabels[2], 1, 1);
    }


    connect(this, &QPushButton::pressed, this, &VirtualKeyboardButton::virtualButtonPressed);

    gVirtualKeyboardButtons.insert(this);

    setCurrentKey(0);
}

UniQKey::VirtualKeyboardButton::~VirtualKeyboardButton() {
    gVirtualKeyboardButtons.erase(this);
}

void UniQKey::VirtualKeyboardButton::virtualButtonPressed() {
    
    switch(mKey.getType()) {

        case KeyType::REGULAR:
            emit virtualKeyPressed(mKey.getCharacters()[mCurrentKey]);
            break;
        
        case KeyType::SHIFT:
            UniQKey::pressShift();
            break;

        case KeyType::CTRL:
            // todo
            break;

        case KeyType::ALT:
            UniQKey::pressAlt();
            break;

        default:
            break;

    }

}

void UniQKey::VirtualKeyboardButton::setCurrentKey(int index) {
    int maxIndex = 0;
    for (; maxIndex < 2; maxIndex++) {
        if (mLabels[maxIndex + 1] == nullptr) {
            break;
        }
    }

    mCurrentKey = std::min(maxIndex, index);

    for (int i = 0; i < 3; i++) {
        if (mLabels[i] != nullptr) {
            if (i == mCurrentKey) {
                mLabels[i]->setFont(mCurrentFont);
                mLabels[i]->setStyleSheet("color: " + mCurrentColor.name());
            } else {
                mLabels[i]->setFont(mDefaultFont);
                mLabels[i]->setStyleSheet("color: " + mDefaultColor.name());
            }
        }
    }
}

UniQKey::VirtualKeyboard::VirtualKeyboard(QWidget *parent) : mParent(parent) {
    // This widget is always on top of the other widgets.
    // But it never gets focus.
    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    mGridLayout = new QGridLayout(this);

    qDebug() << "Parent is " << parent;

    connect(qApp, &QApplication::focusChanged, [=](QWidget *old, QWidget *now) {
        qDebug() << "Focus changed from" << old << "to" << now;
        if (now == this || old == this) {
            return;
        }
        for (auto button : mButtons) {
            if (now == button || old == button) {
                return;
            }
        }
        if (now == mParent) {
            qDebug() << "Virtual keyboard got focus";
            parentTakeFocus();
        } else if (old == mParent) {
            qDebug() << "Virtual keyboard lost focus";
            parentLooseFocus();
        }
    });

    Keyboard keyboard;
    if (loadLayoutFromKeyboard(keyboard)) {
        setLayout(mGridLayout);
    } else {
        throw std::runtime_error("Failed to load the keyboard layout file.");
    }
}

bool UniQKey::VirtualKeyboard::loadLayoutFromKeyboard(const Keyboard& keyboard) {

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
    connect(btn, &VirtualKeyboardButton::virtualKeyPressed, this, &VirtualKeyboard::virtualKeyPressed);
    mGridLayout->addWidget(btn, y, x, spany, spanx);
    mButtons.append(btn);
}

void UniQKey::VirtualKeyboard::virtualKeyPressed(QChar key) {

    if (mIsCtrlPressed) {
        Qt::KeyboardModifiers modifiers = Qt::KeyboardModifier::ControlModifier;
        if (mIsShiftPressed) {
            modifiers |= Qt::KeyboardModifier::ShiftModifier;
        }
        if (mIsAltGrPressed) {
            modifiers |= Qt::KeyboardModifier::AltModifier;
        }
        // emit a Qt shortcut
        QKeySequence shortcut(key);
        QShortcutEvent *shortcutEvent = new QShortcutEvent(shortcut, modifiers);
        QApplication::sendEvent(mParent, shortcutEvent);

        QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, key.unicode(), modifiers, QString(key));
        QApplication::sendEvent(mParent, keyEvent);
    } else {
        QInputMethodEvent *inputMethodEvent = new QInputMethodEvent();
        inputMethodEvent->setCommitString(key);
        QApplication::sendEvent(mParent, inputMethodEvent);
    }
}

void UniQKey::VirtualKeyboard::parentTakeFocus() {
    show();
}

void UniQKey::VirtualKeyboard::parentLooseFocus() {
    hide();
}