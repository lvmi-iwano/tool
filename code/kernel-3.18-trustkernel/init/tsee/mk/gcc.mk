
CC		:= $(CROSS_COMPILE)gcc
CXX		:= $(CROSS_COMPILE)g++
HAS_CXX	:= $(if $(shell which $(CXX) 2>/dev/null),y,n)
CPP		:= $(CROSS_COMPILE)cpp
LD		:= $(CROSS_COMPILE)ld
AR		:= $(CROSS_COMPILE)ar
NM		:= $(CROSS_COMPILE)nm
OBJCOPY	:= $(CROSS_COMPILE)objcopy
OBJDUMP	:= $(CROSS_COMPILE)objdump
READELF	:= $(CROSS_COMPILE)readelf

nostdinc	:= -nostdinc -isystem $(shell $(CC) \
			-print-file-name=include 2> /dev/null)

# Get location of libgcc from gcc
libgcc  	:= $(shell $(CC) $(comp-cflags) \
			-print-libgcc-file-name 2> /dev/null)
