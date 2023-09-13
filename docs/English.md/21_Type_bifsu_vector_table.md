## Tamgu Vector and Table Containers

### Vector Containers
Tamgu provides specialized vector containers for different types of data. These containers are efficient in terms of memory consumption and provide fast access to elements. The following are the available vector types:

- **bvector**: A vector container for bytes.
- **ivector**: A vector container for integers.
- **lvector**: A vector container for longs.
- **fvector**: A vector container for floats.
- **svector**: A vector container for strings.
- **uvector**: A vector container for unicode strings.

These containers can only store values of their specific type. When you store a string in a vector, Tamgu needs to create a string object, which will be stored within the vector since a vector can only store objects. However, in the case of an svector, the string is stored directly without the need for a specific string object. This reduces the storage to only strings and improves access speed.

To use these structures, you can treat them exactly like a regular vector. For example:

```cpp
svector test;
test.push("toto");
```

### Table Container
The "table" is another container provided by Tamgu. It is a fixed-size container that expects integers as indexes. The size of the table must be defined at creation and cannot be changed later. The table container is extremely fast as it is based on a C table implementation.

To create a table with a specific size and access its elements, you can do the following:

```cpp
table test(10);
test[1] = "i";
```

If the initial size of the table is too small, you can use the "resize" method to enlarge or decrease the size. This method will modify the current size of the table and copy all previous elements to their new positions. However, if you decrease the size of the table, elements beyond the new limit will be lost.

```cpp
table test(10);
println(test.size()); // Output: 10
test.resize(20);
println(test.size()); // Output: 20
```

It is important to note that the table container is not protected for simultaneous read/write operations in threads. If you can ensure that no simultaneous read/write will occur on the same elements, then this structure can be very efficient to use as it reduces the number of internal locks. However, if you predict potential collisions, it is safer to use locks to avoid crashes.

Additionally, Tamgu prevents resizing within a thread as concurrent access to elements might be disrupted.