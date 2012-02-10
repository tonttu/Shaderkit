include(../shaderkit.pri)

QMAKE_CXXFLAGS += -std=c++0x

TARGET = shaderkit

unix {
QMAKE_LEX = flex
QMAKE_YACC = bison
LEXSOURCES += parser/pp.l parser/glsl.l
YACCSOURCES += parser/pp.y parser/glsl.y
}
!unix {
DEFINES += YY_NO_UNISTD_H
HEADERS += glsl_yacc.h
SOURCES += glsl_yacc.cpp glsl_lex.cpp
}

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
    gl/sandbox_compiler.hpp \
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
    parser/glpp.hpp \
    parser/grammar.hpp \
    parser/lexer.hpp

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
    gl/sandbox_compiler.cpp \
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
    parser/glpp.cpp \
    parser/grammar.cpp

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

LIBS += -lGLU

RESOURCES += \
    ../gfx/gfx.qrc
