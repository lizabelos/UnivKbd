#include "Keyboard.h"


UniQKey::Key::Key(KeyType type, float xSpan, float ySpan)
    : mType(type), mCharacters(""), mXSpan(xSpan), mYSpan(ySpan) {}

UniQKey::Key::Key(const QString& characters, float xSpan, float ySpan)
    : mType(KeyType::REGULAR), mCharacters(characters), mXSpan(xSpan), mYSpan(ySpan) {}

UniQKey::Keyboard::Keyboard() {

    std::vector<std::vector<Key>> rows = {
        {{"²", 1, 1}, {"&1", 1, 1}, {"é2~", 1, 1}, {"\"3#", 1, 1}, {"'4{", 1, 1}, {"(5[", 1, 1}, {"-6|", 1, 1}, {"è7`", 1, 1}, {"_8\\", 1, 1}, {"ç9^", 1, 1}, {"à0@", 1, 1}, {")°]", 1, 1}, {"=+}", 1, 1}, {KeyType::BACKSPACE, 3, 1}},
        {{KeyType::TAB, 1.25f, 1}, {"aA", 1, 1}, {"zZ", 1, 1}, {"eE", 1, 1}, {"rR", 1, 1}, {"tT", 1, 1}, {"yY", 1, 1}, {"uU", 1, 1}, {"iI", 1, 1}, {"oO", 1, 1}, {"pP", 1, 1}, {"^¨", 1, 1}, {"$£", 1, 1}, {"*", 1, 1}, {KeyType::ENTER, 1.75f, 1}},
        {{KeyType::SHIFT, 1.75f, 1}, {"qQ", 1, 1}, {"sS", 1, 1}, {"dD", 1, 1}, {"fF", 1, 1}, {"gG", 1, 1}, {"hH", 1, 1}, {"jJ", 1, 1}, {"kK", 1, 1}, {"lL", 1, 1}, {"mM", 1, 1}, {"ù%", 1, 1}, {"µ", 1, 1}, {KeyType::SHIFT, 2.25f, 1}},
        {{KeyType::CTRL, 1.25f, 1}, {KeyType::ALT, 1.25f, 1}, {KeyType::SPACE, 5.25f, 1}, {KeyType::ALT, 1.25f, 1}, {KeyType::CTRL, 1.25f, 1}}
    };


    float x = 0;
    float y = 0;

    for (auto& row : rows) {       

        x = 0;

        for (auto& key : row) {

            key.setX(x);
            key.setY(y);

            x = x + key.getXSpan();

            mKeys.push_back(key);

        }

        y = y + 1;

    }



}
