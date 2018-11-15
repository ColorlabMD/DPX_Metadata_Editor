#-------------------------------------------------
#
# Project created by QtCreator 2017-01-06T15:25:22
#
#-------------------------------------------------

QT       -= gui

TARGET = DPX
TEMPLATE = lib

DEFINES += DPX_LIBRARY

SOURCES += \
    dpx-master/libdpx/Codec.cpp \
    dpx-master/libdpx/DPX.cpp \
    dpx-master/libdpx/DPXHeader.cpp \
    dpx-master/libdpx/ElementReadStream.cpp \
    dpx-master/libdpx/InStream.cpp \
    dpx-master/libdpx/OutStream.cpp \
    dpx-master/libdpx/Reader.cpp \
    dpx-master/libdpx/RunLengthEncoding.cpp \
    dpx-master/libdpx/TestFunc.cpp \
    dpx-master/libdpx/Writer.cpp




HEADERS +=\
    dpx-master/libdpx/BaseTypeConverter.h \
    dpx-master/libdpx/Codec.h \
    dpx-master/libdpx/DPX.h \
    dpx-master/libdpx/DPXHeader.h \
    dpx-master/libdpx/DPXStream.h \
    dpx-master/libdpx/ElementReadStream.h \
    dpx-master/libdpx/EndianSwap.h \
    dpx-master/libdpx/ReaderInternal.h \
    dpx-master/libdpx/RunLengthEncoding.h \
    dpx-master/libdpx/TestFunc.h \
    dpx-master/libdpx/WriterInternal.h




unix {
    target.path = /usr/local/lib/
    INSTALLS += target
}

DISTFILES += \
    dpx-master/libdpx/Makefile.am
