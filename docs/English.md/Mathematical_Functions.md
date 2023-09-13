## Mathematical Functions

You can also apply mathematical functions to the content of a vector. See the numerical types (int, float, long) for a list of these functions.

Example:
```python
fvector fv = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1]
fv.cos() is: [1, 0.995004, 0.980067, 0.955336, 0.921061, 0.877583, 0.825336, 0.764842, 0.696707, 0.62161, 0.540302]
```

### Operators
- `x in vect`: Returns true or a list of indexes, according to the receiving variable. If the vector contains strings, the system will return true or its index only if the value is the same string as the one tested. The `in` operator is not performed in this case within the local strings.
- `for (s in vect) {…}`: Loops among all values. At each iteration, "s" contains a value from the vector.
- `+`, `*`, `-`, `/`, etc.: Adds a value to each element of a vector or adds each element of a vector to another.
- `&`, `|`: Intersection or union of two vectors.
- `&&&`: Merges a vector with a value.
- `|||`: Combines a container with another container or a string with a string.

Example:
```python
vector v = [1, 2, 3]
vector vv = [4, 5, 6]
println(v ||| vv) // [[1, 4], [2, 5], [3, 6]]
```

- `::`: Inserts a value in a vector.
  - `10::[1, 2, 3]` → [10, 1, 2, 3]
  - `[1, 2, 3]::10` → [1, 2, 3, 10]

### As an Integer or a Float
- It returns the size of the vector.

### As a String
- It returns a structure where each element is separated from the others with a comma, similar to the structure used to initialize a vector.

### Indexes
- `str[i]`: Returns the ith character of a vector.
- `str[i:j]`: Returns the sub-vector between i and j.

### Tensors
A matrix can be represented either as a vector of vectors or as a flat list of values combined with a shape. In the latter case, we can use a specific way to access elements: `[*d1, d2, ...]`.
- `tensor [*i, j]`: Returns the element at position i, j in a tensor or a matrix.
- When the dimensions are negative, whole lines or columns are extracted at once:
  - Dimension is -1: We extract along the tensor's lines.
  - Dimension is -2: We extract along the tensor's columns.
- Important: If you interpolate negative and positive dimensions, the result is unpredictable. Negative dimensions should always be together. The last negative dimension in a list defines the line or column extraction.
  - `tensor [*-1, -1, 1]`: Extracts lines with elements always at position 1.
  - `tensor [*-1, -2, 1]`: Extracts columns with elements always at position 1.
  - `tensor [*4, -2, 1]`: Extracts columns with elements always at position 1 on the 4th top line.
  - `tensor [*4, -2, 6, -1, 1]`: Interpolation, result is unpredictable.

Example:
```python
fvector fv = [0..59]
// We define a shape: 3x4x5
fv.shape(3, 4, 5)
// One element at position 1x2x3: 33
println(fv[*1, 2, 3])
// A full line: [30, 31, 32, 33, 34]
println(fv[*1, 2])
// Iteration on inner lines: [1, 6, 11, 16, 21, 26, 31, 36, 41, 46, 51, 56]
println(fv[*-1, -1, 1])
// Iteration on inner columns: [1, 21, 41, 2, 22, 42, 3, 23, 43, 4, 24, 44]
println(fv[*-1, -2, 1])
// Returns all values line by line: [0..59]
println(fv[*-1, -1])
// Returns all values column by column:
// [0, 1, 2, 3, 4, 20, 21, 22, 23, 24, 40, 41, 42, 43, 44, 5, 6, 7, 8, 9, 25, 26, 27, 28, 29, 45, 46, ...]
println(fv[*-1, -2])
```

### Extracting Variables from a Vector
Tamgu provides a very peculiar method to read from a vector. You can use a vector pattern of the form: `[a1, ..., an|tail]`, where a1, ..., an, tail are variables or values. The tail is the rest of the vector once each variable has been assigned.

These vector patterns can be used in two ways:
- In assignment:
  - `[a, b|v] = [1, 2, 3, 4, 5]`, then a = 1, b = 2, and v = [3, 4, 5]
- In `for..in` loops:
  - `for ([a, b|v] in [[1, 2, 3, 4], [3, 4, 5]])` etc.
  - In the first iteration, a = 1, b = 2, and v = [3, 4]
  - In the second iteration, a = 3, b = 4, and v = [5]

Example:
```python
vector vect
vect = [1, 2, 3, 4, 5]
print(vect[0])  // Display: 1
print(vect[0:3])  // Display: [1, 2, 3]
vect.push(6)
print(vect)  // Display: [1, 2, 3, 4, 5, 6]
vect.pop(1)
print(vect)  // Display: [1, 3, 4, 5, 6]
vect = vect.reverse()
print(vect)  // Display: [6, 5, 4, 3, 1]
vect.pop()
print(vect)  // Display: [6, 5, 4, 3]
vect += 10
print(vect)  // Display: [16, 15, 14, 13]
```