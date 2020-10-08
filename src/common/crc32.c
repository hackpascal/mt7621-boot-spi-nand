// SPDX-License-Identifier: BSD-3-Clause

#include <types.h>
#include <malloc.h>

static uint32_t *crc_table;

#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf) DO1(buf); DO1(buf);
#define DO4(buf) DO2(buf); DO2(buf);
#define DO8(buf) DO4(buf); DO4(buf);

uint32_t crc32(uint32_t crc, const void *buf, size_t len)
{
	const uint8_t *ptr = (const uint8_t *) buf;

	crc = crc ^ 0xffffffff;

	while (len >= 8) {
		DO8(ptr);
		len -= 8;
	}

	while (len) {
		DO1(ptr);
		len--;
	}

	return crc ^ 0xffffffff;
}

void make_crc32_table(void)
{
	uint32_t c, poly = 0;
	int n, k;

	static const uint8_t p[] = {0, 1, 2, 4, 5, 7, 8, 10, 11, 12, 16, 22, 23, 26};

	crc_table = malloc(256 * sizeof(*crc_table));

	/* make exclusive-or pattern from polynomial (0xedb88320) */
	for (n = 0; n < sizeof(p) / sizeof(uint8_t); n++)
		poly |= 1 << (31 - p[n]);

	for (n = 0; n < 256; n++) {
		c = (uint8_t)n;
		for (k = 0; k < 8; k++)
			c = c & 1 ? poly ^ (c >> 1) : c >> 1;
		crc_table[n] = c;
	}
}
