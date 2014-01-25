####### Compiler, tools and options
CC              = gcc
CXX             = g++
CFLAGS          = -Wall -Wextra -pipe -O2 -fPIE
CXXFLAGS        = -Wall -Wextra -pipe -O2 -fPIE
INCPATH         = -I. -I/usr/include/json-glib-1.0 -I/usr/include/glib-2.0 -I/usr/include/unity/unity -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/include/dee-1.0 -I/usr/include/libdbusmenu-glib-0.4
LFLAGS          = -Wl,-O1
LIBS            = -lunity -lglib-2.0 -lgobject-2.0 -lgio-2.0 -ljson-glib-1.0
SED             = sed
STRIP           = strip
INSTALL_FILE    = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE        = rm -f
MKDIR           = mkdir -p


####### Files
SOURCES       = spotify.c \
		spotify-parser.c 
OBJECTS       = spotify.o \
		spotify-parser.o
TARGET        = unity-scope-spotify


####### Build rules
all: $(TARGET)

$(TARGET): servicefilegen $(OBJECTS)  
	$(CXX) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

servicefilegen: data/unity-scope-spotify.service.in
	sed -e "s,\(Exec=\).*$$,\1/usr/lib/i386-linux-gnu/unity-scope-spotify/${TARGET}," /home/antonio/mybin/spotify-scope/data/unity-scope-spotify.service.in > /home/antonio/mybin/spotify-scope/data/unity-scope-spotify.service

clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) data/unity-scope-spotify.service data/*~
	-$(DEL_FILE) *~


####### Compile
spotify.o: spotify.c config.h spotify-parser.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o spotify.o spotify.c

spotify-parser.o: spotify-parser.c spotify-parser.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o spotify-parser.o spotify-parser.c


####### Install
install_target: all
	@test -d $(INSTALL_ROOT)/usr/lib/i386-linux-gnu/unity-scope-spotify || mkdir -p $(INSTALL_ROOT)/usr/lib/i386-linux-gnu/unity-scope-spotify
	-$(INSTALL_PROGRAM) "$(TARGET)" "$(INSTALL_ROOT)/usr/lib/i386-linux-gnu/unity-scope-spotify/$(TARGET)"
	-$(STRIP) "$(INSTALL_ROOT)/usr/lib/i386-linux-gnu/unity-scope-spotify/$(TARGET)"

install_scopefile: all
	@test -d $(INSTALL_ROOT)/usr/share/unity/scopes/music || mkdir -p $(INSTALL_ROOT)/usr/share/unity/scopes/music
	-$(INSTALL_FILE) /home/antonio/mybin/spotify-scope/data/spotify.scope $(INSTALL_ROOT)/usr/share/unity/scopes/music/

install_servicefile: all
	@test -d $(INSTALL_ROOT)/usr/share/dbus-1/services || mkdir -p $(INSTALL_ROOT)/usr/share/dbus-1/services
	-$(INSTALL_FILE) /home/antonio/mybin/spotify-scope/data/unity-scope-spotify.service $(INSTALL_ROOT)/usr/share/dbus-1/services/

install: install_target install_scopefile install_servicefile
