cmake_minimum_required(VERSION 3.5)

project(Project-WG VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-cast-function-type ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS
    "-Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-cast-function-type ${CMAKE_C_FLAGS}")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
             Widgets
             OpenGLWidgets)

set(PROJECT_SOURCES
    main.cc

    src/ui/ScreenStatus.cc
    src/ui/Animation.cc
    src/ui/CameraEntityStatus.cc
    src/ui/GLWidgetX.cc
    src/ui/GLWidget.cc
    src/ui/ConfigWidget.cc
    src/ui/MessageBoxAlter.cc
    include/ui/ScreenStatus.h
    include/ui/Animation.h
    include/ui/CameraEntityStatus.h
    include/ui/GLWidget.h
    include/ui/ConfigWidget.h
    include/ui/MessageBoxAlter.h

    src/util/Constants.cc
    src/util/Sinkrate.cc
    include/util/Wife.h
    include/util/Constants.h
    include/util/Derive.h
    include/util/Sinkrate.h

    src/glu/FakeGLU.cc
    include/glu/FakeGLU.h

    src/cwglx/Color.cc
    src/cwglx/Material.cc
    src/cwglx/MeshLoader.cc
    src/cwglx/Texture.cc
    src/cwglx/Drawable.cc
    src/cwglx/DrawableArena.cc
    src/cwglx/Vertex.cc
    src/cwglx/Setup.cc
    src/cwglx/Light.cc
    src/cwglx/GLInfo.cc
    include/cwglx/Color.h
    include/cwglx/Material.h
    include/cwglx/Texture.h
    include/cwglx/Drawable.h
    include/cwglx/DrawableArena.h
    include/cwglx/Vertex.h
    include/cwglx/Setup.h
    include/cwglx/Light.h
    include/cwglx/Defs.h
    include/cwglx/DefaultMaterials.inc
    include/cwglx/GL.h
    include/cwglx/GLImpl.h
    include/cwglx/GLInfo.h

    src/cwglx/drawable/PlainTriangles.cc
    src/cwglx/drawable/TriangleGen.cc
    src/cwglx/drawable/FineTriangleGen.cc
    src/cwglx/drawable/FineRectGen.cc
    src/cwglx/drawable/Composition.cc
    include/cwglx/drawable/PlainTriangles.h
    include/cwglx/drawable/TriangleGen.h
    include/cwglx/drawable/FineTriangleGen.h
    include/cwglx/drawable/FineRectGen.h
    include/cwglx/drawable/Composition.h

        src/wgc0310/head/ScreenGlass.cc
    src/wgc0310/head/Screen.cc
    src/wgc0310/head/ScreenCurveHelper.cc
        include/wgc0310/head/ScreenGlass.h
    include/wgc0310/head/Screen.h
    include/wgc0310/head/ScreenCurveHelper.h
    include/wgc0310/Components.h

    resc/main.qrc)

qt_wrap_ui(PROJECT_SOURCES src/ui/ConfigWidget.ui)
qt_wrap_ui(PROJECT_SOURCES src/ui/MessageBoxAlter.ui)

include_directories(include)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Project-WG  MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(Project-WG ${PROJECT_SOURCES})
endif()

target_link_libraries(Project-WG PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

set_target_properties(Project-WG PROPERTIES WIN32_EXECUTABLE TRUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Project-WG)
endif()

# if we are using Windows
if (WIN32)
    # add definition
    add_definitions(-DCW_WIN32)
endif()
