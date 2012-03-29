TEMPLATE = subdirs
SUBDIRS = external \
          shaderdb \
          application
unix:SUBDIRS += parser

external.subdir = ext
shaderdb.subdir = shaderdb
application.subdir = app
application.depends = external shaderdb
unix {
  parser.subdir = tests/parser
  parser.depends = application
}

OTHER_FILES += \
    doc/style.cpp \
    std/150.all \
    std/150.compat.all \
    std/150.compat.frag \
    std/150.compat.geom \
    std/150.compat.vert \
    std/150.frag \
    std/150.geom \
    std/150.vert \
    examples/phong-teapot.shaderkit

QMAKE_EXTRA_TARGETS += license
license.commands = ./update_license.rb -a

docs.files = COPYING README
docs.path = /docs

examples.files = examples/*
examples.path = /examples

std.files = std/*
std.path = /std

INSTALLS += docs examples std
