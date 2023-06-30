cmake_minimum_required(VERSION 3.25)
project(LizaQtKeyboard)

set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(LizaQtKeyboard main.cpp SimpleTextEditor.h SimpleTextEditor.cpp VirtualKeyboard.h VirtualKeyboard.cpp KeyboardLayout.cpp KeyboardLayout.h)
target_link_libraries(LizaQtKeyboard Qt6::Core Qt6::Gui Qt6::Widgets)
