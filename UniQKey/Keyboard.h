#ifndef UNIQKEY_KEYBOARD_H
#define UNIQKEY_KEYBOARD_H

#include <QString>
#include <vector>

namespace UniQKey {

enum class KeyType { 
    REGULAR, 
    SHIFT, 
    CTRL, 
    ALT, 
    FUNCTION,
    TAB,
    BACKSPACE,
    CAPS_LOCK,
    SPACE,
    ENTER    
};

class Key {
public:
    Key() = default;

    Key(KeyType type, float xSpan = 1, float ySpan = 1);

    Key(const QString& characters, float xSpan = 1, float ySpan = 1);

    inline KeyType getType() const {
        return mType;
    }

    inline const QString& getCharacters() const {
        return mCharacters;
    }

    inline float getXSpan() const {
        return mXSpan;
    }

    inline float getYSpan() const {
        return mYSpan;
    }

    inline float getX() const {
        return mX;
    }

    inline float getY() const {
        return mY;
    }

    inline void setX(float x) {
        mX = x;
    }

    inline void setY(float y) {
        mY = y;
    }

private:
    KeyType mType;
    QString mCharacters;
    float mXSpan, mYSpan, mX, mY;
};

class Keyboard {
public:
    Keyboard();

    inline const std::vector<Key>& getKeys() const {
        return mKeys;
    }

private:
    std::vector<Key> mKeys;
};

}  // namespace UniQKey

#endif  // UNIQKEY_KEYBOARD_H
