# Hash Hash Hash

- Implement separate chaining hash table using mutex locks for critical sections.

## Building

```shell
make
```

## Running

```shell
./hash-table-tester -t [THREAD_NUM] -s [ELEMENT_NUM]
```

## First Implementation

If I can add only one lock, then I can lock the whole hash table as one. So I added `pthread_mutex_t lock` in `hash_table_v1`, and it is initialized once in `hash_table_v1_create` function.

In the`hash_table_v1_add_entry` function, I added `pthread_mutex_lock`in the beginning of the function and`pthread_mutex_unlock` at the end of the function to lock the whole insertion process.

### Performance

```shell
./hash-table-tester -t 4 -s 150000
```

Result:

```
Generation: 45,799 usec
Hash table base: 86,676 usec
  - 0 missing
Hash table v1: 245,348 usec
  - 0 missing
```

Version 1 is almost 2-3 times slower than the base version. Since the mutex lock the whole hash table, once one thread enter the critical section, all other threads cannot execute the `hash_table_v1_add_entry` function and have to wait until it is released.

## Second Implementation

If I can add multiple locks, then having a lock in each entriy can make better performance when there is multiple threads. So I added `pthread_mutex_t lock` in `hash_table_entry`, and it is initialized for HASH_TABLE_CAPACITY times in `hash_table_v1_create` function.

In the `hash_table_v2_add_entry` function,

- Added `pthread_mutex_lock` before `get_list_entry` function because this function iterates through the list and check if it already has the key.<br/>
  Let's say there is an existing list entry with value 0 and the process tried to change the entry value to 1 and then change it to 2. If the function returns the list entry pointer and then the context switch occur before it assigns 1 to it, then in the another thread the list entry value is still 0 and it assigns 2. The context switch occur again and the first thread will assign 1. In this case, the final value should be 2 but it will be 1.
- Added `pthread_mutex_unlock` after the value assignment when `list_entry` is not NULL because the lock has to be unlocked before it returns.
- Added `pthread_mutex_lock` after `SLIST_INSERT_HEAD` because inserting head is not a single operation. First, link the newly created node to the node that the head is pointing to. And then move the head pointer to the new node. If we don't lock this section, then multiple `list_entry` node from different threads can be be linked to the node that the current head is pointing to.

### Performance

```shell
./hash-table-tester -t 4 -s 50000
```

Result:

```
Generation: 31,596 usec
Hash table base: 77,674 usec
  - 0 missing
Hash table v1: 235,699 usec
  - 0 missing
Hash table v2: 20,103 usec
  - 0 missing
```

```shell
./hash-table-tester -t 4 -s 150000
```

Result:

```
Generation: 70,465 usec
Hash table base: 1,563,859 usec
  - 0 missing
Hash table v1: 2,502,942 usec
  - 0 missing
Hash table v2: 378,260 usec
  - 0 missing
```

```shell
./hash-table-tester -t 2 -s 150000
```

Result:

```
Generation: 38,302 usec
Hash table base: 172,735 usec
  - 0 missing
Hash table v1: 243,113 usec
  - 0 missing
Hash table v2: 98,287 usec
  - 0 missing
```

```shell
./hash-table-tester -t 4 -s 250000
```

Result:

```
Generation: 127,654 usec
Hash table base: 6,582,630 usec
  - 0 missing
Hash table v1: 7,699,178 usec
  - 0 missing
Hash table v2: 1,697,873 usec
  - 0 missing
```

```shell
./hash-table-tester -t 8 -s 50000
```

Result:

```
Generation: 65,185 usec
Hash table base: 346,739 usec
  - 0 missing
Hash table v1: 692,944 usec
  - 0 missing
Hash table v2: 103,840 usec
  - 0 missing
```

Version 2 is faster than the base version. Since I added lock in each entry, multiple threads can do insertion on different entries. I observed that with more threads and more elements, the performance efficiency increases compared to the base hash table. However, the performance does not significantly improve when the number of threads exceeds 4. Once the number of threads surpasses the number of available CPU cores, adding more threads actually decreases performance due to the overhead of context switching between threads.

## Cleaning up

```shell
make clean
```
