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
find_package(glm REQUIRED)

# if we are using Windows
if (WIN32)
    # more specifically, if we are using MSVC
    if (MSVC)
        message(FATAL_ERROR "This program was not designed as MSVC compatible")
    endif()

    # add definition
    add_definitions(-DCW_WIN32)

    # enable RC processing
    enable_language(RC)

    set(CWUTIL_RESC resc/win32/CWUtil.rc include/Version.h)
    set(CWGLX_RESC resc/win32/CWGLX.rc include/Version.h)
    set(PROJECT_WG_RESC resc/win32/Main.rc)
    set(PHONG_TESTER_RESC resc/win32/PhongTester.rc)

    set(GLM_LINK_NAME glm::glm)
else()
    set(GLM_LINK_NAME glm)
endif()

set(CWGLX_SOURCES

    src/cwglx/Setup.cc
    src/cwglx/Base/VertexArrayObject.cc
    src/cwglx/Base/ElementBufferObject.cc
    src/cwglx/Base/Texture.cc
    src/cwglx/Base/Shader.cc
    src/cwglx/Base/ShaderProgram.cc
    src/cwglx/Object/Vertex.cc
    src/cwglx/Object/Material.cc
    src/cwglx/Object/Object.cc
    src/cwglx/Object/WavefrontLoader.cc
    src/cwglx/GL/GLInfo.cc
    include/cwglx/Setup.h
    include/cwglx/Base/VertexArrayObject.h
    include/cwglx/Base/VertexBufferObject.h
    include/cwglx/Base/VertexBufferObjectImpl.h
    include/cwglx/Base/VBOImpl/GLM.h
    include/cwglx/Base/ElementBufferObject.h
    include/cwglx/Base/Texture.h
    include/cwglx/Base/Shader.h
    include/cwglx/Base/ShaderProgram.h
    include/cwglx/Object/Vertex.h
    include/cwglx/Object/Material.h
    include/cwglx/Object/Object.h
    include/cwglx/Object/WavefrontLoader.h
    include/cwglx/GL/GL.h
    include/cwglx/GL/GLImpl.h
    include/cwglx/GL/GLInfo.h
)

# CWGLX
set_property(SOURCE ${CWGLX_SOURCES} PROPERTY SKIP_AUTOMOC ON)

qt_add_library(CWGLX ${CWGLX_SOURCES} ${CWGLX_RESC})

target_link_libraries(CWGLX PRIVATE
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      ${GLM_LINK_NAME}
                      CWUtil)

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

target_link_libraries(CWUtil PRIVATE Qt6::Core)

# Main executable
set(PROJECT_SOURCES
    main.cc

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
    src/ui_next/track/MPTrackControl.cc
    src/ui_next/track/ManualTrackControl.cc
    src/ui_next/SoundControl.cc
    src/ui_next/ExtraControl.cc
    src/ui_next/HelpBox.cc
    src/ui_next/LicensePresenter.cc
    src/ui_next/GLWindow.cc
    src/ui_next/SearchDialog.cc
    src/ui_next/ShaderEdit.cc
    src/ui_next/ShaderHighlighter.cc
    src/ui_next/CodeEdit.cc
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
    include/ui_next/HelpBox.h
    include/ui_next/LicensePresenter.h
    include/ui_next/GLWindow.h
    include/ui_next/CloseSignallingWidget.h
    include/ui_next/SearchDialog.h
    include/ui_next/ShaderEdit.h
    include/ui_next/ShaderHighlighter.h
    include/ui_next/CodeEdit.h

    src/wgc0310/Mesh.cc
    src/wgc0310/Shader.cc
    # src/wgc0310/ScreenGlass.cc
    src/wgc0310/Screen.cc
    src/wgc0310/ScreenCurveHelper.cc
    src/wgc0310/ScreenAnimationStatus.cc
    src/wgc0310/BodyAnim.cc
    src/wgc0310/BodyStatus.cc
    include/wgc0310/Mesh.h
    include/wgc0310/Shader.h
    # include/wgc0310/ScreenGlass.h
    include/wgc0310/Screen.h
    include/wgc0310/ScreenCurveHelper.h
    include/wgc0310/ScreenAnimationStatus.h
    # include/wgc0310/BodyStatus.h
    # include/wgc0310/BodyAnim.h
    # include/wgc0310/HeadStatus.h
    include/wgc0310/AttachmentStatus.h

    include/wgc0310/api/ScreenAnimation.h
    include/wgc0310/api/Attachment.h
    include/wgc0310/api/WGAPI.h

    include/Version.h

    resc/main.qrc)

include_directories(include)

qt_add_executable(Project-WG MANUAL_FINALIZATION ${PROJECT_SOURCES} ${PROJECT_WG_RESC})
target_link_libraries(Project-WG PRIVATE
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      Qt6::Multimedia
                      Qt6::WebSockets
                      ${GLM_LINK_NAME}
                      CWGLX
                      CWUtil)

set_target_properties(Project-WG PROPERTIES WIN32_EXECUTABLE TRUE)

qt_finalize_executable(Project-WG)

# Attachments
qt_add_library(Emitter SHARED extra/attachments/Emitter/Emitter.cc
                              extra/attachments/Emitter/emitter.qrc)
target_link_libraries(Emitter PRIVATE
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX
                      CWUtil)

# Animations
qt_add_library(Taunter SHARED extra/animations/Taunter/Taunter.cc
                              extra/animations/Taunter/taunter.qrc)
target_link_libraries(Taunter PRIVATE
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX
                      CWUtil)

qt_add_library(TypeWriter SHARED extra/animations/TypeWriter/TypeWriter.cc
                                 extra/animations/TypeWriter/TypeWriterControl.cc
                                 extra/animations/TypeWriter/TypeWriterControl.h)
target_link_libraries(TypeWriter PRIVATE
                      Qt6::Widgets
                      Qt6::OpenGLWidgets
                      CWGLX
                      CWUtil)
