TEMPLATE = subdirs
SUBDIRS = external \
          shaderdb \
          application

external.subdir = ext
shaderdb.subdir = shaderdb
application.subdir = app
application.depends = external shaderdb

OTHER_FILES += \
    doc/style.cpp \
    std/150.all \
    std/150.compat.all \
    std/150.compat.frag \
    std/150.compat.geom \
    std/150.compat.vert \
    std/150.frag \
    std/150.geom \
    std/150.vert

QMAKE_EXTRA_TARGETS += license
license.commands = ./update_license.rb -a
