# List in Tamgu

A list is used to store objects of any type. It provides various methods for manipulating the list. Unlike a vector, a list allows elements to be added or removed from both the front and the back, enabling different management strategies such as FIFO, LILO, FILO, or LIFO.

## Methods

1. `clear()`: Clears the list.
2. `first()`: Returns the first element.
3. `flatten()`: Flattens a vector structure.
4. `insert(i, v)`: Inserts element `v` at position `i`.
5. `join(string sep)`: Produces a string representation of the list.
6. `json()`: Returns a JSON-compatible string representation of the list.
7. `last()`: Returns the last element.
8. `merge(v)`: Merges list `v` into the current list.
9. `pop(i)`: Removes the element at position `i` from the list.
10. `popfirst()`: Removes and returns the first element.
11. `poplast()`: Removes and returns the last element.
12. `product()`: Returns the product of all elements.
13. `push(v)`: Pushes a value `v` into the list.
14. `pushfirst(v)`: Pushes a value `v` into the list at the first position.
15. `remove(e)`: Removes element `e` from the list.
16. `reverse()`: Reverses the order of elements in the list.
17. `size()`: Returns the number of elements in the list.
18. `shuffle()`: Shuffles the values in the list.
19. `sum()`: Returns the sum of all elements.
20. `unique()`: Removes duplicate elements from the list.

## Initialization

A list can be initialized with a structure enclosed in square brackets `[]`.

```tamgu
list v = [1, 2, 3, 4, 5];
list vs = ["a", "b", "v"];
```

## Operators

- `x in vlist`: Returns true or a list of indexes, depending on the receiving variable. If the list contains strings, the system will only return true or its index if the value is an exact match.
- `for (s in vlist) { ... }`: Loops through all values in the list. At each iteration, `s` contains a value from `vlist`.
- `+`, `*`, `-`, `/`, etc.: Performs arithmetic operations on each element of a list or combines two lists.
- `&`, `|`: Computes the intersection or union of two lists.

## Conversion

- As an integer or a float: Returns the size of the list.
- As a string: Returns a structure where each element is separated by a comma, similar to the structure used for initializing a vector or a list.

## Indexing

Indexes can be used with list objects, similar to vectors. However, indexing with lists is less efficient and should be avoided.

## Example

```tamgu
list vlist = [1, 2, 3, 4, 5];
vlist.pushfirst(10);
vlist.pushlast(20); // [10, 1, 2, 3, 4, 5, 20]
```