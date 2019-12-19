TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    arithmetic_adaptive.cpp \
    huffman.cpp \
    huffman_adaptive.cpp \
    lz77.cpp \
    lzari.cpp \
    lzh.cpp \
    lzss.cpp \
    lzw.cpp \
    main.cpp \
    rle.cpp

HEADERS += \
    arithmetic_adaptive.h \
    huffman.h \
    huffman_adaptive.h \
    lz77.h \
    lzari.h \
    lzh.h \
    lzss.h \
    lzw.h \
    main.h \
    rle.h
