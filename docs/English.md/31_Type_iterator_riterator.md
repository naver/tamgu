# Iterator and Riterator

These iterators are used to iterate over objects of various types such as string, vector, map, and rule. 

A riterator is a reverse iterator that allows iteration from the end of the collection.

## Methods

1. `begin()`: Initializes the iterator to the beginning of the collection.
2. `end()`: Returns true when the end of the collection is reached.
3. `key()`: Returns the key of the current element.
4. `next()`: Moves to the next element in the collection.
5. `value()`: Returns the value of the current element.

## Initialization

An iterator can be initialized by simple assignment.

## Example

```cpp
vector<int> v = {1, 2, 3, 4, 5};
iterator it = v;

for (it.begin(); it.end(); it.next()) {
    print(it.value(), ",");
}
```

## Output

```
1, 2, 3, 4, 5,
```