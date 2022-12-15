cmake_minimum_required(VERSION 3.5)

project(Project-WG VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-cast-function-type \
    -fno-rtti -fno-exceptions \
    ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS
    "-Wall -Wextra -pedantic -Wno-unknown-pragmas -Wno-cast-function-type \
    ${CMAKE_C_FLAGS}")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
             Core
             Widgets
             OpenGLWidgets
             Multimedia
             WebSockets)
find_package(Qt6 REQUIRED COMPONENTS
             Core
             Widgets
             OpenGLWidgets
             Multimedia
             WebSockets)

# if we are using Windows
if (WIN32)
    # add definition
    add_definitions(-DCW_WIN32)

    # enable RC processing
    enable_language(RC)

    set(CWUTIL_RESC resc/win32/CWUtil.rc include/Version.h)
    set(CWGLX_RESC resc/win32/CWGLX.rc include/Version.h)
    set(PROJECT_WG_RESC resc/win32/Main.rc)
    set(PHONG_TESTER_RESC resc/win32/PhongTester.rc)
endif()

set(CWGLX_SOURCES
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
    src/cwglx/RawMatrix.cc
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
    include/cwglx/RawMatrix.h

    src/cwglx/drawable/PlainTriangles.cc
    src/cwglx/drawable/TriangleGen.cc
    src/cwglx/drawable/Composition.cc
    include/cwglx/drawable/PlainTriangles.h
    include/cwglx/drawable/TriangleGen.h
    include/cwglx/drawable/Composition.h

    src/glu/FakeGLU.cc
    include/glu/FakeGLU.h
)

# CWGLX
set_property(SOURCE ${CWGLX_SOURCES} PROPERTY SKIP_AUTOMOC ON)

qt_add_library(CWGLX ${CWGLX_SOURCES} ${CWGLX_RESC})

target_link_libraries(CWGLX PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets)

# CWUTIL
set(CWUTIL_SOURCES
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
    include/util/Logger.h)

set_property(SOURCE ${CWUTIL_SOURCES} PROPERTY SKIP_AUTOMOC ON)

qt_add_library(CWUtil ${CWUTIL_SOURCES} ${CWUTIL_RESC})

target_link_libraries(CWUtil PUBLIC Qt6::Core)

# Main executable
set(PROJECT_SOURCES
    main.cc

    src/ui_next/EntityStatus.cc
    src/ui_next/ControlPanel.cc
    src/ui_next/GLInfoDisplay.cc
    src/ui_next/EntityControl.cc
    src/ui_next/BodyControl.cc
    src/ui_next/ScreenAnimationControl.cc
    src/ui_next/AttachmentControl.cc
    src/ui_next/track/FaceTrackControl.cc
    src/ui_next/track/TrackControlImpl.h
    src/ui_next/track/VTSTrackControl.cc
    src/ui_next/track/OSFTrackControl.cc
    src/ui_next/track/ManualTrackControl.cc
    src/ui_next/SoundControl.cc
    src/ui_next/ExtraControl.cc
    src/ui_next/AboutBox.cc
    src/ui_next/LicensePresenter.cc
    src/ui_next/GLWindow.cc
    include/ui_next/EntityStatus.h
    include/ui_next/ControlPanel.h
    include/ui_next/GLInfoDisplay.h
    include/ui_next/EntityControl.h
    include/ui_next/BodyControl.h
    include/ui_next/ScreenAnimationControl.h
    include/ui_next/AttachmentControl.h
    include/ui_next/FaceTrackControl.h
    include/ui_next/SoundControl.h
    include/ui_next/ExtraControl.h
    include/ui_next/AboutBox.h
    include/ui_next/LicensePresenter.h
    include/ui_next/GLWindow.h
    include/ui_next/CloseSignallingWidget.h

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
    include/wgc0310/ScreenAnimationStatus.h
    include/wgc0310/BodyStatus.h
    include/wgc0310/BodyAnim.h
    include/wgc0310/HeadStatus.h
    include/wgc0310/AttachmentStatus.h

    include/wgc0310/api/ScreenAnimation.h
    include/wgc0310/api/Attachment.h
    include/wgc0310/api/WGAPI.h

    include/Version.h

    resc/main.qrc)

include_directories(include)

qt_add_executable(Project-WG MANUAL_FINALIZATION ${PROJECT_SOURCES} ${PROJECT_WG_RESC})
target_link_libraries(Project-WG PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      Qt6::Multimedia
                      Qt6::WebSockets
                      CWGLX
                      CWUtil)

set_target_properties(Project-WG PROPERTIES WIN32_EXECUTABLE TRUE)

qt_finalize_executable(Project-WG)

# Phong tester

qt_add_executable(PhongTester MANUAL_FINALIZATION
                  extra/utility/phong/PhongTester.cc
                  extra/utility/phong/PaintWidget.cc
                  extra/utility/phong/PaintWidget.h
                  extra/utility/phong/TesterWidget.cc
                  extra/utility/phong/TesterWidget.h
                  src/ui_next/EntityStatus.cc
                  include/ui_next/EntityStatus.h
                  resc/icon-only.qrc
                  ${PHONG_TESTER_RESC})

target_link_libraries(PhongTester PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX
                      CWUtil)

set_target_properties(PhongTester PROPERTIES WIN32_EXECUTABLE TRUE)

qt_finalize_executable(PhongTester)

# Attachments
qt_add_library(TrivialAttachment SHARED extra/attachments/TrivialAttachment.cc)
target_link_libraries(TrivialAttachment PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX)

qt_add_library(Emitter SHARED extra/attachments/Emitter/Emitter.cc
                              extra/attachments/Emitter/emitter.qrc)
target_link_libraries(Emitter PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX
                      CWUtil)

# Animations
qt_add_library(TrivialAnimation SHARED extra/animations/TrivialAnimation.cc)
target_link_libraries(TrivialAnimation PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX)

qt_add_library(Taunter SHARED extra/animations/Taunter/Taunter.cc
                              extra/animations/Taunter/taunter.qrc)
target_link_libraries(Taunter PUBLIC
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX
                      CWUtil)
