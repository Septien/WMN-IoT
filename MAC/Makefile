# This makefile is used by RIOT to compile the module. 
# Originally, it was called Makefile.riot to indicate the target
# OS, but it has to be changed simply to Makeifle, 
# so RIOT can recognize it.

CFLAGS += -Wno-unused-variable
ifdef TEST
DIRS += $(CURDIR)/srcs_test
endif

include $(RIOTBASE)/Makefile.base
