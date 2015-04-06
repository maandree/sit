# sit version
VERSION = 1.0

# Customize below to fit your system

# paths
PREFIX = /usr/local

# includes and libs
INCS =
LIBS =

# flags
CPPFLAGS = -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\"
CFLAGS   = -std=c99 -pedantic -Wall -Wextra -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = -s ${LIBS}

# compiler and linker
#CC = cc

