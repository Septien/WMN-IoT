# Linux compilation
ifdef LINUX
unexport LINUX	# No longer needed, remove so it does not causes problems

CC = /usr/bin/gcc

BIN = bin/linux-x86_64
LIB_DIR = $(BIN)/lib
$(info $$BIN is [${BIN}])

# Create a static library
AR = ar csrv
RL = ranlib

WMN_IOT_INCLUDE = /home/phantom/CP_Systems/Implementations/WMN-IoT/include
GLOBAL_DEP = $(WMN_IOT_INCLUDE)/config.h

CFLAGS += -fpic -fstack-protector-strong -Werror -Wall -Wextra -pedantic -g3 -Og -std=gnu11 -fstack-protector-all -ffunction-sections -fwrapv -Wstrict-overflow -fno-common -fdata-sections -Wmissing-include-dirs -fno-delete-null-pointer-checks -fdiagnostics-color -Wstrict-prototypes -Wold-style-definition -gz -Wformat=2 -Wformat-overflow -Wformat-truncation
CFLAGS += -I$(WMN_IOT_INCLUDE) -I$(WMN_IOT_INCLUDE)/../debug/
CPPFLAGS := -D__LINUX__

# Compile each of the submodules
include ipc-queues/Makefile.linux
include utils/Makefile.linux
include MAC/Makefile.linux
include Network/Makefile.linux

ifdef TEST
include tests/Makefile.linux
# Add headers for unit testing
TESTS_INCLUDE := $(CUNIT_INCLUDE)/cUnit.h $(WMN_IOT_INCLUDE)/../debug/printbinary.h #$(INCT_DIR)/mclmac_tests.h
endif

ifdef BDD
CFLAGS += -DBDD
# Add headers for behavior testing
TESTS_INCLUDE := /home/phantom/CP_Systems/External_Modules/bdd-for-c/bdd-for-c.h
endif

# Path to libipcqueues.a
IPC_QUEUES_PATH = $(ipc_curr_dir)/lib
# Path to libutils.a
UTILS_PATH = $(utils_current_dir)/lib
# Path to libmclmac.a
MAC_PATH = $(mac_current_dir)/lib
$(info $$MAC_PATH is [${MAC_PATH}])

# Libraries to include
LIBS += ipcqueues
LIBS += utils
LIBS += mclmac
LIBS += rema
$(info $$LIBS is [${LIBS}])

# Paths to static libraries
LDFLAGS += $(LIB_DIR)
LDFLAGS += $(UTILS_PATH)
LDFLAGS += $(MAC_PATH)
$(info $$LDFLAGS is [${LDFLAGS}])

TARGETS = directories_main

ifeq ($(or $(TEST), $(BDD)), 1)
MKDIR_WMNL := mkdir -p $(BIN) $(LIB_DIR)
OBJ = $(BIN)/main
MKDIR_WMNL += $(OBJ)

TARGETS += $(BIN)/wmn-iot
endif
$(info $$TARGETS is [${TARGETS}])

all: $(TARGETS)

directories_main:
	$(MKDIR_WMNL)

# Comparing multiple variables: 
#https://stackoverflow.com/questions/5584872/complex-conditions-check-in-makefile
ifeq ($(or $(TEST), $(BDD)), 1)
$(BIN)/wmn-iot : $(OBJ)/main_test.o
	$(CC) $< -o $@ $(addprefix -L, $(LDFLAGS)) $(addprefix -l,$(LIBS)) -lrt -lpthread

$(OBJ)/main_test.o : main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ)/main_test.o : $(TESTS_INCLUDE)
endif

# Remove object files and static libraries
clean:
	rm -r $(BIN)/

endif # Linux compilation

ifdef RIOT
unexport RIOT

APPLICATION = stack_test

# Default board
BOARD ?= native

RIOTBASE ?= /home/phantom/CP_Systems/RTOS/RIOT

DEVHELP ?= 1

# For using assertions
DEVELHELP ?= 1

QUIET ?= 1

INCLUDES += -I$(CURDIR)/include

USEMODULE += memarray
USEMODULE += netdev
USEMODULE += nrf24l01p_ng
USEMODULE += nrf24l01p_ng_diagnostics

# Features (mcu architecure, peripherals, sensors, etc.)
ifeq ($(BOARD), esp32-wroom-32)
FEATURES_REQUIRED += arch_esp32
endif
FEATURES_REQUIRED += periph_gpio
FEATURES_REQUIRED += periph_spi

ifdef BDD
CFLAGS += -DBDD
endif

include WMN-IoT.include

CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -D__RIOT__
ifeq ($(BOARD), native)
CFLAGS += -DNATIVE
endif

include $(RIOTBASE)/Makefile.include

endif	# RIOT compilation
