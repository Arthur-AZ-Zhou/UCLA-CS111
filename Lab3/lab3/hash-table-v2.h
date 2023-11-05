#pragma once

#include "hash-table-common.h"

#include <stdbool.h>

struct hash_table_v2;
struct hash_table_v2 *hash_table_v2_create();
void hash_table_v2_add_entry(struct hash_table_v2 *hash_table,
                             const char *key,
                             uint32_t value);
bool hash_table_v2_contains(struct hash_table_v2 *hash_table,
                            const char *key);
uint32_t hash_table_v2_get_value(struct hash_table_v2 *hash_table,
                                 const char* key);
void hash_table_v2_destroy(struct hash_table_v2 *hash_table);
