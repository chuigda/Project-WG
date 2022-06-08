cmake_minimum_required(VERSION 3.5)

project(Project-WG VERSION 0.1 LANGUAGES CXX)

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
    main.cc

    src/ui/GLWidget.cc
    include/ui/GLWidget.h

    include/util/Wife.h
    include/util/Constants.h

    src/glu/FakeGLU.cc
    include/glu/FakeGLU.h

    src/cwglx/Color.cc
    src/cwglx/Material.cc
    src/cwglx/Texture.cc
    src/cwglx/Drawable.cc
    src/cwglx/Vertex.cc
    src/cwglx/Setup.cc
    include/cwglx/Color.h
    include/cwglx/Material.h
    include/cwglx/Texture.h
    include/cwglx/Drawable.h
    include/cwglx/Vertex.h
    include/cwglx/Setup.h
    include/cwglx/DefaultMaterials.inc)

include_directories(include)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Project-WG
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(Project-WG
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(Project-WG PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

set_target_properties(Project-WG PROPERTIES WIN32_EXECUTABLE TRUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Project-WG)
endif()
