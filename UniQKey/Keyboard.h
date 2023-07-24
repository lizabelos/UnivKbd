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

#ifndef UNIQKEY_KEYBOARD_H
#define UNIQKEY_KEYBOARD_H

#include <QString>
#include <QFile>
#include <QList>
#include <QDir>
#include <QDebug>

#include "Key.h"

/**
 * @brief Internal function to initializes the UniQKey Qt resources.
 */
inline void UNIQKEY_INIT_RESOURCE() {
    Q_INIT_RESOURCE(keyboards);
    Q_INIT_RESOURCE(icons);
}

namespace UniQKey {

    /**
     * @brief Returns a list of available keyboard layouts.
     *
     * @return QStringList A list of keyboard layouts.
     */
    inline QStringList getKeyboardLayouts() {
        QStringList layouts;
        layouts << "qwertyuiopasdfghjklzxcvbnm";
        layouts << "azertyuiopqsdfghjklmwxcvbn";
        return layouts;
    }

    /**
     * @class Keyboard
     *
     * @brief Represents a keyboard.
     */
    class Keyboard {
    public:

        /**
         * @brief Returns the keys of the keyboard.
         *
         * @return The keys of the keyboard.
         */
        inline const std::vector<Key>& getKeys() const {
            return mKeys;
        }

        /**
         * @brief Serializes the keyboard into a file.
         *
         * @param file The file to serialize the keyboard into.
         * 
         * @note You can serialize multiple keyboards into the same file.
         */
        inline void serialize(QFile &file) {
            int size = mKeys.size();
            file.write((char*)(&size), sizeof(int));
            for (int i = 0; i < size; ++i) {
                mKeys[i].serialize(file);
            }
        }

        /**
         * @brief Deserializes the keyboard from a file.
         *
         * @param file The file to deserialize the keyboard from.
         * 
         * @note You can deserialize mutliple keyboards from a same file.
         */
        inline void deserialize(QFile &file) {
            int size;
            file.read((char*)(&size), sizeof(int));
            mKeys.resize(size);
            for (int i = 0; i < size; ++i) {
                mKeys[i] = Key::deserialize(file);
            }
        }

        /**
         * @brief Returns a list of keyboards grabbed from the operating system.
         *
         * @return A list of operating system keyboards.
         */
        static QList<QString> getOperatingSystemKeyboards();

        /**
         * @brief Returns a keyboard based on the country and layout from the operating system.
         *
         * @param country The country of the keyboard.
         * @param layout The layout of the keyboard.
         * @return The keyboard corresponding to the country and layout.
         */
        static Keyboard getKeyboardFromOperatingSystem(const QString &country, const QString &layout);

        /**
         * @brief Returns the default keyboard from the operating system.
         *
         * @return The default keyboard from the operating system.
         */
        static Keyboard getDefaultKeyboardFromOperatingSystem();

        /**
         * @brief Exports operating system keyboards to files.
         */
        static inline void exportOperatingSystemKeyboards() {
            // create directory keyboards
            QDir dir("keyboards");
            if (!dir.exists()) {
                dir.mkpath(".");
            }

            // export keyboards
            QList<QString> keyboards = getOperatingSystemKeyboards();
            for (int i = 0; i < keyboards.size(); ++i) {
                qDebug() << "Loading keyboard" << keyboards[i];
                Keyboard keyboard = getKeyboardFromOperatingSystem(keyboards[i], getKeyboardLayouts()[0]);
                qDebug() << "Exporting keyboard" << keyboards[i];
                QFile file("keyboards/" + keyboards[i] + ".keyboard");
                file.open(QIODevice::WriteOnly);
                keyboard.serialize(file);
                file.close();
            }
        }

        /**
         * @brief Lists the exported keyboards.
         *
         * @param path The path to look at for keyboards
         * @return A list of exported keyboards.
         */
        static inline QList<QString> listExportedKeyboards(QString path=":/") {
            // initialize keyboards resources
            UNIQKEY_INIT_RESOURCE();
            QDir dir(path);
            if (!dir.exists()) {
                return QList<QString>();
            }
            // list every file that ends with .keyboard. Remove the .keyboard extension.
            QStringList keyboards = dir.entryList(QStringList() << "*.keyboard", QDir::Files);
            for (int i = 0; i < keyboards.size(); ++i) {
                keyboards[i].chop(9);
            }
            return keyboards;
        }

        /**
         * @brief Imports a keyboard from a file.
         *
         * @param name The name of the keyboard.
         * @param layout The layout of the keyboard.
         * @return Keyboard The imported keyboard.
         */
        static inline Keyboard importKeyboard(const QString &name, const QString &layout) {
            QFile file(":/" + name + ".keyboard");
            file.open(QIODevice::ReadOnly);
            if (!file.isOpen()) {
                qDebug() << "Could not open keyboard" << name;
                return Keyboard();
            }
            Keyboard keyboard;
            keyboard.deserialize(file);
            file.close();
            qDebug() << "Converting imported keyboard" << name << "from" << getKeyboardLayouts()[0] << "to" << layout;
            return convertLayout(keyboard, getKeyboardLayouts()[0], layout);
        }

        static QString convertLayout(const QString &text, const std::map<QChar, QChar> &charMap) {
            QString convertedText = "";
            for (int i = 0; i < text.size(); i++) {
                if (charMap.find(text.at(i)) != charMap.end()) {
                    convertedText += charMap.at(text.at(i));
                } else {
                    convertedText += text.at(i);
                }
            }
            return convertedText;
        }

        static Keyboard convertLayout(Keyboard &keyboard, const std::map<QChar, QChar> &charMap) {
            Keyboard convertedKeyboard;
            for (int i = 0; i < keyboard.getKeys().size(); i++) {
                Key key = keyboard.getKeys()[i];
                if (key.getType() == KeyType::REGULAR) {
                    key = key.withCharacters(convertLayout(key.getCharacters(), charMap));
                }
                convertedKeyboard.mKeys.push_back(key);
            }
            return convertedKeyboard;
        }

        static Keyboard convertLayout(Keyboard &keyboard, const QString &from, const QString &to) {
            // create a QChar map from inputLayout to outputLayout
            std::map<QChar, QChar> charMap;
            for (int i = 0; i < from.size(); i++) {
                charMap[from.at(i)] = to.at(i);
                charMap[QChar(from.at(i)).toUpper()] = QChar(to.at(i)).toUpper();
            }
            return convertLayout(keyboard, charMap);
        }

    protected:
        Keyboard() = default;

    private:
        std::vector<Key> mKeys;
    };



}  // namespace UniQKey

#endif  // UNIQKEY_KEYBOARD_H
