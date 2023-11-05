#include "hash-table-common.h"

#include <stdbool.h>
#include <stddef.h>

uint32_t bernstein_hash(const char *string)
{
	uint32_t hash = 0;
	size_t i = 0;
	while (true) {
		char c = string[i];
		if (c == 0) {
			break;
		}
		hash = (33 * hash) + c;
		++i;
	}
	return hash;
}
