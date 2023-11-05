#pragma once

#include "hash-table-common.h"

#include <stdbool.h>

struct hash_table_base;
struct hash_table_base *hash_table_base_create();
void hash_table_base_add_entry(struct hash_table_base *hash_table,
                               const char *key,
                               uint32_t value);
bool hash_table_base_contains(struct hash_table_base *hash_table,
                              const char *key);
uint32_t hash_table_base_get_value(struct hash_table_base *hash_table,
                                   const char* key);
void hash_table_base_destroy(struct hash_table_base *hash_table);
