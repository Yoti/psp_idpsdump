TARGET = regedit
OBJS = regedit_prx.o

INCDIR = inc_lib
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 300

BUILD_PRX = 1
PRX_EXPORTS = regedit_exp.exp
USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

LIBDIR = inc_lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
