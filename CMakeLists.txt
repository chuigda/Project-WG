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
             Multimedia
             OpenGLWidgets)

set(PROJECT_SOURCES
    main.cc

    src/ui_next/EntityStatus.cc
    src/ui_next/ControlPanel.cc
    src/ui_next/EntityControl.cc
    src/ui_next/BodyControl.cc
    src/ui_next/ScreenAnimationControl.cc
    src/ui_next/track/FaceTrackControl.cc
    src/ui_next/track/TrackControlImpl.h
    src/ui_next/track/VTSTrackControl.cc
    src/ui_next/track/OSFTrackControl.cc
    src/ui_next/track/ManualTrackControl.cc
    src/ui_next/SoundControl.cc
    src/ui_next/LicensePresenter.cc
    src/ui_next/GLWindow.cc
    include/ui_next/EntityStatus.h
    include/ui_next/ControlPanel.h
    include/ui_next/EntityControl.h
    include/ui_next/BodyControl.h
    include/ui_next/ScreenAnimationControl.h
    include/ui_next/FaceTrackControl.h
    include/ui_next/SoundControl.h
    include/ui_next/LicensePresenter.h
    include/ui_next/GLWindow.h
    include/ui_next/CloseSignallingWidget.h

    src/util/FileUtil.cc
    src/util/DynLoad.cc
    src/util/Constants.cc
    include/util/FileUtil.h
    include/util/DynLoad.h
    include/util/Wife.h
    include/util/Constants.h
    include/util/CircularBuffer.h
    include/util/Derive.h
    include/util/Sinkrate.h
    include/util/Logger.h

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
    src/cwglx/drawable/Composition.cc
    include/cwglx/drawable/PlainTriangles.h
    include/cwglx/drawable/TriangleGen.h
    include/cwglx/drawable/Composition.h

    src/wgc0310/Mesh.cc
    src/wgc0310/ScreenGlass.cc
    src/wgc0310/Screen.cc
    src/wgc0310/ScreenCurveHelper.cc
    src/wgc0310/ScreenAnimationStatus.cc
    src/wgc0310/BodyAnim.cc
    src/wgc0310/BodyStatus.cc
    include/wgc0310/Mesh.h
    include/wgc0310/ScreenGlass.h
    include/wgc0310/Screen.h
    include/wgc0310/ScreenCurveHelper.h
    include/wgc0310/ScreenAnimation.h
    include/wgc0310/ScreenAnimationStatus.h
    include/wgc0310/BodyStatus.h
    include/wgc0310/BodyAnim.h
    include/wgc0310/HeadStatus.h

    resc/main.qrc)

qt6_wrap_ui(PROJECT_SOURCES src/ui/ConfigWidget.ui)
qt6_wrap_ui(PROJECT_SOURCES src/ui/MessageBoxAlter.ui)

include_directories(include)
include_directories(${OpenCV_INCLUDE_DIRS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Project-WG MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(Project-WG ${PROJECT_SOURCES})
endif()

target_link_libraries(Project-WG PRIVATE
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::OpenGLWidgets
                      Qt${QT_VERSION_MAJOR}::Multimedia)

set_target_properties(Project-WG PROPERTIES WIN32_EXECUTABLE TRUE)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Project-WG)
endif()

# if we are using Windows
if (WIN32)
    # add definition
    add_definitions(-DCW_WIN32)
endif()
