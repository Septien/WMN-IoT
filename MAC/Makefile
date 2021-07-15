CC=/usr/bin/gcc

SOURCES=test.c ControlPacket.c CFPacket.c DataPacket.c
HEADERS=ControlPacket.h CFPacket.h DataPacket.h
SOURCEST=tests/ControlPacket_Tests.c tests/CFPacket_Tests.c tests/DataPacket_Tests.c
HEADERST=tests/ControlPacket_Tests.h tests/CFPacket_Tests.h tests/DataPacket_Tests.h

CFLAGS=

DEBUGF=
ifdef DEBUG
	DEBUGF+=-g
endif

test: $(SOURCES) $(SOURCEST) $(HEADERS) $(HEADERST)
	$(CC) $(CFLAGS) $(DEBUGF) $(SOURCES) $(SOURCEST) -o $@.out