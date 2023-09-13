# Lock-Free Types

Lock-free types are extremely useful in multithreading applications as they allow manipulation of numbers, strings, vectors, or maps without the need for locks. Locks can significantly impact execution performance. When a thread is put on hold due to a lock, the system must store its context and reactivate it once the lock is released. Lock-free programming aims to minimize the use of locks to avoid costly interruptions in the execution flow.

## Table and Rawstring

In previous sections, we have discussed two lock-free objects: table and rawstring.

These objects have the unique characteristic that their size is fixed and cannot be modified within the context of a thread. This guarantees that concurrent access to these objects will not interfere with their inner buffer declaration.

## Atomic Values: a_bool, a_int, a_float, a_string, a_ustring

Tamgu also provides five atomic values for Booleans, integers, floats, and strings. These objects can be used as regular values, but in the case of concurrent access, their atomicity is guaranteed. Atomicity, in this context, means that only one thread can modify their value at a time.

## Containers: a_vector, a_[i|f|s|u]vector, a_map [i|f|s|u][i|f|s|u]

Tamgu also offers containers that can be modified by different threads simultaneously with minimal use of locks. These structures can be used just like their corresponding non-atomic maps. They are implemented as a linked list of chunks, with each chunk capable of accommodating a significant number of values. Locks are only used when a new element is added to the linked list. Reading and most storage operations are performed without any locks, except when the map or vector is not large enough to accommodate a new element.

Note: a_maps are value containers, while a_vector is an object container.

Note: You need to load the "allmaps" library to access these maps.

## Ring

"Ring" is another lock-free container that can hold a maximum of 65535 elements. It is implemented as a structure where elements can be stored or removed from the front or back. Unlike other containers, these operations have the same memory time or space footprint.

## Important

When multiple threads attempt to modify the same lock-free variable, there is no guarantee that both modifications will be successful. The last thread to access the variable may be the one to modify its value.