include(../subdirs.pri)

HEADERS += \
    shaderdb.hpp \
    metainfo.hpp

SOURCES += \
    shaderdb.cpp \
    metainfo.cpp

unix {
  VERSION_SOURCES = ../version
  version.input = VERSION_SOURCES
  version.depend_command = ruby ../update_version.rb --dep
  version.commands = ruby ../update_version.rb
  version.output = ../version.hpp
  version.variable_out = HEADERS
  QMAKE_EXTRA_COMPILERS += version
}

INCLUDEPATH += ../app
