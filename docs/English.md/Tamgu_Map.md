# Tamgu Map

Tamgu Map is a hash table that uses any string or element that can be analyzed as a string as a key. The map converts all keys into strings, meaning that "123" and 123 are considered as the same key.

## Types of Maps

1. **TreeMap**: Similar to a map, but with the difference that keys in a TreeMap are automatically sorted.
2. **BinMap**: Similar to a map, but with short keys whose values are between 0 and 65535. Keys are always sorted. BinMap provides the fastest way to access elements. Note that BinMap is available through the "allmaps" library.
3. **PrimeMap**: A hash-map where keys are organized along prime numbers. The advantage of this map is that you can iterate along the order in which the values were stored.

## Methods

1. `clear()`: Clears the container.
2. `find(value)`: Tests if a value belongs to the map and returns 'true' or the corresponding keys.
3. `invert()`: Returns a map with key/value inverted.
4. `items()`: Returns a vector of {key:value} pairs.
5. `join(string sepkey, string sepvalue)`: Produces a string representation for the container.
6. `json()`: Returns a JSON compatible string matching the container.
7. `keys()`: Returns the map container keys as a vector.
8. `merge(v)`: Merges v into the vector.
9. `pop(key)`: Erases an element from the map.
10. `product()`: Returns the product of elements.
11. `size()`: Returns the number of elements, i.e., the size of the container.
12. `sum()`: Returns the sum of elements.
13. `test(key)`: Tests if key belongs to the map container.
14. `values()`: Returns the map container values as a vector.

## Initialization

A map can be initialized with a description in the format: `{ "k1":v1, "k2":v2, ... }`

Example:
```cpp
map toto = { "a":1, "b":2 };
```

## Operators

- `x in amap`: Returns true or a list of indexes, depending on the receiving variable. If the map contains string values, the system will return true or its index only if a value is the same string as the one tested. The `in` operator is not performed within the local strings.
- `for (s in amap) { ... }`: Loops among all keys. At each iteration, `s` contains a key from `amap`.
- `+`, `*`, `-`, `/`, etc.: Adds a value to each element of a map or adds each element of a map to another along keys.
- `&`, `|`: Intersection or union of two maps along keys.

## Indexes

- `map[key]`: Returns the element whose key is `key`. If `key` is not a key in the map, then `null` is returned.

## Size

- As an integer or a float: Returns the size of the map.
- As a string: Returns a string that mimics the map initialization structure.

## Example

```cpp
map vmap;
vmap["toto"] = 1;
vmap[10] = 27;
print(vmap); // displays: {'10':27, 'toto':1}
```

## Testing Keys

There are different ways to test whether a map contains a specific key. The first way is to use the `test` operator, which will return true or false. The other way is to catch the error when an incorrect index is provided with the container. However, it is faster and more efficient to use `test` instead of checking for equality.

Example:
```cpp
if (m.test("ee"))
    println("ee is not a key in m");

// To avoid an exception whenever a wrong key is used, use the following code:
erroronkey(false);
if (m["ee"] == empty)
    println("ee is not a key in m");
```