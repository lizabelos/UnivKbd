#ifndef UNIQKEY_KEY_H
#define UNIQKEY_KEY_H

#include <QString>

namespace UniQKey {

    enum class KeyType { 
        REGULAR = 0, 
        SHIFT = 1, 
        ALT = 2,
        CTRL = 4, 
        FUNCTION = 8,
        TAB = 16,
        BACKSPACE = 32,
        CAPS_LOCK = 64,
        SPACE = 128,
        ENTER = 256    
    };

    class Key {
    public:
        Key() = default;

        Key(KeyType type, float xSpan = 1, float ySpan = 1) : mType(type), mCharacters(""), mXSpan(xSpan), mYSpan(ySpan) {

        }

        Key(const QString& characters, float xSpan = 1, float ySpan = 1) : mType(KeyType::REGULAR), mCharacters(characters), mXSpan(xSpan), mYSpan(ySpan) {

        }

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

        inline void serialize(QFile &file) {
            file.write(reinterpret_cast<char*>(&mType), sizeof(KeyType));
            file.write(reinterpret_cast<char*>(&mXSpan), sizeof(float));
            file.write(reinterpret_cast<char*>(&mYSpan), sizeof(float));
            file.write(reinterpret_cast<char*>(&mX), sizeof(float));
            file.write(reinterpret_cast<char*>(&mY), sizeof(float));
            file.write(reinterpret_cast<char*>(&mCharacters), sizeof(QString));
        }

        inline void deserialize(QFile &file) {
            file.read(reinterpret_cast<char*>(&mType), sizeof(KeyType));
            file.read(reinterpret_cast<char*>(&mXSpan), sizeof(float));
            file.read(reinterpret_cast<char*>(&mYSpan), sizeof(float));
            file.read(reinterpret_cast<char*>(&mX), sizeof(float));
            file.read(reinterpret_cast<char*>(&mY), sizeof(float));
            file.read(reinterpret_cast<char*>(&mCharacters), sizeof(QString));
        }

    private:
        KeyType mType;
        QString mCharacters;
        float mXSpan, mYSpan, mX, mY;
    };

}

#endif