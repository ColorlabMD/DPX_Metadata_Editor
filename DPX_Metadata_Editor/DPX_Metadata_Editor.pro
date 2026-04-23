#-------------------------------------------------
#
# Project created by QtCreator 2015-11-18T13:09:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


VERSION = 0.9.5.0
macx {ICON = DPX_ICON.icns}




TARGET = DPX_Metadata_Editor
TEMPLATE = app
APP_NAME =

SOURCES += main.cpp\
        mainwindow.cpp \
    DPXHeaderHelper.cpp \
    aboutdialog.cpp \
    dpxviewer.cpp \
    keycodedialog.cpp \
    dpxsequence.cpp \
    rawcookedreversibilityextractor.cpp \
    reversibilityblob_rewrite.cpp \
    tcsetdialog.cpp \
    confirmwritedialog.cpp


INCLUDEPATH += $$PWD/ /opt/homebrew/include/
DEPENDPATH += $$PWD/

HEADERS  += mainwindow.h \
    DPX.h \
    DPXHeader.h \
    DPXStream.h \
    DPXHeaderHelper.h \
    aboutdialog.h \
    dpxviewer.h \
    keycodedialog.h \
    dpxsequence.h \
    rawcookedreversibilityextractor.h \
    reversibilityblob_rewrite.h \
    tcsetdialog.h \
    confirmwritedialog.h

macx {
        LIBS += -L/libdpxpath/

        QMAKE_LIBDIR_FLAGS += -L/usr/local/lib -L/opt/homebrew/lib -L/libdpxpath/
        QMAKE_RPATHDIR += @executable_path/../Frameworks/
        QMAKE_LINK += -headerpad_max_install_names
LIBS += -ldpx -lz
}
win32 {
 QMAKE_LIBDIR += "C:\LIBDPX"
LIBS += -lDPX
}


FORMS    += mainwindow.ui \
    aboutdialog.ui \
    dpxviewer.ui \
    keycodedialog.ui \
    tcsetdialog.ui \
    confirmwritedialog.ui

DISTFILES +=

RESOURCES += \
    images.qrc



