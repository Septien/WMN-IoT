CC=/usr/bin/gcc

SOURCES=test.c ControlPacket.c CFPacket.c DataPacket.c MAC_Internals.c
HEADERS=ControlPacket.h CFPacket.h DataPacket.h MAC_Internals.h
SOURCEST=tests/ControlPacket_Tests.c tests/CFPacket_Tests.c tests/DataPacket_Tests.c tests/MAC_Internals_Tests.c
HEADERST=tests/ControlPacket_Tests.h tests/CFPacket_Tests.h tests/DataPacket_Tests.h tests/MAC_Internals_Tests.h

CFLAGS=

DEBUGF=
ifdef DEBUG
	DEBUGF+=-g
endif

OS=
ifdef LINUX
	OS += -D__LINUX__
endif
ifdef RIOT
	OS += -D__RIOT__
endif

test: $(SOURCES) $(SOURCEST) $(HEADERS) $(HEADERST)
	$(CC) $(CFLAGS) $(OS) $(DEBUGF) $(SOURCES) $(SOURCEST) -o $@.out