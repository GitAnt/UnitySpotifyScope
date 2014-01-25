TEMPLATE = app
TARGET = spotify-scope
INCLUDEPATH += . /usr/include/json-glib-1.0 /usr/include/glib-2.0 /usr/include/unity/unity /usr/lib/i386-linux-gnu/glib-2.0/include /usr/include/dee-1.0 /usr/include/libdbusmenu-glib-0.4
LIBS += -lunity -lmrss -lnxml -lglib-2.0 -lgobject-2.0 -lgio-2.0 -ljson-glib-1.0

# Input
HEADERS += config.h spotify-parser.h
SOURCES += spotify-parser.c spotify.c
