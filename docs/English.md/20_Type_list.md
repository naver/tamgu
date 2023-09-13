# Type list

A `list` is used to store objects of any type. It provides the following methods. It is different from a `vector` in that it allows elements to be added or removed from both the front and the back, enabling FIFO, LILO, FILO, or LIFO management of lists.

## Methods

1. `clear()`: Clears the container.
2. `first()`: Returns the first element.
3. `flatten()`: Flattens a vector structure.
4. `insert(i, v)`: Inserts `v` at position `i`.
5. `join(string sep)`: Produces a string representation of the container.
6. `json()`: Returns a JSON compatible string matching the container.
7. `last()`: Returns the last element.
8. `merge(v)`: Merges `v` into the list.
9. `pop(i)`: Erases an element from the list at position `i`.
10. `popfirst(i)`: Removes and returns the first element.
11. `poplast(i)`: Removes and returns the last element.
12. `product()`: Returns the product of elements.
13. `push(v)`: Pushes a value into the list.
14. `pushfirst(v)`: Pushes a value into the list in the first position.
15. `remove(e)`: Removes `e` from the vector.
16. `reverse()`: Reverses a vector.
17. `size()`: Returns the number of elements, the size of the container.
18. `shuffle()`: Shuffles the values in the list.
19. `sum()`: Returns the sum of elements.
20. `unique()`: Removes duplicate elements.

## Initialization

A list can be initialized with a structure between `[]`.

```python
list v = [1, 2, 3, 4, 5];
list vs = ["a", "b", "v"];
```

## Operators

- `x in vlist`: Returns true or a list of indexes, depending on the receiving variable. If the list contains strings, it will return true or its index only if the value is the same string as the one tested.
- `for (s in vlist) { ... }`: Loops through all values. At each iteration, `s` contains a value from `vlist`.
- `+`, `*`, `-`, `/`, etc.: Adds a value to each element of a list or adds each element of a list to another.
- `&`, `|`: Intersection or union of two lists.

## Size

- As an integer or a float: Returns the size of the list.
- As a string: Returns a structure where each element is separated from the others with a comma, similar to the structure used to initialize a vector or a list.

## Indexes

You can use indexes with list objects, similar to vectors. However, indexes with lists are rather inefficient and should be avoided.

## Example

```python
list vlist = [1, 2, 3, 4, 5];
vlist.pushfirst(10);
vlist.pushlast(20); // [10, 1, 2, 3, 4, 5, 20]
vlist.popfirst(); // [1, 2, 3, 4, 5, 20]
vector v = vlist; // Transform a list into a vector
```