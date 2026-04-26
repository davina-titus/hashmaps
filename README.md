# HashMap

A templated chaining hash table implemented in C++ from scratch.

## Overview

`HashMap<KeyT, ValT>` is a generic hash map using **separate chaining** (singly linked lists per bucket) to handle collisions. It supports the standard map operations and automatic resizing, with full memory safety verified by AddressSanitizer.

## Features

- Templated key and value types
- Separate chaining with singly linked lists
- Automatic resizing — doubles capacity when load factor exceeds 1.5, rehashing all existing nodes without allocating new ones
- Full iteration via `begin()` / `next()`
- Deep copy via copy constructor and assignment operator with self-assignment protection
- No memory leaks — verified with AddressSanitizer + LeakSanitizer



## Implementation Details

- **Bucket array**: `ChainNode** data` — a heap-allocated array of pointers, each pointing to the head of a linked list
- **Hash function**: `hash<KeyT>{}(key) % capacity` via the standard library
- **Load factor check**: uses `double` division to avoid integer truncation
- **Resize**: rethreads existing nodes into a new bucket array — no new node allocations
- **Copy**: deep copies all nodes bucket by bucket, preserving chain order

## Building & Testing

```bash
make test_hashmap_core       # run core tests
make test_hashmap_aug        # run augmented tests
make test_hashmap_all        # run all tests
make run_main                # run main for ad-hoc testing
```

Tests use GoogleTest and run with `-fsanitize=address,undefined` automatically.
