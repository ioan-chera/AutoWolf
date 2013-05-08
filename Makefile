CONFIG ?= config.default
-include $(CONFIG)
CC = gcc
CXX = g++ -std=c++11

BINARY    ?= autowolf
PREFIX    ?= /usr/local
MANPREFIX ?= $(PREFIX)

INSTALL         ?= install
INSTALL_PROGRAM ?= $(INSTALL) -m 555 -s
INSTALL_MAN     ?= $(INSTALL) -m 444
INSTALL_DATA    ?= $(INSTALL) -m 444


SDL_CONFIG  ?= sdl-config
CFLAGS_SDL  ?= $(shell $(SDL_CONFIG) --cflags)
LDFLAGS_SDL ?= $(shell $(SDL_CONFIG) --libs)


CFLAGS += $(CFLAGS_SDL)

#CFLAGS += -Wall
#CFLAGS += -W
CFLAGS += -Wpointer-arith
CFLAGS += -Wreturn-type
CFLAGS += -Wwrite-strings
CFLAGS += -Wcast-align

ifdef GPL
    CFLAGS += -DUSE_GPL
endif

CFLAGS += -DMEMMEM_NOT_DEFINED

CCFLAGS += $(CFLAGS)
CCFLAGS += -std=gnu99
CCFLAGS += -Werror-implicit-function-declaration
CCFLAGS += -Wimplicit-int
CCFLAGS += -Wsequence-point

CXXFLAGS += $(CFLAGS)

LDFLAGS += $(LDFLAGS_SDL)
LDFLAGS += -lSDL_mixer
ifneq (,$(findstring MINGW,$(shell uname -s)))
LDFLAGS += -static-libgcc
endif

SRCS :=
SRCS += src/audioabstract.cpp
SRCS += src/CheckSum.cpp
SRCS += src/DataFile.cpp
SRCS += src/DirectoryFile.cpp
SRCS += src/Exception.cpp
SRCS += src/dosbox/dbopl.cpp
SRCS += src/e_hashkeys.cpp
SRCS += src/gfxvabstract.cpp
SRCS += src/HistoryRatio.cpp
SRCS += src/id_ca.cpp
SRCS += src/id_in.cpp
SRCS += src/id_pm.cpp
SRCS += src/id_sd.cpp
SRCS += src/id_us_1.cpp
SRCS += src/id_vh.cpp
SRCS += src/id_vl.cpp
SRCS += src/ioan_bas.cpp
SRCS += src/ioan_bot.cpp
SRCS += src/ioan_secret.cpp
SRCS += src/MasterDirectoryFile.cpp
SRCS += src/obattrib.cpp
SRCS += src/PathArray.cpp
SRCS += src/Property.cpp
SRCS += src/PropertyFile.cpp
SRCS += src/PString.cpp
SRCS += src/sdl_winmain.cpp
SRCS += src/signon.cpp
SRCS += src/SODFlag.cpp
SRCS += src/sprabstract.cpp
SRCS += src/wl_act1.cpp
SRCS += src/wl_act2.cpp
SRCS += src/wl_agent.cpp
SRCS += src/wl_atmos.cpp
SRCS += src/wl_cloudsky.cpp
SRCS += src/wl_debug.cpp
SRCS += src/wl_dir3dspr.cpp
SRCS += src/wl_draw.cpp
SRCS += src/wl_floorceiling.cpp
SRCS += src/wl_game.cpp
SRCS += src/wl_inter.cpp
SRCS += src/wl_main.cpp
SRCS += src/wl_menu.cpp
SRCS += src/wl_parallax.cpp
SRCS += src/wl_play.cpp
SRCS += src/wl_shade.cpp
SRCS += src/wl_state.cpp
SRCS += src/wl_text.cpp

DEPS = $(filter %.d, $(SRCS:.c=.d) $(SRCS:.cpp=.d) $(SRCS:.m=.d) $(SRCS:.mm=.d))
OBJS = $(filter %.o, $(SRCS:.c=.o) $(SRCS:.cpp=.o) $(SRCS:.m=.o) $(SRCS:.mm=.o))

.SUFFIXES:
.SUFFIXES: .c .cpp .m .mm .d .o

Q ?= @

all: $(BINARY)

ifndef NO_DEPS
depend: $(DEPS)

ifeq ($(findstring $(MAKECMDGOALS), clean depend Data),)
-include $(DEPS)
endif
endif

$(BINARY): $(OBJS)
	@echo '===> LD $@'
	$(Q)$(CXX) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

.c.o:
	@echo '===> CC $<'
	$(Q)$(CC) $(CCFLAGS) -c $< -o $@
	
.m.o:
	@echo '===> CC $<'
	$(Q)$(CC) $(CCFLAGS) -c $< -o $@

.cpp.o:
	@echo '===> CXX $<'
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

.mm.o:
	@echo '===> CXX $<'
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@

.c.d:
	@echo '===> DEP $<'
	$(Q)$(CC) $(CCFLAGS) -MM $< | sed 's#^$(@F:%.d=%.o):#$@ $(@:%.d=%.o):#' > $@

.cpp.d:
	@echo '===> DEP $<'
	$(Q)$(CXX) $(CXXFLAGS) -MM $< | sed 's#^$(@F:%.d=%.o):#$@ $(@:%.d=%.o):#' > $@

clean distclean:
	@echo '===> CLEAN'
	$(Q)rm -fr $(DEPS) $(OBJS) $(BINARY) $(BINARY).exe

install: $(BINARY)
	@echo '===> INSTALL'
	$(Q)$(INSTALL) -d $(PREFIX)/bin
	$(Q)$(INSTALL_PROGRAM) $(BINARY) $(PREFIX)/bin
