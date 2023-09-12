# Self-included containers in Tamgu

In Tamgu, there are two types of containers: value containers and object containers.

## Value Containers
Value containers include `svector`, `ivector`, and `mapii`. These containers store values.

## Object Containers
Object containers include `vector`, `map`, `treemap`, `table`, and `list`. These containers store objects.

When including a container within itself, the system cannot display the entire structure. Instead, it replaces the self-reference with "[...]" for value containers or "{...}" for map containers.

For example:
```python
vector v = [1..5]
v.push(v)
println(v)
```
Output: `v is: [1,2,3,4,5,[...]]`, where `[...]` represents a self-reference.

However, if a container contains itself, there may be a problem when traversing the structure as it can lead to an infinite loop.

To address this issue, Tamgu provides three methods:

1. `mark(byte v)`: This method marks an object container.
2. `mark()`: This method returns the mark on an object container.
3. `unmark()`: This method removes all marks within an object container.

For example:
```python
vector v = [10..50:10]
v.push([60..80:10])
```

Please refer to the official Tamgu documentation for more information on using containers in Tamgu.