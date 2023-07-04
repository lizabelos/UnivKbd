#ifndef UNIQKEY_KEYBOARD_H
#define UNIQKEY_KEYBOARD_H

#include <QString>
#include <QFile>
#include <QList>

#include "Key.h"

namespace UniQKey {

    class Keyboard {
    public:
        inline const std::vector<Key>& getKeys() const {
            return mKeys;
        }

        inline void serialize(QFile &file) {
            int size = mKeys.size();
            file.write(reinterpret_cast<char*>(&size), sizeof(int));
            for (int i = 0; i < size; ++i) {
                mKeys[i].serialize(file);
            }
        }

        inline void deserialize(QFile &file) {
            int size;
            file.read(reinterpret_cast<char*>(&size), sizeof(int));
            mKeys.resize(size);
            for (int i = 0; i < size; ++i) {
                mKeys[i].deserialize(file);
            }
        }

        static QList<QString> getOperatingSystemKeyboards();

        static Keyboard getKeyboardFromOperatingSystem(const QString &layout);

        static Keyboard getDefaultKeyboardFromOperatingSystem();

    protected:
        Keyboard() = default;

    private:
        std::vector<Key> mKeys;
    };



}  // namespace UniQKey

#endif  // UNIQKEY_KEYBOARD_H
