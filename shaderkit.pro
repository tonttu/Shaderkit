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

QMAKE_EXTRA_TARGETS += update-icons
update-icons.commands = ./gfx/update-icons.sh

unix {
  QMAKE_EXTRA_TARGETS += doxygen doxyfile
  doxygen.commands = cd $$IN_PWD && doxygen $$OUT_PWD/Doxyfile
  doxygen.depends = doxyfile

  doxyfile.commands = sed -e s/SHADERKIT_VERSION/`grep STR_HASH version.hpp|cut -d\'\"\' -f2`/ \
                          -e \"s|SHADERKIT_OUTPUT|$$OUT_PWD/docs|\" \
                          $$IN_PWD/Doxyfile.in > Doxyfile
}

docs.files = COPYING README
unix:docs.path = $$PREFIX/share/doc/shaderkit
!unix:docs.path = $$PREFIX/docs

examples.files = examples/*
unix:examples.path = $$PREFIX/share/shaderkit/examples
!unix:examples.path = $$PREFIX/examples

std.files = std/*
unix:std.path = $$PREFIX/share/shaderkit/std
!unix:std.path = $$PREFIX/std

unix {
  desktop.files = shaderkit.desktop
  desktop.path = $$PREFIX/share/applications
  INSTALLS += desktop

  icons.files = gfx/icons/*
  icons.path = $$PREFIX/share/icons/hicolor
  INSTALLS += icons

  pixmaps.files = gfx/pixmaps/*
  pixmaps.path = $$PREFIX/share/pixmaps
  INSTALLS += pixmaps
}

INSTALLS += docs examples std
