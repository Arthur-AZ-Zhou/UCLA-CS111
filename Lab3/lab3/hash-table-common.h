#pragma once

#include <stdint.h>

#define HASH_TABLE_CAPACITY 4096

uint32_t bernstein_hash(const char *string);
