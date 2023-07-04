#include "VirtualKeyboard.h"
#include <QFile>
#include <QKeyEvent>
#include <QLabel>

#include <unordered_set>

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

    setCurrentKey(0);
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

    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);

    QList<QString> keyboardLayouts = Keyboard::getOperatingSystemKeyboards();
    if (keyboardLayouts.size() == 0) {
        throw std::runtime_error("No keyboard layouts found");
    }
    // add a combobox to select the keyboard layout
    mKeyboardSelector = new QComboBox(this);
    mMainLayout->addWidget(mKeyboardSelector);
    for (auto keyboardLayout : keyboardLayouts) {
        mKeyboardSelector->addItem(keyboardLayout);
    }
    // set to English (United States) as default
    mKeyboardSelector->setCurrentText("US");

    connect(mKeyboardSelector, &QComboBox::currentTextChanged, [=](const QString &text) {
        Keyboard keyboard = Keyboard::getKeyboardFromOperatingSystem(text);
        if (!loadLayoutFromKeyboard(keyboard)) {
            throw std::runtime_error("Failed to load the keyboard layout file.");
        }
    });

    mKeyboardLayout = new QGridLayout(this);
    mMainLayout->addLayout(mKeyboardLayout);

    qDebug() << "Parent is " << parent;

    connect(qApp, &QApplication::focusChanged, [=](QWidget *old, QWidget *now) {
        qDebug() << "Focus changed from" << old << "to" << now;
        if (now == this || old == this || now == mKeyboardSelector) {
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

    Keyboard keyboard = Keyboard::getDefaultKeyboardFromOperatingSystem();
    if (!loadLayoutFromKeyboard(keyboard)) {
        throw std::runtime_error("Failed to load the keyboard layout file.");
    }
}

bool UniQKey::VirtualKeyboard::loadLayoutFromKeyboard(const Keyboard& keyboard) {

    // empty the layout
    QLayoutItem *child;
    while ((child = mKeyboardLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

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
    connect(btn, &VirtualKeyboardButton::virtualKeyPressed, this, &VirtualKeyboard::onVirtualKeyPressed);
    mKeyboardLayout->addWidget(btn, y, x, spany, spanx);
    mButtons.append(btn);
}

void UniQKey::VirtualKeyboard::onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key) {

    switch (key.getType()) {
    
    case KeyType::REGULAR:
        onVirtualRegularKeyPressed(button, key.getCharacters()[button.getCurrentKey()]); // todo : this is not 0
        break;
        
    case KeyType::SHIFT:
    case KeyType::ALT:
    case KeyType::CTRL:
        mKeyModifier ^= (unsigned char)key.getType();
        for (auto button : mButtons) {
            button->setCurrentKey(mKeyModifier);
        }
    
    }

}

void UniQKey::VirtualKeyboard::onVirtualRegularKeyPressed(VirtualKeyboardButton &button, const QChar &key) {

    if (mKeyModifier & (unsigned char)KeyType::CTRL == 0) {
        Qt::KeyboardModifiers modifiers = Qt::KeyboardModifier::ControlModifier;
        if (mKeyModifier & (unsigned char)KeyType::SHIFT == 0) {
            modifiers |= Qt::KeyboardModifier::ShiftModifier;
        }
        if (mKeyModifier & (unsigned char)KeyType::ALT == 0) {
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