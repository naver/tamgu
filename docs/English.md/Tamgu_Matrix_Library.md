# Tamgu Matrix Library

This library provides the `fmatrix` and `imatrix` types for handling matrices in Tamgu. You can define the size of the matrix at creation time and store elements using the `:` operator. Matrices can only store floats.

## Usage

To use the matrix types, you can create a matrix with a specified size and store elements using the `:` operator. Here are some examples:

```cpp
fmatrix m(3, 3); // Create a 3x3 float matrix
imatrix m(3, 3); // Create a 3x3 integer matrix

m[0:0] = 3.0; // Store the value 3.0 at row 0 and column 0
m[1:0] = 0.0; // Store the value 0.0 at row 1 and column 0
m[2:0] = 0.0; // Store the value 0.0 at row 2 and column 0

m[0:] // Returns the row 0 as a float vector
m[:0] // Returns the column 0 as a float vector
```

## Methods

The matrix types provide several methods for performing operations on matrices. Here are some of the available methods:

1. `determinant()`: Returns the determinant of the matrix.
2. `dimension()`: Returns the size of the matrix.
3. `dimension(int rowsize, int columnsize)`: Sets the size of the matrix.
4. `invert()`: Returns the inverted matrix.
5. `product()`: Returns the product of all elements in the matrix.
6. `sum()`: Returns the sum of all elements in the matrix.
7. `transpose()`: Returns the transposed matrix.

## Operators

The matrix types support various operators such as addition (`+`), multiplication (`*`), division (`/`), and subtraction (`-`). However, note that matrix multiplication and division are performed differently compared to scalar operations. 

For example, when multiplying two matrices, the operation follows matrix multiplication rules. The same applies to division.

## Examples

Here are some examples of how to use the matrix types:

```cpp
fmatrix m(3, 3); // Create a 3x3 float matrix
fmatrix v(3, 1); // Create a 3x1 float matrix

v[0:0] = 3.0; // Store the value 3.0 at row 0 and column 0
v[1:0] = 0.0; // Store the value 0.0 at row 1 and column 0
v[2:0] = 0.0; // Store the value 0.0 at row 2 and column 0
```

Feel free to explore more features and functionalities of the Tamgu matrix library.