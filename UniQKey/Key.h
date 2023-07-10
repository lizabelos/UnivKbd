/*
* --------------------------------------------------------------
* Project: UniQKey
* Author: Liza Belos
* Year: 2023
* 
* Copyright (c) 2023. All rights reserved.
* This work is licensed under the terms of the MIT License.
* For a copy, see <https://opensource.org/licenses/MIT>.
* --------------------------------------------------------------
*
* NOTICE:
* This file is part of the original distribution of the UniQKey project. 
* All changes and redistributions of this file must retain this notice, 
* the list of contributors, and the entire copyright notice including the
* MIT License information.
* 
* DISCLAIMER:
* This software is provided 'as-is', without any express or implied warranty. 
* In no event will the authors be held liable for any damages arising from 
* the use of this software.
*/

#ifndef UNIQKEY_KEY_H
#define UNIQKEY_KEY_H

#include <QString>

namespace UniQKey {

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
        FN1 = 8,
        FN2 = 9,
        FN3 = 10,
        FN4 = 11,
        FN5 = 12,
        FN6 = 13,
        FN7 = 14,
        FN8 = 15,
        FN9 = 16,
        FN10 = 17,
        FN11 = 18,
        FN12 = 19    
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
                file.write(mCharacters.toStdString().c_str(), mCharactersSize);
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
                char* mCharactersBuffer = new char[mCharactersSize];
                file.read(mCharactersBuffer, mCharactersSize);
                key.mCharacters = QString(mCharactersBuffer);
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

        /**
         * @brief Convert the key to a Qt key.
         * 
         * @return The Qt key.
         **/
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