# Hash Hash Hash
We are to make a hash-table implementation that is safe to use concurrently through adding lock mutexes. In part v1 we are only allotted 1 while in part 2 we are allotted as many as we want. Given the skeleton code we are expected to make v1 and v2 contain no missing elements and v1 has to be slower than the base implementation while v2 has to be faster. We have to make use of the pthread.h library to initialize, lock, unlock, and clean up the locks.

## Building
```shell
make to utilize the makefile.
```

## Running
```shell
./hash-table-tester -t 8 -s 50000 to run a test of the hashtable. -t changes the number of threads (default 4) and -s changes the number of hash entries (default 25000)
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added "static pthread_mutex_t hash_mutex_v1;" to the line after list_entry struct because we are only allotted one lock and 

### Performance
```shell
TODO how to run and results
```
Version 1 is a little slower/faster than the base version. As TODO

## Second Implementation
In the `hash_table_v2_add_entry` function, I TODO

### Performance
```shell
TODO how to run and results
```

TODO more results, speedup measurement, and analysis on v2

## Cleaning up
```shell
TODO how to clean
```