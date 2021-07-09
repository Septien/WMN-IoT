CC=/usr/bin/gcc

SOURCES=test.c controlPacket.c CFPacket.c
HEADERS=controlPacket.h CFPacket.h 
SOURCEST=tests/controlPacket_Tests.c tests/CFPacket_Tests.c
HEADERST=tests/controlPacket_Tests.h tests/CFPacket_Tests.h

CFLAGS=

DEBUGF=
ifdef DEBUG
	DEBUGF+=-g
endif

test: $(SOURCES) $(SOURCEST) $(HEADERS) $(HEADERST)
	$(CC) $(CFLAGS) $(DEBUGF) $(SOURCES) $(SOURCEST) -o $@.out