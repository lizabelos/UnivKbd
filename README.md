# UnivKbd
Alternative Cross-Platform On-Screen Keyboard for Qt5 and Qt6

![UnivKbd](images/capture.png)

## How to build

```
mkdir build
cmake ..
cmake --build .
```

Install with : 

```
cmake --install .
```

## Usage

To link with cmake : 
```cmake
find_package(UnivKbd REQUIRED)
target_link_libraries(myapp PRIVATE UnivKbd::UnivKbd)
```

With autotools, link to the library `libUnivKbd.a` or `libUnivKbd.lib`.

To create a floating keyboard that will show up when a widget is focused, use the following code:
```cpp
#include <UnivKbd/UnivKbd>

QWidget *anyWidget = ...;
UnivKbd::VirtualKeyboard *keyboard = new VirtualKeyboard(mTextEditor);
```

If you don't want the keyboard to float, you can add it whenever you want to your widget's layout:
```cpp
anyWidget->layout()->addWidget(keyboard);
```

And that's this simple !!