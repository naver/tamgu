# Type vector

A `Type vector` is used to store objects of any type. It provides the following methods:

## Methods

1. `clear()`: Clears the container.
2. `convert()`: Detects number values in a string vector and converts them into actual numbers. Returns a vector object.
3. `editdistance(v)`: Computes the edit distance with vector 'v'.
4. `flatten()`: Flattens a vector structure.
5. `insert(i,v)`: Inserts 'v' at position 'i'.
6. `join(string sep)`: Produces a string representation for the container.
7. `json()`: Returns a JSON compatible string matching the container.
8. `last()`: Returns the last element.
9. `merge(v)`: Merges 'v' into the vector.
10. `pop(i)`: Erases an element from the vector.
11. `product()`: Returns the product of elements.
12. `∏(v,i,j)`: Multiplies the elements from 'i' to 'j' (optional).
13. `push(v)`: Pushes a value into the vector.
14. `read(string path)`: Reads a file into the vector.
15. `remove(e)`: Removes 'e' from the vector.
16. `reserve(int sz)`: Reserves a size of 'sz' potential elements in the vector.
17. `reverse()`: Reverses the vector.
18. `size()`: Returns the number of elements in the container.
19. `shape(d1,d2,d3)`: Returns the dimensions of a tensor on a one-dimensional vector.
20. `shuffle()`: Shuffles the values in the vector.
21. `sort(bool reverse | function | method)`: Sorts the values in the vector.
22. `sortfloat(bool reverse)`: Sorts the values in the vector as floats.
23. `sortint(bool reverse | function)`: Sorts the values in the vector as integers.
24. `sortstring(bool reverse)`: Sorts the values in the vector as strings.
25. `sortustring(bool reverse | function)`: Sorts the values in the vector as unsigned strings.
26. `sum()`: Returns the sum of elements.
27. `∑(v,i,j)`: Sums the elements from 'i' to 'j' (optional).
28. `unique()`: Removes duplicate elements.
29. `write(string file)`: Writes the vector content into a file.

## Initialization

A vector can be initialized with a structure between `[]`.

Example:
```cpp
vector v = [1,2,3,4,5];
vector vs = ["a","b","v"];
vector vr = range(10,20,2); // vr is initialized with [10,12,14,16,18];
vs = range('a','z',2); // vs is initialized with ['a','c','e','g','i','k','m','o','q','s','u','w','y']
```

## Mathematical functions

You can also apply mathematical functions to the content of a vector. See the numerical types (int, float, long) for a list of these functions.

Example:
```cpp
fvector fv = [0,0.1..1];
// fv is: [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1]
fv.cos() is: [1,0.995004,0.980067,0.955336,0.921061,0.877583,0.825336,0.764842,0.696707,0.62161,0.540302]
```

## Operators

- `x in vect`: Returns true or a list of indexes, according to the receiving variable. If the vector contains strings, it will return true or its index only if the value is the same as the one being tested. This operation is not performed within local strings.
- `for (s in vect) { ... }`: Loops through all values. At each iteration, 's' contains a value from 'vect'.
- `+`, `*`, `-`, `/`, etc.: Adds, multiplies, subtracts, divides, etc., a value to each element of a vector or adds each element of a vector to another.
- `&`, `|`: Returns the intersection or union of two vectors.
- `&&&`: Merges a vector with a value.
- `|||`: Combines a container with another container or a string with a string.
- `v = [1,2,3]; vv = [4,5,6]; println(v ||| vv); //[[1,4],[2,5],[3,6]]`
- `::`: Inserts a value in a vector.
- `10::[1,2,3]` → `[10,1,2,3]`
- `[1,2,3]::10` → `[1,2,3,10]`

## Extracting variables from a vector

Tamgu provides a method to read from a vector using a vector pattern of the form: `[a1,..,an|tail]`, where `a1,..,an` and `tail` are variables or values. The `tail` is the rest of the vector once each variable has been assigned. These vector patterns can be used in two ways:

- In assignment:
  - `[a,b|v] = [1,2,3,4,5]`, then `a=1`, `b=2`, and `v=[3,4,5]`
- In `for..in` loops:
  - `for ([a,b|v] in [[1,2,3,4],[3,4,5]]) { ... }`
  - In the first iteration, `a=1`, `b=2`, and `v=[3,4]`
  - In the second iteration, `a=3`, `b=4`, and `v=[5]`

## Examples

```cpp
vector vect;
vect = [1,2,3,4,5];
print(vect[0]); // Display: 1
print(vect[0:3]); // Display: [1,2,3]
vect.push(6);
print(vect); // Display: [1,2,3,4,5,6]
vect.pop(1);
print(vect); // Display: [1,3,4,5,6]
vect = vect.reverse();
print(vect); // Display: [6,5,4,3,1]
vect.pop();
print(vect); // Display: [6,5,4,3]
vect += 10;
print(vect); // Display: [16,15,14,13]
```

Example (sorting integers in a vector):

```cpp
// This function should return only true or false
// The type of the parameters will determine its behavior, in this case, we suppose each element to be a string or converted as a string.
function compare(int i, int j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// Result is: [5,10,20]
```

Example (sorting integers in a vector but seen as strings):

```cpp
function compare(string i, string j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// Result is: [10,20,5]
```

Example (sorting strings with the `size` method):

```cpp
vector v = ["100","1","22"];
v.sort(size);
// Result is: ['1','22','100']
```