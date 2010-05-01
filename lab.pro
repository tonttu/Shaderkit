QT += opengl

QMAKE_EXTRA_COMPILERS += bison
bison.output = shader/glsl.tab.cpp shader/glsl.tab.hpp
bison.commands = bison -v -d -t -o shader/glsl.tab.cpp ${QMAKE_FILE_IN}
bison.input = BISON_FILES
bison.CONFIG = no_link
BISON_FILES = shader/glsl.y

QMAKE_EXTRA_COMPILERS += flex
flex.output = shader/lex.yy.c
flex.commands = flex -o shader/lex.yy.c ${QMAKE_FILE_IN}
flex.input = FLEX_FILES
flex.CONFIG = no_link
FLEX_FILES = shader/glsl.l

HEADERS += forward.hpp \
    shader/grammar.hpp \
    highlighter.hpp \
    shader/lexer.hpp \
    watcher.hpp \
    ext/glut_teapot.hpp \
    shader/glsl.tab.hpp \
    state.hpp \
    json_value.hpp \
    light.hpp \
    camera.hpp \
    object3d.hpp \
    shader/error.hpp \
    shader/compiler_output_parser.hpp \
    shader/uniform.hpp \
    shader/shader.hpp \
    shader/program.hpp \
    editor.hpp \
    renderpass.hpp \
    scene.hpp \
    project.hpp \
    glwidget.hpp \
    properties.hpp \
    mainwindow.hpp

SOURCES += shader/glsl.tab.cpp \
    shader/lex.yy.c \
    shader/grammar.cpp \
    highlighter.cpp \
    watcher.cpp \
    ext/glut_teapot.c \
    main.cpp \
    state.cpp \
    json_value.cpp \
    light.cpp \
    json_parser.cpp \
    camera.cpp \
    object3d.cpp \
    shader/error.cpp \
    shader/compiler_output_parser.cpp \
    shader/uniform.cpp \
    shader/shader.cpp \
    shader/program.cpp \
    editor.cpp \
    renderpass.cpp \
    scene.cpp \
    project.cpp \
    glwidget.cpp \
    properties.cpp \
    mainwindow.cpp

OTHER_FILES += doc/style.cpp \
    std/150.all \
    std/150.compat.all \
    std/150.compat.frag \
    std/150.compat.geom \
    std/150.compat.vert \
    std/150.frag \
    std/150.geom \
    std/150.vert

FORMS += mainwindow.ui

QMAKE_CXXFLAGS += -DGL_GLEXT_PROTOTYPES

include(qtpropertybrowser/src/qtpropertybrowser.pri)

QMAKE_EXTRA_TARGETS += license
license.commands = ./update_license.rb -a
