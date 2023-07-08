cmake_minimum_required(VERSION 3.25)
project(UniQKey)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

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
add_library(UniQKey STATIC
        UniQKey/SimpleTextEditor.h
        UniQKey/SimpleTextEditor.cpp
        UniQKey/VirtualKeyboard.h
        UniQKey/VirtualKeyboard.cpp
        UniQKey/Keyboard.h
        UniQKey/Key.h
        )

generate_qrc(UniQKey "${CMAKE_CURRENT_LIST_DIR}/keyboards" "keyboards")

# OS Dependant sources
if (WIN32 AND NOT MINGW)
    target_sources(UniQKey PRIVATE
            UniQKey/Keyboard.windows.cpp
    )
else()
    target_sources(UniQKey PRIVATE
            UniQKey/Keyboard.default.cpp
    )
endif()

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

# Doxygen
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/UniQKey)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_MAN NO)
    set(DOXYGEN_PROJECT_NAME "UniQKey")
    set(DOXYGEN_PROJECT_VERSION "1.0.0")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    set(DOXYGEN_GENERATE_XML YES)
    
    doxygen_add_docs(doc ${DOXYGEN_INPUT})
endif()