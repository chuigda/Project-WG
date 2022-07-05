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

set(PROJECT_SOURCES
    main.cc)
