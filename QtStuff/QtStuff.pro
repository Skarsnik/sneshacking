#-------------------------------------------------
#
# Project created by QtCreator 2019-06-15T16:48:53
#
#-------------------------------------------------

include(QHexView/QHexView.pri)

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtStuff
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

SOURCES += \
        asm/asmtextblock.cpp \
        asm/asmtextview.cpp \
        asm/asmtextviewsidebar.cpp \
        asm/disasm.c \
        asm/disasmhandler.cpp \
        asm/gen_asm_definition.c \
        fieldstablewidget.cpp \
        main.cpp \
        romviewer.cpp \
        rom.cpp \
        ../src/rominfo.c \
        snestype.cpp \
        wramentrydisplay.cpp \
        wramentryeditor.cpp \
        wrammap.cpp \
        structeditor.cpp \
        wrammapeditor.cpp \
        emunwaccessclient.cpp

HEADERS += \
        asm/asmline.h \
        asm/asmtextblock.h \
        asm/asmtextview.h \
        asm/asmtextviewsidebar.h \
        asm/disasm.h \
        asm/disasmhandler.h \
        fieldstablewidget.h \
        mglobal.h \
        romviewer.h \
        rom.h \
        snestype.h \
        wramentrydisplay.h \
        wramentryeditor.h \
        wrammap.h \
        structeditor.h \
        wrammapeditor.h \
        emunwaccessclient.h

FORMS += \
        romviewer.ui \
        wramentrydisplay.ui \
        wramentryeditor.ui \
        structeditor.ui \
        wrammapeditor.ui

INCLUDEPATH += $$PWD/../src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
