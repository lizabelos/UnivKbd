#include "VirtualKeyboard.h"
#include <QFile>
#include <QKeyEvent>
#include <QLabel>

UniQKey::VirtualKeyboardButton::VirtualKeyboardButton(QChar mainkey, QChar shiftkey, QChar altgrkey) {

    mMainkey = mainkey;
    mShiftkey = shiftkey;
    mAltgrkey = altgrkey;
    mIsModifier = false;
    mCurrentKey = mMainkey;

    QFont bigFont;
    bigFont.setPointSize(14);

    QFont smallFont;
    smallFont.setPointSize(10);

    gridLayout = new QGridLayout(this);
    mainkeyLabel = new QLabel(mainkey, this);
    shiftkeyLabel = new QLabel(shiftkey, this);
    altgrkeyLabel = new QLabel(altgrkey, this);

    mainkeyLabel->setFont(bigFont);
    shiftkeyLabel->setFont(smallFont);
    altgrkeyLabel->setFont(smallFont);

    // center the text in the QLabel
    mainkeyLabel->setAlignment(Qt::AlignCenter);
    shiftkeyLabel->setAlignment(Qt::AlignCenter);
    altgrkeyLabel->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(mainkeyLabel, 0, 0, 2, 2);
    gridLayout->addWidget(shiftkeyLabel, 2, 1);
    gridLayout->addWidget(altgrkeyLabel, 2, 2);

    setMinimumSize(60, 70);

    setLayout(gridLayout);

    connect(this, &QPushButton::pressed, this, &VirtualKeyboardButton::virtualButtonPressed);
}

UniQKey::VirtualKeyboardButton::VirtualKeyboardButton(Qt::KeyboardModifier modifier) {

    mIsModifier = true;
    mModifier = modifier;

    setMinimumSize(60, 70);

    QFont bigFont;
    bigFont.setPointSize(14);
    setFont(bigFont);

    setText(QKeySequence(modifier).toString());

    setCheckable(true);

    connect(this, &QPushButton::pressed, this, &VirtualKeyboardButton::virtualButtonPressed);

}

void UniQKey::VirtualKeyboardButton::updateModifiers(bool shift, bool altgr, bool isctrl) {
    if (mIsModifier) {
        if (mModifier == Qt::KeyboardModifier::ShiftModifier) {
            setChecked(shift);
        }
        if (mModifier == Qt::KeyboardModifier::AltModifier) {
            setChecked(altgr);
        }
        if (mModifier == Qt::KeyboardModifier::ControlModifier) {
            setChecked(isctrl);
        }
        return;
    }

    if ((!shift && !altgr) || isctrl) {
        mainkeyLabel->setText(mMainkey);
        shiftkeyLabel->setText(mShiftkey);
        altgrkeyLabel->setText(mAltgrkey);
        mCurrentKey = mMainkey;
    } else if (shift && !altgr) {
        mainkeyLabel->setText(mShiftkey);
        shiftkeyLabel->setText(mMainkey);
        altgrkeyLabel->setText(mAltgrkey);
        mCurrentKey = mShiftkey;
    } else if (!shift && altgr) {
        mainkeyLabel->setText(mAltgrkey);
        shiftkeyLabel->setText(mShiftkey);
        altgrkeyLabel->setText(mMainkey);
        mCurrentKey = mAltgrkey;
    } else if (shift && altgr) {
        mainkeyLabel->setText(mAltgrkey);
        shiftkeyLabel->setText(mMainkey);
        altgrkeyLabel->setText(mShiftkey);
        mCurrentKey = mAltgrkey;
    }

}

void UniQKey::VirtualKeyboardButton::virtualButtonPressed() {
    if (mIsModifier) {
        emit virtualModifierKeyPressed(mModifier);
        setChecked(!isChecked());
    } else {
        emit virtualKeyPressed(mCurrentKey);
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

    if (loadLayoutFromFile("../azerty.txt")) {
        setLayout(mGridLayout);
    } else {
        throw std::runtime_error("Failed to load the keyboard layout file.");
    }
}

bool UniQKey::VirtualKeyboard::loadLayoutFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);

    int minX = 9999, minY = 9999, maxX = -9999, maxY = -9999;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList keys = line.split(" ");

        if (keys.size() != 5) {
            continue;
        }

        int x = keys.at(0).toInt();
        int y = keys.at(1).toInt();

        minX = std::min(minX, x);
        minY = std::min(minY, y);
        maxX = std::max(maxX, x);
        maxY = std::max(maxY, y);

        addButton(x, y, keys.at(2).at(0), keys.at(3).at(0), keys.at(4).at(0));
    }

    addButton(maxX + 1, minY - 1, Qt::KeyboardModifier::ControlModifier, 1, 2);
    addButton(maxX + 1, minY + 1, Qt::KeyboardModifier::AltModifier, 1, 2);
    addButton(maxX + 1, minY + 3, ' ', ' ', ' ', 1, 4);
    addButton(maxX + 1, minY + 7, Qt::KeyboardModifier::ShiftModifier, 1, 2);

    file.close();
    return true;
}

void UniQKey::VirtualKeyboard::addButton(int x, int y, QChar mainkey, QChar shiftkey, QChar altgrkey, int spanx, int spany) {
    VirtualKeyboardButton *btn = new VirtualKeyboardButton(mainkey, shiftkey, altgrkey);
    connect(btn, &VirtualKeyboardButton::virtualKeyPressed, this, &VirtualKeyboard::virtualKeyPressed);
    mGridLayout->addWidget(btn, x, y, spanx, spany);
    mButtons.append(btn);
}

void UniQKey::VirtualKeyboard::addButton(int x, int y, Qt::KeyboardModifier modifier, int spanx, int spany) {
    VirtualKeyboardButton *btn = new VirtualKeyboardButton(modifier);
    connect(btn, &VirtualKeyboardButton::virtualModifierKeyPressed, this, &VirtualKeyboard::virtualModifierKeyPressed);
    mGridLayout->addWidget(btn, x, y, spanx, spany);
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

void UniQKey::VirtualKeyboard::virtualModifierKeyPressed(Qt::KeyboardModifier modifier) {
    switch (modifier) {
        case Qt::KeyboardModifier::ShiftModifier:
            mIsShiftPressed = !mIsShiftPressed;
            break;
        case Qt::KeyboardModifier::ControlModifier:
            mIsCtrlPressed = !mIsCtrlPressed;
            break;
        case Qt::KeyboardModifier::AltModifier:
            mIsAltGrPressed = !mIsAltGrPressed;
            break;
        default:
            break;
    }

    for (auto button : mButtons) {
        button->updateModifiers(mIsShiftPressed, mIsAltGrPressed, mIsCtrlPressed);
    }
}

void UniQKey::VirtualKeyboard::parentTakeFocus() {
    show();
}

void UniQKey::VirtualKeyboard::parentLooseFocus() {
    hide();
}