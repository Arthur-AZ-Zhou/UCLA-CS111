#include "hash-table-base.h"
#include "hash-table-v1.h"
#include "hash-table-v2.h"

#include <argp.h>
#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

char *entries;

void (*add_entry)(void *, const char *key, uint32_t value);

#define BYTES_PER_STRING 8

struct arguments {
	uint32_t threads;
	uint32_t size;
};

static struct argp_option options[] = { 
	{ "threads", 't', "NUM", 0, "Number of threads."},
	{ "size", 's', "NUM", 0, "Size per thread."},
	{ 0 } 
};

static uint32_t parse_uint32_t(const char *string) {
	uint32_t current = 0;
	uint8_t i = 0;
	while (true) {
		char c = string[i];
		if (c == 0) {
			break;
		}

		/* Definitely greater than UINT32_MAX */
		if (i == 10) {
			exit(EINVAL);
		}

		/* Ensure the character is a digit */
		if (c < 0x30 || c > 0x39) {
			exit(EINVAL);
		}

		uint8_t digit = (c - 0x30);

		/* Check for overflows */
		if (i == 9) {
			if (current > 429496729) {
				exit(EINVAL);
			}
			else if (current == 429496729 && digit > 5) {
				exit(EINVAL);
			}
		}

		current = current * 10 + digit;

		++i;
	}
	return current;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;
	switch (key) {
	case 't':
		arguments->threads = parse_uint32_t(arg);
		break;
	case 's':
		arguments->size = parse_uint32_t(arg);
		break;
	}   
	return 0;
}

static struct arguments arguments;
static char *data;

static size_t get_global_index(uint32_t thread, uint32_t index)
{
	return thread * arguments.size + index;
}

static char *get_string(size_t global_index)
{
	return data + (global_index * BYTES_PER_STRING);
}

static unsigned long usec_diff(struct timeval *a, struct timeval *b)
{
	unsigned long usec;
	usec = (b->tv_sec - a->tv_sec)*1000000;
	usec += b->tv_usec - a->tv_usec;
	return usec;
}

static struct hash_table_v1 *hash_table_v1;

void *run_v1(void *arg) {
	uint32_t thread = (uintptr_t) arg;
	for (uint32_t j = 0; j < arguments.size; ++j) {
		size_t global_index = get_global_index(thread, j);
		char *string = get_string(global_index);
		hash_table_v1_add_entry(hash_table_v1, string, global_index);
	}
	return NULL;
}

static struct hash_table_v2 *hash_table_v2;

void *run_v2(void *arg) {
	uint32_t thread = (uintptr_t) arg;
	for (uint32_t j = 0; j < arguments.size; ++j) {
		size_t global_index = get_global_index(thread, j);
		char *string = get_string(global_index);
		hash_table_v2_add_entry(hash_table_v2, string, global_index);
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	arguments.threads = 4;
	arguments.size = 25000;
  
	static struct argp argp = { options, parse_opt };
	argp_parse(&argp, argc, argv, 0, 0, &arguments);

	setlocale(LC_ALL, "en_US.UTF-8");

	data = calloc(arguments.threads * arguments.size, BYTES_PER_STRING);

	struct timeval start, end;

	gettimeofday(&start, NULL);
	srand(42);
	for (uint32_t i = 0; i < arguments.threads; ++i) {
		for (uint32_t j = 0; j < arguments.size; ++j) {
			size_t global_index = get_global_index(i, j);
			char *string = get_string(global_index);
			for (uint32_t k = 0; k < (BYTES_PER_STRING - 1); ++k) {
				int r = rand() % 52;
				if (r < 26) {
					string[k] = r + 0x41;
				}
				else {
					string[k] = r + 0x47;
				}
			}
			string[BYTES_PER_STRING - 1] = 0;
		}
	}
	gettimeofday(&end, NULL);
	printf("Generation: %'lu usec\n", usec_diff(&start, &end));

	struct hash_table_base *hash_table_base = hash_table_base_create();
	gettimeofday(&start, NULL);
	for (uint32_t i = 0; i < arguments.threads; ++i) {
		for (uint32_t j = 0; j < arguments.size; ++j) {
			size_t global_index = get_global_index(i, j);
			char *string = get_string(global_index);
			hash_table_base_add_entry(hash_table_base, string, global_index);
		}
	}
	gettimeofday(&end, NULL);
	printf("Hash table base: %'lu usec\n", usec_diff(&start, &end));

	size_t missing = 0;
	for (uint32_t i = 0; i < arguments.threads; ++i) {
		for (uint32_t j = 0; j < arguments.size; ++j) {
			size_t global_index = get_global_index(i, j);
			char *string = get_string(global_index);
			if (!hash_table_base_contains(hash_table_base, string)) {
				++missing;
			}
		}
	}
	printf("  - %'lu missing\n", missing);
	hash_table_base_destroy(hash_table_base);

	pthread_t *threads = calloc(arguments.threads, sizeof(pthread_t));

	hash_table_v1 = hash_table_v1_create();
	gettimeofday(&start, NULL);
	for (uintptr_t i = 0; i < arguments.threads; ++i) {
		int err = pthread_create(&threads[i], NULL, run_v1, (void*) i);
		if (err != 0) {
			printf("pthread_create returned %d\n", err);
			return err;
		}
	}
	for (uintptr_t i = 0; i < arguments.threads; ++i) {
		int err = pthread_join(threads[i], NULL);
		if (err != 0) {
			printf("pthread_join returned %d\n", err);
			return err;
		}
	}
	gettimeofday(&end, NULL);
	printf("Hash table v1: %'lu usec\n", usec_diff(&start, &end));

	missing = 0;
	for (uint32_t i = 0; i < arguments.threads; ++i) {
		for (uint32_t j = 0; j < arguments.size; ++j) {
			size_t global_index = get_global_index(i, j);
			char *string = get_string(global_index);
			if (!hash_table_v1_contains(hash_table_v1, string)) {
				++missing;
			}
		}
	}
	printf("  - %'lu missing\n", missing);
	hash_table_v1_destroy(hash_table_v1);

	hash_table_v2 = hash_table_v2_create();
	gettimeofday(&start, NULL);
	for (uintptr_t i = 0; i < arguments.threads; ++i) {
		int err = pthread_create(&threads[i], NULL, run_v2, (void*) i);
		if (err != 0) {
			printf("pthread_create returned %d\n", err);
			return err;
		}
	}
	for (uintptr_t i = 0; i < arguments.threads; ++i) {
		int err = pthread_join(threads[i], NULL);
		if (err != 0) {
			printf("pthread_join returned %d\n", err);
			return err;
		}
	}
	gettimeofday(&end, NULL);
	printf("Hash table v2: %'lu usec\n", usec_diff(&start, &end));

	missing = 0;
	for (uint32_t i = 0; i < arguments.threads; ++i) {
		for (uint32_t j = 0; j < arguments.size; ++j) {
			size_t global_index = get_global_index(i, j);
			char *string = get_string(global_index);
			if (!hash_table_v2_contains(hash_table_v2, string)) {
				++missing;
			}
		}
	}
	printf("  - %'lu missing\n", missing);
	hash_table_v2_destroy(hash_table_v2);

	free(threads);
	free(data);

	return 0;
}
