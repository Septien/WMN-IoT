# Linux compilation
ifdef LINUX
unexport LINUX	# No longer needed, remove so it does not causes problems

CC = /usr/bin/gcc

# Create a static library
AR = ar csrv
RL = ranlib

CFLAGS += -fpic -fstack-protector-strong -Werror -Wall -Wextra -pedantic -g3 -Og -std=gnu11 -fstack-protector-all -ffunction-sections -fwrapv -Wstrict-overflow -fno-common -fdata-sections -Wmissing-include-dirs -fno-delete-null-pointer-checks -fdiagnostics-color -Wstrict-prototypes -Wold-style-definition -gz -Wformat=2 -Wformat-overflow -Wformat-truncation

# Compile each of the submodules
include MAC/Makefile.linux

BIN = bin/linux-x86_64
$(info $$LIB is [${BIN}])

ifdef TEST
SRC = main_test.c

OBJ = obj
$(info $$OBJ is [$(OBJ)])
endif

MKDIR_WMNL = mkdir -p $(BIN)
ifdef TEST
MKDIR_WMNL += $(OBJ)
endif

# Path to mclmac.a
MAC_PATH = $(mac_current_dir)/lib
$(info $$MAC_PATH is [${MAC_PATH}])

# Libraries to include
ifdef TEST
LIBS += mclmacT
endif
LIBS += mclmac
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
$(BIN)/wmnlora : $(OBJ)/main_test.o
	$(CC) $< $(addprefix -L, $(LDFLAGS)) $(addprefix -l,$(LIBS)) -o $@

$(OBJ)/main_test.o : main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ)/main_test.o : $(INCT_DIR)/mclmac_tests.h
endif

# Remove object files and static libraries
clean:
	rm $(OBJ_DIR)/*.o
	rm $(OBJ)/*.o
	rm $(LIB_MAC)/*.a
	rm $(BIN)/*

endif # Linux compilation

ifdef RIOT
unexport RIOT

APPLICATION = main_test

# Default board
BOARD ?= native

RIOTBASE ?= /home/phantom/CP_Systems/RTOS/RIOT

DEVHELP ?= 1

# For using assertions
DEVELHELP ?= 1

QUIET ?= 1

USEMODULE += memarray

include WMNLoRa.include

# Which features do you require? (mcu architecure, peripherals, sensors, etc.)
#FEATURES_REQUIRED += arch_esp32
FEATURES_REQUIRED += periph_gpio

CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -D__RIOT__

DISABLE_MODULE += test_utils_interactive_sync

include $(RIOTBASE)/Makefile.include

endif
