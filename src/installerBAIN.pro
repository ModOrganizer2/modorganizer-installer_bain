#-------------------------------------------------
#
# Project created by QtCreator 2013-01-01T18:46:50
#
#-------------------------------------------------

TARGET = installerBAIN
TEMPLATE = lib

CONFIG += plugins
CONFIG += dll

CONFIG(release, debug|release) {
  QMAKE_CXXFLAGS += /Zi
  QMAKE_LFLAGS += /DEBUG
}

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets declarative script
} else {
  QT += declarative script
}

DEFINES += INSTALLERBAIN_LIBRARY

SOURCES += installerbain.cpp \
    baincomplexinstallerdialog.cpp

HEADERS += installerbain.h \
    baincomplexinstallerdialog.h

include(../plugin_template.pri)

FORMS += \
    baincomplexinstallerdialog.ui

OTHER_FILES += \
    installerbain.json\
    SConscript
