def export(lst) lst.join(" \\\n    ") end
puts "HEADERS += #{export Dir['assimp/**/*.h{,pp}']}"
puts
puts "SOURCES += #{export Dir['assimp/**/*.c{,pp,c}']}"
puts "
INCLUDEPATH += assimp/assimp assimp/code/BoostWorkaround assimp/contrib/unzip
DEFINES += ASSIMP_BUILD_NO_EXPORT
DEFINES += ASSIMP_BUILD_BOOST_WORKAROUND

win32 {
  DEFINES += _SCL_SECURE_NO_WARNINGS _CRT_SECURE_NO_WARNINGS
}
"
