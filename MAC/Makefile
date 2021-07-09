CC=/usr/bin/gcc

SOURCES=test.c controlPacket.c tests/controlPacket_Tests.c
HEADERS=controlPacket.h tests/controlPacket_Tests.h

CFLAGS=

DEBUGF=
ifdef DEBUG
	DEBUGF+=-g
endif

test: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(DEBUGF) $(SOURCES) -o $@.out