TARGET = eboot
OBJS = eboot.o
OBJS += ../regedit_prx/regedit_prx.S

INCDIR = ./inc ./inc_lib
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ./lib ./inc_lib
#LIBS = 
LDFLAGS = -L.

PSP_FW_VERSION = 100
PSP_FW_VERSION_SFO = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = IDPS Dumper v0.2
#PSP_EBOOT_ICON = 

#ENCRYPT = 1
BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
