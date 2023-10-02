cmake_minimum_required(VERSION 3.0)
project(UnivKbd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

set(QT_VERSION 6)
find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
if (NOT Qt6_FOUND)
    set(QT_VERSION 5)
    find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
endif()
if (NOT Qt5_FOUND AND NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt5 or Qt6 not found")
endif()

# Enable all errors
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# QRC Function
function(generate_qrc TARGET_NAME SOURCE_DIR QRC_FILE_NAME)
    # Scan source directory and build list of files
    file(GLOB_RECURSE SRC_FILES "${SOURCE_DIR}/*")

    # Write the header of the .qrc file
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${QRC_FILE_NAME}.qrc" "<RCC>\n")
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${QRC_FILE_NAME}.qrc" "<qresource>\n")

    # Write each file into the .qrc file
    foreach(SRC_FILE ${SRC_FILES})
        file(RELATIVE_PATH REL_PATH ${SOURCE_DIR} ${SRC_FILE})
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${QRC_FILE_NAME}.qrc" "<file alias='${REL_PATH}'>${SRC_FILE}</file>\n")
    endforeach()

    # Close the .qrc file
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${QRC_FILE_NAME}.qrc" "</qresource>\n")
    file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/${QRC_FILE_NAME}.qrc" "</RCC>\n")

    # Generate resource source file
    qt_add_resources(QRC_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${QRC_FILE_NAME}.qrc")

    # Add generated source to target
    target_sources(${TARGET_NAME} PRIVATE ${QRC_SOURCES})
endfunction()

# Library
add_library(UnivKbd STATIC
        UnivKbd/SimpleTextEditor.h
        UnivKbd/SimpleTextEditor.cpp
        UnivKbd/VirtualKeyboard.h
        UnivKbd/VirtualKeyboard.cpp
        UnivKbd/Keyboard.h
        UnivKbd/Key.h
        UnivKbd/VirtualKeyboardButton.cpp
        UnivKbd/VirtualKeyboardButton.h
        UnivKbd/VirtualKeyboardInnerWidget.cpp
        UnivKbd/VirtualKeyboardInnerWidget.h
        UnivKbd/VirtualKeyboardConfigurationWidget.cpp
        UnivKbd/VirtualKeyboardConfigurationWidget.h
        UnivKbd/CustomDockWidget.h
        )

generate_qrc(UnivKbd "${CMAKE_CURRENT_LIST_DIR}/keyboards" "keyboards")
generate_qrc(UnivKbd "${CMAKE_CURRENT_LIST_DIR}/icons" "icons")

if (MINGW)
    target_compile_options(UnivKbd PRIVATE -municode)
endif()

# OS Dependant sources
if (WIN32)
    target_sources(UnivKbd PRIVATE
            UnivKbd/Keyboard.windows.cpp
    )
else()
    target_sources(UnivKbd PRIVATE
            UnivKbd/Keyboard.default.cpp
    )
endif()

if (QT_VERSION EQUAL 5)
    target_link_libraries(UnivKbd Qt5::Core Qt5::Gui Qt5::Widgets)
else()
    target_link_libraries(UnivKbd Qt6::Core Qt6::Gui Qt6::Widgets)
endif()

# Test

option(BUILD_TESTS "Build tests" OFF)
if (BUILD_TESTS)
add_executable(
        UnivKbdTest
        main.cpp
)

target_link_libraries(UnivKbdTest UnivKbd)
endif ()

# Install

# Library
install(TARGETS UnivKbd
        ARCHIVE DESTINATION lib
        )

# Headers
install(FILES
        UnivKbd/SimpleTextEditor.h
        UnivKbd/VirtualKeyboard.h
        UnivKbd/Keyboard.h
        UnivKbd/Key.h
        UnivKbd/UnivKbd
        UnivKbd/VirtualKeyboardButton.h
        UnivKbd/VirtualKeyboardInnerWidget.h
        UnivKbd/VirtualKeyboardConfigurationWidget.h
        UnivKbd/CustomDockWidget.h
        DESTINATION include/UnivKbd
        )

# CMake package configuration
include(CMakePackageConfigHelpers)

# Generate UnivKbdConfig.cmake
configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/UnivKbdConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/UnivKbdConfig.cmake"
        INSTALL_DESTINATION lib/cmake/UnivKbd
)

# Generate UnivKbdConfigVersion.cmake
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/UnivKbdConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY SameMajorVersion
)

# Install package configuration files
install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/UnivKbdConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/UnivKbdConfigVersion.cmake"
        DESTINATION lib/cmake/UnivKbd
)

# Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/UnivKbd)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_PROJECT_NAME "UnivKbd")
    set(DOXYGEN_PROJECT_VERSION "1.0.0")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    set(DOXYGEN_GENERATE_XML YES)
    
    doxygen_add_docs(doc ${DOXYGEN_INPUT})
endif()