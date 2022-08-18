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
CFLAGS += -I$(WMN_IOT_INCLUDE)
CPPFLAGS := -D__LINUX__

# Compile each of the submodules
include ipc-queues/Makefile.linux
include utils/Makefile.linux
include MAC/Makefile.linux
ifdef TEST
include tests/Makefile.linux
endif

ifdef TEST
SRC = main_test.c

OBJ = obj
$(info $$OBJ is [$(OBJ)])
endif

MKDIR_WMNL = mkdir -p $(BIN) $(LIB_DIR)
ifdef TEST
MKDIR_WMNL += $(OBJ)
endif

# Path to libipcqueues.a
IPC_QUEUES_PATH = $(ipc_curr_dir)/lib
# Path to libutils.a
UTILS_PATH = $(utils_current_dir)/lib
# Path to libmclmac.a
MAC_PATH = $(mac_current_dir)/lib
$(info $$MAC_PATH is [${MAC_PATH}])

# Libraries to include
#ipc-queues
LIBS += ipcqueues

# utils
ifdef TEST
LIBS += utilsT
endif
LIBS += utils

# MAC (optionally mactests)
ifdef TEST
LIBS += mclmacT
endif
LIBS += mclmac
$(info $$LIBS is [${LIBS}])

# Paths to static libraries
LDFLAGS += $(LIB_DIR)
LDFLAGS += $(UTILS_PATH)
LDFLAGS += $(MAC_PATH)
$(info $$LDFLAGS is [${LDFLAGS}])

TARGETS = directories_main
ifdef TEST
TARGETS += $(BIN)/wmn-iot
endif
$(info $$TARGETS is [${TARGETS}])

all: $(TARGETS)

directories_main:
	$(MKDIR_WMNL)

ifdef TEST
$(BIN)/wmn-iot : $(OBJ)/main_test.o
	$(CC) $< -o $@ $(addprefix -L, $(LDFLAGS)) $(addprefix -l,$(LIBS)) -lrt -lpthread

$(OBJ)/main_test.o : main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ)/main_test.o : $(INCT_DIR)/mclmac_tests.h $(CUNIT_INCLUDE)/cUnit.h
endif

# Remove object files and static libraries
clean:
# For ipc_queues
	rm $(LIB_IPC_QUEUES)/*.a
	rm $(OBJQ_DIR)/*.o
# For utils
	rm $(OBJU_DIR)/*.o
	rm $(LIB_UTILS)/*.a
# For MAC
	rm $(LIB_MAC)/*.a
	rm $(OBJ_DIR)/*.o
# For main
	rm $(OBJ)/*.o
	rm $(BIN)/*

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

include WMN-IoT.include

CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unused-parameter
CFLAGS += -D__RIOT__
ifeq ($(BOARD), native)
CFLAGS += -DNATIVE
endif

include $(RIOTBASE)/Makefile.include

endif	# RIOT compilation
