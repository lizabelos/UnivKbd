cmake_minimum_required(VERSION 3.25)
project(UniQKey)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

# Library
add_library(UniQKey STATIC
        UniQKey/SimpleTextEditor.h
        UniQKey/SimpleTextEditor.cpp
        UniQKey/VirtualKeyboard.h
        UniQKey/VirtualKeyboard.cpp
        UniQKey/Keyboard.cpp
        UniQKey/Keyboard.h
        )

target_link_libraries(UniQKey Qt6::Core Qt6::Gui Qt6::Widgets)

# Test

add_executable(
        UniQKeyTest
        main.cpp
)

target_link_libraries(UniQKeyTest UniQKey)

# Install

# Library
install(TARGETS UniQKey
        ARCHIVE DESTINATION lib
        )

# Headers
install(FILES
        UniQKey/SimpleTextEditor.h
        UniQKey/VirtualKeyboard.h
        UniQKey/Keyboard.h
        UniQKey/UniQKey
        DESTINATION include/UniQKey
        )

# CMake package configuration
include(CMakePackageConfigHelpers)

# Generate UniQKeyConfig.cmake
configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/UniQKeyConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/UniQKeyConfig.cmake"
        INSTALL_DESTINATION lib/cmake/UniQKey
)

# Generate UniQKeyConfigVersion.cmake
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/UniQKeyConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY SameMajorVersion
)

# Install package configuration files
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/UniQKeyConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/UniQKeyConfigVersion.cmake"
        DESTINATION lib/cmake/UniQKey
)
