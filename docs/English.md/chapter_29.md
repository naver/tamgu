# Tamgu Iterator

Tamgu provides iterators for iterating over objects of various types, including strings, vectors, maps, and rules. Additionally, it also provides a reverse iterator, called `riterator`, which allows iterating from the end of the collection.

## Methods

The following methods are available for iterators:

1. `begin()`: Initializes the iterator with the beginning of the collection.
2. `end()`: Returns `true` when the end of the collection is reached.
3. `key()`: Returns the key of the current element.
4. `next()`: Moves to the next element in the collection.
5. `value()`: Returns the value of the current element.

## Initialization

To initialize an iterator, simply assign it to the collection you want to iterate over.

## Example

Here's an example of iterating over a vector using an iterator:

```cpp
vector<int> v = {1, 2, 3, 4, 5};
iterator it = v;
for (it.begin(); it.end(); it.next()) {
    print(it.value(), ",");
}
```

Running this code will output:

```
1, 2, 3, 4, 5,
```