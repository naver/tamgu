# Lock-Free Types in Tamgu

Lock-free types are extremely useful in multithreading applications as they allow manipulation of numbers, strings, vectors, or maps without any locks. Locks have a significant impact on execution performance. When a thread is put on hold due to a lock, the system must store its context and reactivate it when the lock is released. Lock-free programming aims to minimize the use of locks to avoid costly interruptions in the execution flow.

## Table and Rawstring

In previous sections, we have discussed two of these lock-free objects: table and rawstring. These objects have the specificity that their size is fixed and cannot be modified within the context of a thread. This guarantees that concurrent access to these objects will not interfere with their inner buffer declaration.

## Atomic Values: A_Bool, A_Int, A_Float, A_String, A_Ustring

Tamgu also provides five atomic values for Boolean, integers, floats, and strings. These objects can be used as regular values, but in the case of concurrent access, their atomicity is guaranteed. Atomicity, in this context, means that only one thread can modify their value at a time.

## Containers: A_Vector, A_[I|F|S|U]Vector, A_Map [I|F|S|U][I|F|S|U]

Tamgu also provides containers that can be modified by different threads simultaneously with minimal locks. These structures can be used exactly like their corresponding non-atomic maps. They are implemented as a linked list of chunks, where each chunk can typically accommodate a large number of values. A lock is only set when a new element is added to this linked list. Reading and most storage operations are processed without any locks, except in cases where the map or vector is not large enough to accommodate a new element.

**Note:** A_maps are all value containers, while A_vector is an object container. You need to load the "allmaps" library to access these maps.

## Ring

"Ring" is another lock-free container that can hold a maximum of 65535 elements. It is implemented as a structure where elements can be stored or removed from the front or back. Unlike other containers, these operations have the same memory time or space footprint.

## Important

When different threads attempt to modify the same lock-free variable, there is no guarantee that both modifications will be successful. The last thread to access the variable may be the one to modify its value.