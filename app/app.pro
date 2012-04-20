include(../shaderkit.pri)

TARGET = shaderkit

unix {
QMAKE_LEX = flex
QMAKE_YACC = bison
}
win32 {
DEFINES += YY_NO_UNISTD_H
QMAKE_LEX = $$IN_PWD/../ext/win32/bin/ruby.exe $$IN_PWD/parser/abs-wrapper.rb $$IN_PWD/../ext/win32/bin/flex.exe
QMAKE_YACC = $$IN_PWD/../ext/win32/bin/ruby.exe $$IN_PWD/parser/abs-wrapper.rb $$IN_PWD/../ext/win32/bin/bison.exe
QMAKE_YACCFLAGS_MANGLE += -p $base -b $base
QMAKE_YACC_HEADER = $base.tab.h
QMAKE_YACC_SOURCE = $base.tab.c
}
LEXSOURCES += parser/pp.l parser/glsl.l
YACCSOURCES += parser/pp.y parser/glsl.y

HEADERS += \
    forward.hpp \
    core/animation.hpp \
    core/file_resource.hpp \
    core/history.hpp \
    core/log.hpp \
    core/material.hpp \
    core/mesh_manager.hpp \
    core/model.hpp \
    core/object3d.hpp \
    core/object_creator.hpp \
    core/obj_importer.hpp \
    core/renderpass.hpp \
    core/resource_locator.hpp \
    core/scene.hpp \
    core/scene_saver.hpp \
    core/utils.hpp \
    core/watcher.hpp \
    editor/editor.hpp \
    editor/highlighter.hpp \
    gl/buffer_object.hpp \
    gl/camera.hpp \
    gl/compiler_output_parser.hpp \
    gl/error.hpp \
    gl/fbo.hpp \
    gl/light.hpp \
    gl/opengl.hpp \
    gl/program.hpp \
    gl/query_shader.hpp \
    gl/scene_object.hpp \
    gl/shader.hpp \
    gl/state.hpp \
    gl/texture.hpp \
    gl/transform_feedback.hpp \
    gl/uniform.hpp \
    gl/wrap_glext.h \
    gui/camera_editor.hpp \
    gui/gizmos.hpp \
    gui/glwidget.hpp \
    gui/importer_wizard.hpp \
    gui/light_list.hpp \
    gui/limbo_dialog.hpp \
    gui/mainwindow.hpp \
    gui/new_wizard.hpp \
    gui/object_list.hpp \
    gui/properties.hpp \
    gui/render_pass_list.hpp \
    gui/render_pass_properties.hpp \
    gui/save_project.hpp \
    gui/texture_browser.hpp \
    gui/viewport.hpp \
    gui/welcome.hpp \
    parser/grammar.hpp \
    parser/lexer.hpp \
    editor/glsl_editor.hpp \
    parser/glsl_pp.hpp \
    parser/glsl_parser.hpp \
    core/color.hpp
!win32:HEADERS += gl/sandbox_compiler.hpp

SOURCES += \
    main.cpp \
    core/animation.cpp \
    core/file_resource.cpp \
    core/history.cpp \
    core/log.cpp \
    core/material.cpp \
    core/mesh_manager.cpp \
    core/model.cpp \
    core/object3d.cpp \
    core/object_creator.cpp \
    core/obj_importer.cpp \
    core/renderpass.cpp \
    core/resource_locator.cpp \
    core/scene.cpp \
    core/scene_saver.cpp \
    core/utils.cpp \
    core/watcher.cpp \
    editor/editor.cpp \
    editor/highlighter.cpp \
    gl/buffer_object.cpp \
    gl/camera.cpp \
    gl/compiler_output_parser.cpp \
    gl/error.cpp \
    gl/fbo.cpp \
    gl/light.cpp \
    gl/opengl.cpp \
    gl/program.cpp \
    gl/query_shader.cpp \
    gl/scene_object.cpp \
    gl/shader.cpp \
    gl/state.cpp \
    gl/texture.cpp \
    gl/transform_feedback.cpp \
    gl/uniform.cpp \
    gl/wrap_glext.c \
    gui/camera_editor.cpp \
    gui/gizmos.cpp \
    gui/glwidget.cpp \
    gui/importer_wizard.cpp \
    gui/light_list.cpp \
    gui/limbo_dialog.cpp \
    gui/mainwindow.cpp \
    gui/new_wizard.cpp \
    gui/object_list.cpp \
    gui/properties.cpp \
    gui/render_pass_list.cpp \
    gui/render_pass_properties.cpp \
    gui/save_project.cpp \
    gui/texture_browser.cpp \
    gui/viewport.cpp \
    gui/welcome.cpp \
    parser/grammar.cpp \
    editor/glsl_editor.cpp \
    parser/glsl_pp.cpp \
    parser/glsl_parser.cpp \
    core/color.cpp
!win32:SOURCES += gl/sandbox_compiler.cpp

FORMS += \
    gui/camera_editor.ui \
    gui/importer_wizard.ui \
    gui/light_list.ui \
    gui/limbo_dialog.ui \
    gui/mainwindow.ui \
    gui/new_wizard.ui \
    gui/object_list.ui \
    gui/render_pass_list.ui \
    gui/save_project.ui \
    gui/texture_browser.ui \
    gui/welcome.ui

win32 {
    CONFIG(release, debug|release):LIBS += ../ext/release/ext.lib ../shaderdb/release/shaderdb.lib
    CONFIG(debug, debug|release):LIBS += ../ext/debug/ext.lib ../shaderdb/debug/shaderdb.lib
}
!win32 {
    LIBS += ../ext/libext.a ../shaderdb/libshaderdb.a
}
PRE_TARGETDEPS += $$LIBS pp_lex.cpp glsl_lex.cpp

# TODO: Get rid of GLU, we don't really need it anyway
unix:LIBS += -lGLU
win32:LIBS += -lGLU32

RESOURCES += \
    ../gfx/gfx.qrc

INCLUDEPATH += $$OUT_PWD/..

target.path = $$PREFIX/bin
INSTALLS += target

!win32:DEFINES += INSTALL_PREFIX=$$PREFIX

# VERSION = $$cat(../version)

win32 {
  RC_FILE = app.rc
}
