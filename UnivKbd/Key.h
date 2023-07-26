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

#ifndef UNIVKBD_KEY_H
#define UNIVKBD_KEY_H

#include <QString>

namespace UnivKbd {

    /**
     * @brief The KeyType enum
     * 
     * Define the type of a key.
     */
    enum class KeyType { 
        REGULAR = -1, 
        SHIFT = 0, 
        ALT = 1,
        CTRL = 2, 
        ESC = 3,
        TAB = 4,
        BACKSPACE = 5,
        SPACE = 6,
        ENTER = 7,
        CAPS_LOCK = 8,
        UP = 9,
        DOWN = 10,
        LEFT = 11,
        RIGHT = 12,
        FN1 = 13,
        FN2 = 14,
        FN3 = 15,
        FN4 = 16,
        FN5 = 17,
        FN6 = 18,
        FN7 = 19,
        FN8 = 20,
        FN9 = 21,
        FN10 = 22,
        FN11 = 23,
        FN12 = 24,
        CONFIG = 25,
        OPENCLOSE = 26
    };


    /**
     * @brief The Key class
     * 
     * Define a key of a keyboard, with its type, its associated characters, and its position.
     */
    class Key {
    public:
        /***
         * @brief The default constructor of a key.
         * 
         * Warning : The default constructor of a key does not initialize any field. It is here only for serialization purposes,
         * or for pre-allocating a vector of keys.
        */
        Key() = default;

        /**
         * @brief The constructor of a "not regular" key.
         * 
         * @param type The type of the key.
         * @param xSpan The width of the key.
         * @param ySpan The height of the key.
         * 
         * @see KeyType
        */
        Key(KeyType type, float xSpan = 1, float ySpan = 1) : mType(type), mCharacters(""), mXSpan(xSpan), mYSpan(ySpan) {

        }

        /**
         * @brief The constructor of a regular key.
         * 
         * @param characters The characters associated to the key.
         * @param xSpan The width of the key.
         * @param ySpan The height of the key.
         * 
         * @see KeyType
        */
        Key(const QString& characters, float xSpan = 1, float ySpan = 1) : mType(KeyType::REGULAR), mCharacters(characters), mXSpan(xSpan), mYSpan(ySpan) {
            mSpecials.resize(characters.size());
        }

        /**
         * @brief Get the type of the key.
         * 
         * @return The type of the key.
         * @see KeyType
         **/
        inline KeyType getType() const {
            return mType;
        }

        /**
         * @brief Get the characters associated to the key.
         * 
         * @return The characters associated to the key. If the key is not regular, it returns an empty string.
         **/
        inline const QString& getCharacters() const {
            return mCharacters;
        }

        /**
         * @brief Set the characters associated to the key.
         *
         * @param characters The characters associated to the key.
         * @return The key with the new characters.
         **/
        inline Key withCharacters(const QString& characters) const {
            Key key = *this;
            key.mCharacters = characters;
            key.mSpecials.resize(characters.size());
            return key;
        }

        /**
         * @brief Get the special characters associated to the key.
         *
         * @return The special characters associated to the key. If the key is not regular, it returns an empty string.
         **/
        inline const QStringList& getSpecials(int i) const {
            return mSpecials[i];
        }

        /**
         * @brief Set the special characters associated to the key.
         *
         * @param specials The special characters associated to the key.
         * @return The key with the new special characters.
         **/
        inline void setSpecials(int i, const QStringList& specials) {
            mSpecials[i] = specials;
        }

        /**
         * @brief Get the width of the key.
         * 
         * @return The width of the key in the keyboard.
         **/
        inline float getXSpan() const {
            return mXSpan;
        }


        /**
         * @brief Get the height of the key.
         * 
         * @return The height of the key in the keyboard.
         **/
        inline float getYSpan() const {
            return mYSpan;
        }


        /**
         * @brief Get the x position of the key.
         * 
         * @return The x position of the key in the keyboard.
         **/
        inline float getX() const {
            return mX;
        }

        /**
         * @brief Get the y position of the key.
         * 
         * @return The y position of the key in the keyboard.
         **/
        inline float getY() const {
            return mY;
        }

        inline void setX(float x) {
            mX = x;
        }

        inline void setY(float y) {
            mY = y;
        }

        /**
         * @brief Serialize the key in a file.
         * 
         * @param file The file in which the key will be serialized.
         **/
        inline void serialize(QFile &file) const {
            file.write((const char*)(&mType), sizeof(KeyType));
            file.write((const char*)(&mXSpan), sizeof(float));
            file.write((const char*)(&mYSpan), sizeof(float));
            file.write((const char*)(&mX), sizeof(float));
            file.write((const char*)(&mY), sizeof(float));

            int mCharactersSize = mCharacters.size();
            file.write((const char*)(&mCharactersSize), sizeof(int));
            if (mCharactersSize > 0) {
                file.write((char*)mCharacters.utf16(), mCharactersSize * sizeof(uchar));
            }

        }

