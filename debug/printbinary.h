#ifndef BYTES_H
#define BYTES_H

#define BYTE_TO_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY_INT8(byte) \
    ((byte) & 0x80 ? '1' : '0'), \
    ((byte) & 0x40 ? '1' : '0'), \
    ((byte) & 0x20 ? '1' : '0'), \
    ((byte) & 0x10 ? '1' : '0'), \
    ((byte) & 0x08 ? '1' : '0'), \
    ((byte) & 0x04 ? '1' : '0'), \
    ((byte) & 0x02 ? '1' : '0'), \
    ((byte) & 0x01 ? '1' : '0')

#define BYTE_TO_BINARY_PATTERN_INT16 \
    BYTE_TO_BINARY_PATTERN_INT8     BYTE_TO_BINARY_PATTERN_INT8
#define BYTE_TO_BINARY_INT16(bytes) \
    BYTE_TO_BINARY_INT8((bytes) >> 8), BYTE_TO_BINARY_INT8(bytes)

#define BYTE_TO_BINARY_PATTERN_INT32 \
    BYTE_TO_BINARY_PATTERN_INT16     BYTE_TO_BINARY_PATTERN_INT16
#define BYTE_TO_BINARY_INT32(bytes) \
    BYTE_TO_BINARY_INT16((bytes) >> 16), BYTE_TO_BINARY_INT16(bytes)

#define BYTE_TO_BINARY_PATTERN_INT64 \
    BYTE_TO_BINARY_PATTERN_INT32     BYTE_TO_BINARY_PATTERN_INT32
#define BYTE_TO_BINARY_INT64(bytes) \
    BYTE_TO_BINARY_INT32((bytes) >> 32), BYTE_TO_BINARY_INT32(bytes)

#endif      // BYTES_H
