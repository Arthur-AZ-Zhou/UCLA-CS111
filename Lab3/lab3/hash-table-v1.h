#pragma once

#include "hash-table-common.h"

#include <stdbool.h>

struct hash_table_v1;
struct hash_table_v1 *hash_table_v1_create();
void hash_table_v1_add_entry(struct hash_table_v1 *hash_table,
                             const char *key,
                             uint32_t value);
bool hash_table_v1_contains(struct hash_table_v1 *hash_table,
                            const char *key);
uint32_t hash_table_v1_get_value(struct hash_table_v1 *hash_table,
                                 const char* key);
void hash_table_v1_destroy(struct hash_table_v1 *hash_table);
