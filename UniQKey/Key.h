#ifndef UNIQKEY_KEY_H
#define UNIQKEY_KEY_H

#include <QString>

namespace UniQKey {

    enum class KeyType { 
        REGULAR = -1, 
        SHIFT = 0, 
        ALT = 1,
        CTRL = 2, 
        ESC = 3,
        TAB = 4,
        BACKSPACE = 5,
        CAPS_LOCK = 6,
        SPACE = 7,
        ENTER = 8,
        FN1 = 9,
        FN2 = 10,
        FN3 = 11,
        FN4 = 12,
        FN5 = 13,
        FN6 = 14,
        FN7 = 15,
        FN8 = 16,
        FN9 = 17,
        FN10 = 18,
        FN11 = 19,
        FN12 = 20    
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

        inline void serialize(QFile &file) const {
            file.write(reinterpret_cast<const char*>(&mType), sizeof(KeyType));
            file.write(reinterpret_cast<const char*>(&mXSpan), sizeof(float));
            file.write(reinterpret_cast<const char*>(&mYSpan), sizeof(float));
            file.write(reinterpret_cast<const char*>(&mX), sizeof(float));
            file.write(reinterpret_cast<const char*>(&mY), sizeof(float));
            file.write(reinterpret_cast<const char*>(&mCharacters), sizeof(QString));
        }

        inline void deserialize(QFile &file) {
            file.read(reinterpret_cast<char*>(&mType), sizeof(KeyType));
            file.read(reinterpret_cast<char*>(&mXSpan), sizeof(float));
            file.read(reinterpret_cast<char*>(&mYSpan), sizeof(float));
            file.read(reinterpret_cast<char*>(&mX), sizeof(float));
            file.read(reinterpret_cast<char*>(&mY), sizeof(float));
            file.read(reinterpret_cast<char*>(&mCharacters), sizeof(QString));
        }

        inline QString toString(int i = 0) const {
            switch (getType()) {

                case KeyType::REGULAR:
                    return getCharacters()[i];
                    break;

                case KeyType::SHIFT:
                    return "Shift";
                    break;

                case KeyType::CTRL:
                    return "Ctrl";
                    break;

                case KeyType::ALT:
                    return "Alt";
                    break;

                case KeyType::ESC:
                    return "Esc";
                    break;

                case KeyType::TAB:
                    return "Tab";
                    break;

                case KeyType::BACKSPACE:
                    return "Backspace";
                    break;

                case KeyType::CAPS_LOCK:
                    return "Caps Lock";
                    break;

                case KeyType::SPACE:
                    return "Space";
                    break;

                case KeyType::ENTER:
                    return "Enter";
                    break;

                case KeyType::FN1:
                    return "Fn1";
                    break;

                case KeyType::FN2:
                    return "Fn2";
                    break;

                case KeyType::FN3:
                    return "Fn3";
                    break;

                case KeyType::FN4:
                    return "Fn4";
                    break;

                case KeyType::FN5:
                    return "Fn5";
                    break;

                case KeyType::FN6:
                    return "Fn6";
                    break;

                case KeyType::FN7:
                    return "Fn7";
                    break;

                case KeyType::FN8:
                    return "Fn8";
                    break;

                case KeyType::FN9:
                    return "Fn9";
                    break;

                case KeyType::FN10:
                    return "Fn10";
                    break;

                case KeyType::FN11:
                    return "Fn11";
                    break;

                case KeyType::FN12:
                    return "Fn12";
                    break;

                default:
                    return "";

            }
        }

        Qt::Key toQtKey() const {
            switch (getType()) {
                    
                    case KeyType::REGULAR:
                        return static_cast<Qt::Key>(getCharacters()[0].unicode());
                        break;
    
                    case KeyType::SHIFT:
                        return Qt::Key_Shift;
                        break;
    
                    case KeyType::CTRL:
                        return Qt::Key_Control;
                        break;
    
                    case KeyType::ALT:
                        return Qt::Key_Alt;
                        break;
    
                    case KeyType::ESC:
                        return Qt::Key_Escape;
                        break;
    
                    case KeyType::TAB:
                        return Qt::Key_Tab;
                        break;
    
                    case KeyType::BACKSPACE:
                        return Qt::Key_Backspace;
                        break;
    
                    case KeyType::CAPS_LOCK:
                        return Qt::Key_CapsLock;
                        break;
    
                    case KeyType::SPACE:
                        return Qt::Key_Space;
                        break;
    
                    case KeyType::ENTER:
                        return Qt::Key_Enter;
                        break;
    
                    case KeyType::FN1:
                        return Qt::Key_F1;
                        break;
    
                    case KeyType::FN2:
                        return Qt::Key_F2;
                        break;
    
                    case KeyType::FN3:
                        return Qt::Key_F3;
                        break;
    
                    case KeyType::FN4:
                        return Qt::Key_F4;
                        break;
    
                    case KeyType::FN5:
                        return Qt::Key_F5;
                        break;
    
                    case KeyType::FN6:
                        return Qt::Key_F6;
                        break;
    
                    case KeyType::FN7:
                        return Qt::Key_F7;
                        break;
    
                    case KeyType::FN8:
                        return Qt::Key_F8;
                        break;
    
                    case KeyType::FN9:
                        return Qt::Key_F9;
                        break;
    
                    case KeyType::FN10:
                        return Qt::Key_F10;
                        break;
    
                    case KeyType::FN11:
                        return Qt::Key_F11;
                        break;
    
                    case KeyType::FN12:
                        return Qt::Key_F12;
                        break;
    
                    default:
                        return Qt::Key_unknown;
                }
        }

    private:
        KeyType mType;
        QString mCharacters;
        float mXSpan, mYSpan, mX, mY;
    };

}

#endif