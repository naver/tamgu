# Value Containers and Logical Operators in Tamgu

Tamgu provides value containers as specific implementations of vectors and maps for strings, floats, and integers. When using logical operators with these containers, their processing depends on the values stored within them.

## String Containers

For string containers, the logical operators function as set operators. The `&` operator yields the intersection between two string containers, the `|` operator yields the union of two string containers, and the `^` operator yields the non-common values between two strings.

```python
svector sv = ["a", "b", "c", "d", "e", "h"];
svector svv = ["e", "f", "g", "h"];

println("And:", sv & svv);  # Output: ['e', 'h']
println("XOR:", sv ^ svv);  # Output: ['f', 'g', 'a', 'b', 'c', 'd']
println("OR:", sv | svv);   # Output: ['a', 'b', 'c', 'd', 'e', 'h', 'f', 'g']
```

## Map Containers

For map containers, the logical operators function similarly to set operators. However, when a non-common value is encountered, the `^` operator displays the non-common value and indicates that it has a different value.

```python
smap sm = { "a": 1, "b": 2, "c": 3, "d": 4, 'e': 5, 'h': 6 };
smap smm = { 'e': 5, "f": 2, "g": 3, 'h': 4 };

println("And:", sm & smm);  # Output: {'e': 5} ('h' has a different value)
println("XOR:", sm ^ smm);  # Output: {'f': 2, 'g': 3, 'a': 1, 'b': 2, 'c': 3, 'd': 4}
println("OR:", sm | smm);   # Output: {'a': 1, 'b': 2, 'c': 3, 'd': 4, 'e': 5, 'h': 6, 'f': 2, 'g': 3}
```

## Numerical Values

For numerical values, the logical operators work at the binary level rather than the set level.

```python
ivector iv = [1, 2, 3, 4, 5, 6, 7, 8, 9];
ivector vi = [2, 4, 6, 8, 10, 12, 14, 16, 18];

println("And:", iv & vi);  # Output: [0, 0, 2, 0, 0, 4, 6, 0, 0]
println("XOR:", iv ^ vi);  # Output: [3, 6, 5, 12, 15, 10, 9, 24, 27]
println("OR:", iv | vi);   # Output: [3, 6, 7, 12, 15, 14, 15, 24, 27]
```

Note: The above code is written in Tamgu syntax.