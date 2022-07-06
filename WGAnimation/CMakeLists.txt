cmake_minimum_required(VERSION 3.5)
project(WGAnimation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
        "-Wall -Wextra -pedantic -Wno-unknown-pragmas ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS
        "-Wall -Wextra -pedantic -Wno-unknown-pragmas ${CMAKE_C_FLAGS}")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)

include_directories(include)

set(PROJECT_SOURCES src/lib.cc)
add_library(WGAnimation SHARED ${PROJECT_SOURCES})
target_link_libraries(WGAnimation PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

set(PLAYGROUND_SOURCES
    playground/main.cc
    playground/PlaygroundWidget.cc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Playground MANUAL_FINALIZATION ${PLAYGROUND_SOURCES})
else()
    qt_add_executable(Playground ${PLAYGROUND_SOURCES})
endif()

target_link_libraries(Playground PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::OpenGLWidgets)
set_target_properties(Playground PROPERTIES WIN32_EXECUTABLE TRUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Playground)
endif()

if(WIN32)
  add_definitions(-DCW_WIN32)
endif()
