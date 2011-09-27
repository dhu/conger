MYFLAGS = -DHAS_SOCKLEN_T -g

AM_CFLAGS = @CFLAGS@ $(MYFLAGS)
AM_CXXFLAGS = @CXXFLAGS@ $(MYFLAGS)

lib_LIBRARIES = libenet.a
libenet_a_SOURCES = host.c list.c memory.c packet.c peer.c protocol.c unix.c win32.c
INCLUDES = -Iinclude/

SUBDIRS = include
