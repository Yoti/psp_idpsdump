TARGET = eboot
OBJS = eboot.o

UNAME := $(shell uname)

all:
ifeq ($(UNAME), Linux)
	WINEPREFIX="$(shell pwd)/prefix/" wine bin/prxEncrypter.exe $(TARGET).prx
else
	$(shell pwd)\bin\prxEncrypter $(TARGET).prx
endif
	pack-pbp $(EXTRA_TARGETS) PARAM.SFO icon0.png NULL pic0.png pic1.png NULL data.psp NULL

INCDIR = ./include
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

LIBDIR = ./lib 
#LIBS = 
LDFLAGS = -L.

LIBS = -lpspexploit -lpsprtc


PSP_FW_VERSION = 300
PSP_FW_VERSION_SFO = 1
EXTRA_TARGETS = EBOOT.PBP

BUILD_PRX = 1


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

#clean:
	#@rm -rf eboot.prx eboot.elf eboot.o PARAM.SFO EBOOT.PBP EBOOT.PBP prefix data.psp
	#@echo Cleaned


