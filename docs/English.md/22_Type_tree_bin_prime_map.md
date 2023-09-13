# Type (Tree, Binary, Prime) Map

A map is a hash table that uses any string or any element that can be analyzed as a string as a key. The map in Tamgu converts any keys into a string, which means that "123" and 123 are considered as one unique key.

**Note:**
- A treemap is similar to a map, with the difference that keys in a treemap are automatically sorted out.
- A binmap is also similar to a map, but the keys are short and their values are between 0 and 65535. Keys are always sorted out. Binmap is also the fastest way to access elements.
**Note:** binmap is available through the "allmaps" library (see specialized map below).
- A primemap is a hash-map where keys are organized using prime numbers. The advantage of this map is that you can iterate along the order in which the values were stored in the map.

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
11. `size()`: Returns the number of elements, size of the container.
12. `sum()`: Returns the sum of elements.
13. `test(key)`: Tests if key belongs to the map container.
14. `values()`: Returns the map container values as a vector.

## Initialization
A map can be initialized with a description such as: { "k1":v1, "k2":v2...}
```cpp
map toto = { "a":1, "b":2 };
```

## Operator
- `x in amap`: Returns true or a list of indexes, according to the receiving variable. If the map contains string values, then the system will return true or its index only if a value is the same string as the one tested. The "in" operation is not performed in this case within the local strings.
- Important: `x` is tested against the keys of the map as for `test`.
- `for (s in amap) { ... }`: Loops among all keys. At each iteration, "s" contains a key from `amap`.
- `+`, `*`, `-`, `/` etc.: Adds a value to each element of a map or adds each element of a map to another along keys.
- `&`, `|`: Intersection or union of two maps along keys.

## Indexes
- `map[key]`: Returns the element whose key is `key`. If `key` is not a key from the map, then return null.
- As an integer or a float: Returns the size of the map.
- As a string: Returns a string that mimics the map initialization structure.

## Example
```cpp
map vmap;
vmap["toto"] = 1;
vmap[10] = 27;
print(vmap); // display: {'10':27,'toto':1}
```

## Testing Keys
There are different ways to test whether a map possesses a specific key. The first way is to use the `test` operator, which will return true or false. The other way is to catch the error when a wrong index is provided with the container. However, it is faster and more efficient to use `test` instead of the above equality.
```cpp
if (m.test("ee"))
    println("ee is not a key in m");
```
If you want to avoid an exception whenever a wrong key is used, place `erroronkey(false)` at the beginning of your code. In that case, an empty value will be returned instead of an exception.
```cpp
if (m["ee"] == empty)
    println("ee is not a key in m");
```