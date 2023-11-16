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
In the `hash_table_v1_add_entry` function, I added "pthread_mutex_t hash_mutex_v1" the hashtable struct as we are only allotted one lock and I initiated it at hash_table_v1_create() function. I would then clean it up in hash_table_v1_destroy(). I then put an unlock at the end of the function AND an unlock call in the case of an existing list entry so we don't have an unlockable hashtable entry. This netted us 0 missing elements but a runtime almost 50% slower compared to the base case. Additionally I had error statements in case if any of the lock actions failed everytime I used a pthread function.

### Performance
```shell
./hash-table-tester -t 8 -s 50000 runs a test. We can see it test the base case, v1, and v2. We get results:
Generation: 64,787 usec

Hash table base: 1,816,442 usec

  - 0 missing

Hash table v1: 2,431,287 usec

  - 0 missing

Hash table v2: 989,943 usec

  - 0 missing
```
Version 1 is a little slower than the base version. As it is due to the overhead between threads. This is because of scenarios such as if the scheduler tries to perform a context switch, if the selected thread tries to execute the locked region of code it'll be forced to go to sleep adding some overhead. The context switching will keep occuring but no thread will be able to execute the locked region of code until the lock is released. While this does ensure that one thread can successfully execute on the region of code it needs to execute in it adds a bit of overhead. Additionally, version 1 has the lock above the getters in the code. This is unnecessary as the getters do not actually need to be locked for the code to function correctly as if a thread is modifying the shared access code it'll be locked, so it's unnecessary code that's being locked when in reality other threads can be getting the shared memory at the same time, thus making version 1 slower.

## Second Implementation
In the `hash_table_v2_add_entry` function, I simply had each hash table entry possess a lock of its own. I put the lock in the hash_table_entry struct and initiated it in the hash_table_v2_create() function. I then only locked and unlocked the insertion part of the code. This netted us a result that was quite fast and met all the performance requirements. It was quite fast as we eliminated unnecessary overhead by not locking the getting part of hash_table_v2_add_entry. For every pthread action along the way, I had error checking and exits in case if any of the actions ever went wrong. To clean up, in the for loop iteration in the hash_table_v2_destroy() function, I used pthread_mutex_destroy() to destroy all the locks in the entries.

### Performance
```shell
./hash-table-tester -t 8 -s 50000 runs a test. We can see it test the base case, v1, and v2. We get results:
Generation: 66,616 usec

Hash table base: 1,840,136 usec

  - 0 missing

Hash table v1: 2,270,919 usec

  - 0 missing

Hash table v2: 736,001 usec

  - 0 missing

```

We see that version 2 is almost twice as fast as the initial version, and this is due to the eliminated overhead. We can try to change the variables around to look at other test cases, each within the performance requirements:
----------------------------------------------------------------
./hash-table-tester 
Generation: 12,471 usec

Hash table base: 62,972 usec

  - 0 missing

Hash table v1: 51,521 usec

  - 0 missing

Hash table v2: 33,149 usec

  - 0 missing
----------------------------------------------------------------
./hash-table-tester -t 12 -s 100000  
Generation: 109,975 usec

Hash table base: 22,151,859 usec

  - 0 missing

Hash table v1: 26,088,028 usec

  - 0 missing

Hash table v2: 11,747,749 usec

  - 0 missing
----------------------------------------------------------------
./hash-table-tester -t 6 -s 20000
Generation: 11,716 usec

Hash table base: 57,921 usec

  - 0 missing

Hash table v1: 88,633 usec

  - 0 missing

Hash table v2: 26,330 usec

  - 0 missing
----------------------------------------------------------------
./hash-table-tester -t 6 -s 40000
Generation: 11,716 usec

Hash table base: 57,921 usec

  - 0 missing

Hash table v1: 88,633 usec

  - 0 missing

Hash table v2: 26,330 usec

  - 0 missing
----------------------------------------------------------------

## Cleaning up
```shell
make clean cleans up the shell as it removes all executable file such as hash-table-common.o, hash-table-base.o, hash-table-v1.o, hash-table-v2.o, hash-table-tester.o, and hash-table-tester. We make again to create these executables.
```