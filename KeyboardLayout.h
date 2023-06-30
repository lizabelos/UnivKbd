//
// Created by lizab on 28/06/2023.
//

#ifndef LIZAQTKEYBOARD_KEYBOARDLAYOUT_H
#define LIZAQTKEYBOARD_KEYBOARDLAYOUT_H

#include <QApplication>

namespace UniQKey {

    class KeyboardLayout {

        class KeyboardLayoutItem {

            int id;
            qreal x;
            qreal y;
            qreal w;
            qreal h;

        };

    public:
        KeyboardLayout();

        ~KeyboardLayout();

    private:


    };

    class KeyboardVariant {

        class KeyboardVariantItem {

            int id;
            int a, b, c;

        };

    };

    class Keyboard {


    };

}

#endif //LIZAQTKEYBOARD_KEYBOARDLAYOUT_H
