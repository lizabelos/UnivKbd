#include "VirtualKeyboard.h"

#include <QFile>
#include <QKeyEvent>
#include <QLabel>
#include <QTimer>

#include <unordered_set>

UniQKey::VirtualKeyboardButton::VirtualKeyboardButton(const Key &key) : mKey(key) {

    switch (key.getType()) {

        case KeyType::REGULAR:
            mKeyString[0] = key.toString(0);
            mKeyString[1] = key.toString(1);
            mKeyString[2] = key.toString(2);
            break;

        default:
            mKeyString[0] = key.toString();
            setCheckable(true);
            break;

    }

    mGridLayout = new QGridLayout();
    setMinimumSize(60, 70);
    setLayout(mGridLayout);

    mCurrentFont.setPointSize(14);
    mCurrentFont.setBold(true);
    mCurrentColor = QColor(0, 0, 0);

    mDefaultFont.setPointSize(10);
    mDefaultColor = QColor(100, 100, 100);

    if (mKeyString[1] == "" && mKeyString[2] == "") {
        mLabels[0] = new QLabel(mKeyString[0]);
        mLabels[0]->setAlignment(Qt::AlignCenter);
        mGridLayout->addWidget(mLabels[0], 0, 0, 1, 2);
    }

    if (mKeyString[1] != "" && mKeyString[2] == "") {
        mLabels[0] = new QLabel(mKeyString[0]);
        mLabels[1] = new QLabel(mKeyString[1]);

        mLabels[0]->setAlignment(Qt::AlignCenter);
        mLabels[1]->setAlignment(Qt::AlignCenter);

        mGridLayout->addWidget(mLabels[0], 0, 0, 1, 2);
        mGridLayout->addWidget(mLabels[1], 2, 1);
    }

    if (mKeyString[1] != "" && mKeyString[2] != "") {
        mLabels[0] = new QLabel(mKeyString[0]);
        mLabels[1] = new QLabel(mKeyString[1]);
        mLabels[2] = new QLabel(mKeyString[2]);

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

    mMainLayout = new QVBoxLayout();
    setLayout(mMainLayout);

    QList<QString> keyboardLayouts = Keyboard::listExportedKeyboards();
    if (keyboardLayouts.size() == 0) {
        throw std::runtime_error("No keyboard layouts found");
    }
    // add a combobox to select the keyboard layout
    mCountrySelector = new QComboBox();
    mMainLayout->addWidget(mCountrySelector);
    for (auto keyboardLayout : keyboardLayouts) {
        mCountrySelector->addItem(keyboardLayout);
    }
    // set to English (United States) as default
    mCountrySelector->setCurrentText("US");


    mLayoutSelector = new QComboBox();
    mMainLayout->addWidget(mLayoutSelector);
    for (auto layout : getKeyboardLayouts()) {
        mLayoutSelector->addItem(layout);
    }
    mLayoutSelector->setCurrentText(getKeyboardLayouts()[0]);

    connect(mCountrySelector, &QComboBox::currentTextChanged, [=](const QString &text) {
        Keyboard keyboard = Keyboard::importKeyboard(mCountrySelector->currentText(), mLayoutSelector->currentText());
        if (!loadLayoutFromKeyboard(keyboard)) {
            throw std::runtime_error("Failed to load the keyboard layout file.");
        }
    });

    connect(mLayoutSelector, &QComboBox::currentTextChanged, [=](const QString &text) {
        Keyboard keyboard = Keyboard::importKeyboard(mCountrySelector->currentText(), mLayoutSelector->currentText());
        if (!loadLayoutFromKeyboard(keyboard)) {
            throw std::runtime_error("Failed to load the keyboard layout file.");
        }
    });
    

    mKeyboardLayout = new QGridLayout();
    mMainLayout->addLayout(mKeyboardLayout);

    mOpenCloseButton = new QPushButton("Open/Close", this);
    mMainLayout->addWidget(mOpenCloseButton);
    
    // connect to slot triggerSetEnabled
    connect(mOpenCloseButton, &QPushButton::clicked, this, &VirtualKeyboard::triggerSetEnabled);


    qDebug() << "Parent is " << parent;

    connect(qApp, &QApplication::focusChanged, [=](QWidget *old, QWidget *now) {
        qDebug() << "Focus changed from" << old << "to" << now;
        if (now == this || old == this || now == mCountrySelector || now == mLayoutSelector) {
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

    Keyboard keyboard = Keyboard::importKeyboard(mCountrySelector->currentText(), mLayoutSelector->currentText());
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

    qDebug() << "Adding button : " << x << " " << y << " " << spanx << " " << spany;

    VirtualKeyboardButton *btn = new VirtualKeyboardButton(key);
    connect(btn, &VirtualKeyboardButton::virtualKeyPressed, this, &VirtualKeyboard::onVirtualKeyPressed);
    mKeyboardLayout->addWidget(btn, y, x, spany, spanx);
    mButtons.append(btn);
}

void UniQKey::VirtualKeyboard::onVirtualKeyPressed(VirtualKeyboardButton &button, const Key &key) {

    QKeyEvent *event;
    //QKeySequence mKeySequence;

    switch (key.getType()) {
        
    case KeyType::SHIFT:
    case KeyType::ALT:
    case KeyType::CTRL:
        pressModifier(key);
        qDebug() << currentKeyType();
        for (auto otherButton : mButtons) {
            if (otherButton == &button) {
                continue;
            }
            if (otherButton->getKey().getType() == KeyType::REGULAR) {
                otherButton->setCurrentKey(currentKeyType());
            } else {
                otherButton->setChecked(isModifierPressed(otherButton->getKey()));
            }
        }
        event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), "");
        //mKeySequence = QKeySequence(QKeyCombination(getModifiers()));

    default:
        event = new QKeyEvent(QEvent::KeyPress, (int)key.toQtKey(), getModifiers(), key.getCharacters()[button.getCurrentKey()]);
        //mKeySequence = QKeySequence(QKeyCombination(getModifiers(), key.toQtKey()));
        break;

    }

    //QShortcutEvent *shortcutEvent = new QShortcutEvent(mKeySequence, 0, false);
    //QCoreApplication::postEvent(mParent, shortcutEvent);
    QCoreApplication::postEvent(mParent, event);

}


void UniQKey::VirtualKeyboard::parentTakeFocus() {
    show();
}

void UniQKey::VirtualKeyboard::parentLooseFocus() {
    hide();
}

void UniQKey::VirtualKeyboard::setEnabled(bool enabled) {
    mIsEnabled = enabled;
    if (enabled) {
        mOpenCloseButton->setText("Close");

        mMainLayout->removeWidget(mOpenCloseButton); // to keep the button at the bottom

        // add the selectors and the keyboard layout
        mMainLayout->addWidget(mCountrySelector);
        mMainLayout->addWidget(mLayoutSelector);
        mMainLayout->addLayout(mKeyboardLayout);
        mMainLayout->addWidget(mOpenCloseButton);

    } else {
        mOpenCloseButton->setText("Open");


        // hide the selectors and the keyboard layout. for that we are going to revmoe them from the layout
        mMainLayout->removeWidget(mCountrySelector);
        mMainLayout->removeWidget(mLayoutSelector);
        mMainLayout->removeItem(mKeyboardLayout);


    }
}

void UniQKey::VirtualKeyboard::triggerSetEnabled() {
    setEnabled(!mIsEnabled);
}