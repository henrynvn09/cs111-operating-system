# Hash Hash Hash

Making hash table implementation to use concurrently by adding mutex to the code. Then compare performance

## Building

```shell
make
```

## Running

```shell
> ./hash-table-tester -t 8 -s 50000

Hash table base: 1,581,974 usec
- 0 missing
Hash table v1: 359,149 usec
- 28 missing
Hash table v2: 396,051 usec
- 24 missing
```

## First Implementation

In the `hash_table_v1_add_entry` function, I added a mutex in the struct hash_table_v1 for the whole table. Then

- Initialize the mutex on table creation hash_table_v1_create
- add mutex lock and unlock on entry access and creation
- add mutex lock/unlock when destroying the list hash_table_v1_destroy. Next, destroy the mutex before freeing the table

### Performance

```shell
> ./hash-table-tester -t 8 -s 50000
Generation: 67,008 usec
Hash table base: 1,003,971 usec
  - 0 missing
Hash table v1: 1,329,409 usec
  - 0 missing
Hash table v2: 341,578 usec
  - 4 missing
```

Version 1 is a little slower than the base version. As it basically base version with extra overhead of the mutex.

## Second Implementation

In the `hash_table_v2_add_entry` function, I add a mutex for every entry of the hash table. Then:

- Initialize the mutex on each entry creation
- lock the list on adding a new entry
- destroy every mutex on entry destroying

### Performance

```shell
> ./hash-table-tester -t 8 -s 50000
Generation: 82,403 usec
Hash table base: 1,031,292 usec
  - 0 missing
Hash table v1: 1,424,154 usec
  - 0 missing
Hash table v2: 383,234 usec
  - 0 missing
```

```bash
> ./hash-table-tester -t 16 -s 50000

Generation: 138,743 usec
Hash table base: 6,545,790 usec
  - 0 missing
Hash table v1: 8,308,370 usec
  - 0 missing
Hash table v2: 1,959,926 usec
  - 0 missing

> ./hash-table-tester -t 10 -s 90000

Generation: 160,116 usec
Hash table base: 8,879,179 usec
  - 0 missing
Hash table v1: 11,057,072 usec
  - 0 missing
Hash table v2: 2,768,276 usec
  - 0 missing
```

Version 2 is a faster than base and v1 since it parallelize most part of the code: checking contain, get, initialization and table entry destroy. It only uses locks in adding list entry, which creates a little overhead, thus the code is way faster.

## Cleaning up

```shell
make clean
```
