# fmatrix and imatrix

These types are used to handle matrices. You define the size of the matrix at creation time, and you can store elements by redefining the ":" operator. This operator is used to define the rows and columns of the value to be stored. Matrices can only store floats.

To store an element v at row r and column c, use the syntax `m[r:c]=v`. 

To retrieve a specific row or column as a fvector, use the following syntax:
- `m[r:]` returns the row r as a fvector
- `m[:c]` returns the column c as a fvector

## Methods
1. `determinant()`: Returns the matrix determinant
2. `dimension()`: Returns the matrix size
3. `dimension(int rowsize, int columnsize)`: Sets the matrix size
4. `invert()`: Returns the inverted matrix
5. `product()`: Returns the product of all elements
6. `sum()`: Returns the sum of all elements
7. `transpose()`: Returns the transposed matrix

## Operators
The operators +, *, /, and - are all available. However, note that the multiplication of two matrices multiplies one matrix with the other according to matrix multiplication. The same is true for division.

## Examples
```cpp
// We define the number of rows and columns
fmatrix m(3,3);
fmatrix v(3,1);

// We store elements
v[0:0] = 3;
v[1:0] = 0;
v[2:0] = 0;

float angle = 56;

// Function for loading matrix with a given angle
function loading(fmatrix mx, float θ) {
    θ = θ.radian();
    mx[0:0] = cos(θ);
    mx[0:1] = 0;
    mx[0:2] = sin(θ);
    mx[1:0] = 0;
    mx[1:1] = 1;
    mx[1:2] = 0;
    mx[2:0] = -sin(θ);
    mx[2:1] = 0;
    mx[2:2] = cos(θ);
}

loading(m, angle);

fmatrix vx;

// Matrix multiplication
vx = m * v;

m[0:0] = -2;
m[0:1] = 2;
m[0:2] = -3;
m[1:0] = -1;
m[1:1] = 1;
m[1:2] = 3;
m[2:0] = 2;
m[2:1] = 0;
m[2:2] = -1;

// Calculate the determinant
int det = m.determinant();
println(det);

m[0:0] = 1;
m[0:1] = 2;
m[0:2] = -1;
m[1:0] = -2;
m[1:1] = 1;
m[1:2] = 1;
m[2:0] = 0;
m[2:1] = 3;
m[2:2] = -3;

fmatrix inv;

// Matrix inversion
inv = m.invert();
```