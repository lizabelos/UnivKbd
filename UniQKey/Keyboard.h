#ifndef UNIQKEY_KEYBOARD_H
#define UNIQKEY_KEYBOARD_H

#include <QString>
#include <QFile>
#include <QList>
#include <QDir>
#include <QDebug>

#include "Key.h"

inline void UniQKey_init() {
    Q_INIT_RESOURCE(keyboards);
}

namespace UniQKey {

    inline QStringList getKeyboardLayouts() {
        QStringList layouts;
        layouts << "qwertyuiopasdfghjklzxcvbnm";
        layouts << "azertyuiopqsdfghjklmwxcvbn";
        return layouts;
    }

    class Keyboard {
    public:
        inline const std::vector<Key>& getKeys() const {
            return mKeys;
        }

        inline void serialize(QFile &file) {
            int size = mKeys.size();
            file.write((char*)(&size), sizeof(int));
            for (int i = 0; i < size; ++i) {
                mKeys[i].serialize(file);
            }
        }

        inline void deserialize(QFile &file) {
            qDebug() << "Deserializing keyboard...";
            int size;
            file.read((char*)(&size), sizeof(int));
            qDebug() << "Keyboard size:" << size;
            mKeys.resize(size);
            for (int i = 0; i < size; ++i) {
                mKeys[i].deserialize(file);
            }
            qDebug() << "Deserialized keyboard.";
        }

        static QList<QString> getOperatingSystemKeyboards();

        static Keyboard getKeyboardFromOperatingSystem(const QString &country, const QString &layout);

        static Keyboard getDefaultKeyboardFromOperatingSystem();

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

        static inline QList<QString> listExportedKeyboards() {
            // initialize keyboards resources
            UniQKey_init();
            QDir dir(":/");
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

        static inline Keyboard importKeyboard(const QString &name, const QString &layout) {
            // todo : layout
            QFile file(":/" + name + ".keyboard");
            file.open(QIODevice::ReadOnly);
            if (!file.isOpen()) {
                qDebug() << "Could not open keyboard" << name;
                return Keyboard();
            }
            Keyboard keyboard;
            keyboard.deserialize(file);
            file.close();
            return keyboard;
        }

    protected:
        Keyboard() = default;

    private:
        std::vector<Key> mKeys;
    };



}  // namespace UniQKey

#endif  // UNIQKEY_KEYBOARD_H