        /**
         * @brief Deserialize a key from a file.
         * 
         * @param file The file from which the key will be deserialized.
         * @return The deserialized key.
         **/
        static inline Key deserialize(QFile &file) {
            Key key;

            file.read((char*)(&key.mType), sizeof(KeyType));
            file.read((char*)(&key.mXSpan), sizeof(float));
            file.read((char*)(&key.mYSpan), sizeof(float));
            file.read((char*)(&key.mX), sizeof(float));
            file.read((char*)(&key.mY), sizeof(float));
            
            int mCharactersSize;
            file.read((char*)(&mCharactersSize), sizeof(int));
            if (mCharactersSize > 0) {
                ushort *mCharactersBuffer = new ushort[mCharactersSize];
                file.read((char*)mCharactersBuffer, mCharactersSize * sizeof(uchar));
                key.mCharacters = QString::fromUtf16(mCharactersBuffer, mCharactersSize);
                delete[] mCharactersBuffer;
            }
            return key;
        }

        /**
         * @brief Convert the key to a string that can be used for UI.
         * 
         * @param i The index of the character to display. If the key is not regular, this parameter is ignored.
         * @return The string that can be used for UI.
         **/        
        inline QString toString(int i = 0) const {
            switch (getType()) {

                case KeyType::REGULAR:
                    return getCharacters()[std::min((int)i, (int)getCharacters().size() - 1)];

                case KeyType::SHIFT:
                    return "Shift";

                case KeyType::CTRL:
                    return "Ctrl";

                case KeyType::ALT:
                    return "Alt";

                case KeyType::ESC:
                    return "Esc";

                case KeyType::TAB:
                    return "Tab";

                case KeyType::BACKSPACE:
                    return "Backspace";

                case KeyType::SPACE:
                    return "Space";

                case KeyType::ENTER:
                    return "Enter";

                case KeyType::CAPS_LOCK:
                    return "Caps Lock";

                case KeyType::UP:
                    return "↑";

                case KeyType::DOWN:
                    return "↓";

                case KeyType::LEFT:
                    return "←";

                case KeyType::RIGHT:
                    return "→";

                case KeyType::FN1:
                    return "F1";

                case KeyType::FN2:
                    return "F2";

                case KeyType::FN3:
                    return "F3";

                case KeyType::FN4:
                    return "F4";

                case KeyType::FN5:
                    return "F5";

                case KeyType::FN6:
                    return "F6";

                case KeyType::FN7:
                    return "F7";

                case KeyType::FN8:
                    return "F8";

                case KeyType::FN9:
                    return "F9";

                case KeyType::FN10:
                    return "F10";

                case KeyType::FN11:
                    return "F11";

                case KeyType::FN12:
                    return "F12";

                case KeyType::CONFIG:
                    return "Config";

                case KeyType::OPENCLOSE:
                    return "Open/Close";

                default:
                    return "";

            }
        }

        /**
         * @brief Convert the key to a Qt key.
         * 
         * @return The Qt key.
         **/
        Qt::Key toQtKey() const {
            switch (getType()) {
                    
                    case KeyType::REGULAR:
                        return static_cast<Qt::Key>(getCharacters()[0].unicode());

                    case KeyType::SHIFT:
                        return Qt::Key_Shift;

                    case KeyType::CTRL:
                        return Qt::Key_Control;

                    case KeyType::ALT:
                        return Qt::Key_Alt;

                    case KeyType::ESC:
                        return Qt::Key_Escape;

                    case KeyType::TAB:
                        return Qt::Key_Tab;

                    case KeyType::BACKSPACE:
                        return Qt::Key_Backspace;

                    case KeyType::SPACE:
                        return Qt::Key_Space;

                    case KeyType::ENTER:
                        return Qt::Key_Enter;

                    case KeyType::CAPS_LOCK:
                        return Qt::Key_CapsLock;

                    case KeyType::UP:
                        return Qt::Key_Up;

                    case KeyType::DOWN:
                        return Qt::Key_Down;

                    case KeyType::LEFT:
                        return Qt::Key_Left;

                    case KeyType::RIGHT:
                        return Qt::Key_Right;

                    case KeyType::FN1:
                        return Qt::Key_F1;

                    case KeyType::FN2:
                        return Qt::Key_F2;

                    case KeyType::FN3:
                        return Qt::Key_F3;

                    case KeyType::FN4:
                        return Qt::Key_F4;

                    case KeyType::FN5:
                        return Qt::Key_F5;

                    case KeyType::FN6:
                        return Qt::Key_F6;

                    case KeyType::FN7:
                        return Qt::Key_F7;

                    case KeyType::FN8:
                        return Qt::Key_F8;

                    case KeyType::FN9:
                        return Qt::Key_F9;

                    case KeyType::FN10:
                        return Qt::Key_F10;

                    case KeyType::FN11:
                        return Qt::Key_F11;

                    case KeyType::FN12:
                        return Qt::Key_F12;

                    default:
                        return Qt::Key_unknown;
                }
        }

    private:
        KeyType mType;
        QString mCharacters;
        QVector<QStringList> mSpecials;
        float mXSpan, mYSpan, mX, mY;
    };

}

#endif