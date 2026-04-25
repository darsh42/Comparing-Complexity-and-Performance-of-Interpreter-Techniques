#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Standard CRC-32 lookup table
uint32_t crc32_table[256];

void init_crc32_table() {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t ch = i;
        for (size_t j = 0; j < 8; j++) {
            ch = (ch >> 1) ^ (0xEDB88320 & (-(ch & 1)));
        }
        crc32_table[i] = ch;
    }
}

uint32_t crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

int main() {
    init_crc32_table();

    // Allocate 1MB of data
    size_t size = 1024 * 1024;
    uint8_t *buffer = malloc(size);
    for (size_t i = 0; i < size; i++) {
        buffer[i] = (uint8_t)(i & 0xFF);
    }

    printf("Calculating CRC-32 over 1MB buffer 500 times...\n");

    uint32_t result = 0;
    for (int r = 0; r < 500; r++) {
        result = crc32(buffer, size);
    }

    printf("Final CRC32: 0x%08X\n", result);
    free(buffer);
    return 0;
}
