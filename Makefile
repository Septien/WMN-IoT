# Linux compilation
ifdef LINUX
unexport LINUX	# No longer needed, remove so it does not causes problems

CC = /usr/bin/gcc

# Create a static library
AR = ar csrv
RL = ranlib

# Compile each of the submodules
include MAC/Makefile.linux

BIN = bin
$(info $$LIB is [${BIN}])

ifdef TEST
SRC = main_test.c

OBJ = obj
endif

MKDIR_WMNL = mkdir -p $(BIN)
ifdef TEST
MKDIR_WMNL += $(OBJ)
endif

# Path to mclmac.a
MAC_PATH = $(mac_current_dir)/lib
$(info $$MAC_PATH is [${MAC_PATH}])

# Libraries to include
LIBS = mclmac
ifdef TEST
LIBS += mclmacT
endif
$(info $$LIBS is [${LIBS}])

# Paths to static libraries
LDFLAGS = $(MAC_PATH)
$(info $$LDFLAGS is [${LDFLAGS}])

TARGETS = directories_main
ifdef TEST
TARGETS += $(BIN)/wmnlora
endif
$(info $$TARGETS is [${TARGETS}])

all: $(TARGETS)

directories_main:
			$(MKDIR_WMNL)

ifdef TEST
$(BIN)/wmnlora : $(OBJ)/main.o
	$(CC) $(addprefix -L, $(LDFLAGS)) $(addprefix -l,$(LIBS)) -c $< -o $@ 

$(OBJ)/main_test.o : main_test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ)/main_test.o : $(INCT_DIR)/mclmac_tests.h
endif

# Remove object files
clean:
	rm $(OBJ_DIR)/*.o

# Remove libraries and executables
clear:
	rm $(LIB_MAC)/*.a
	rm $(BIN)/*.a

endif # Linux compilation

ifdef RIOT
unexport RIOT

APPLICATION = main_test

# Default board
BOARD ?= native

RIOTBASE ?= /home/jash/riot/RIOT

DEVHELP ?= 1

QUIET ?= 1

USEMODULE += MAC
EXTERNAL_MODULE_DIRS += $(CURDIR)

# Which features do you require? (mcu architecure, peripherals, sensors, etc.)
FEATURES_REQUIRED += arch_esp32
FEATURES_REQUIRED += periph_gpio

CFLAGS = -D__RIOT__

include $(RIOTBASE)/Makefile.include

endif
