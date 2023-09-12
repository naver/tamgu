# Tamgu Vector and Table Containers

## Vector Containers
Tamgu provides specialized vector containers for different types of values. These containers are designed to efficiently store and manage specific types of values, reducing memory consumption. The following vector types are available:

- **bvector**: specialized for bytes
- **ivector**: specialized for integers
- **lvector**: specialized for longs
- **fvector**: specialized for floats
- **svector**: specialized for strings
- **uvector**: specialized for unicode strings

These containers can only store values of their specific type. They offer faster and more efficient access to the stored values compared to a generic vector. 

When you store a string in a vector, Tamgu needs to create a string object to store it within the vector, as vectors can only store objects. However, if you use an svector, the string is stored directly without creating a specific string object. This reduces storage to only strings and improves both access speed and memory usage.

You can use these vector structures in the same way as a generic vector. Here is an example of using an svector:

```cpp
svector test;
test.push("toto");
```

## Table Container
The **table** is a container that must be defined with a fixed size at creation. It expects integers as indexes. The table container is extremely fast as it is based on a C table implementation. However, its size is fixed at creation and cannot be changed dynamically. If the initial size is too small, you can use the `resize` function to enlarge or decrease the size.

Here is an example of creating and resizing a table:

```cpp
table test(10);
test[1] = "i";

println(test.size()); // Output: 10

test.resize(20);

println(test.size()); // Output: 20
```

Note: The `println` function is not part of Tamgu and is used here for demonstration purposes.