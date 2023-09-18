# fmatrix e imatrix

Estos tipos se utilizan para manejar matrices. Se define el tamaño de la matriz al momento de su creación y se pueden almacenar elementos redefiniendo el operador ":". Este operador se utiliza para definir las filas y columnas del valor que se va a almacenar. Las matrices solo pueden almacenar números flotantes.

Para almacenar un elemento v en la fila r y columna c, utiliza la sintaxis `m[r:c]=v`.

Para recuperar una fila o columna específica como un fvector, utiliza la siguiente sintaxis:
- `m[r:]` devuelve la fila r como un fvector
- `m[:c]` devuelve la columna c como un fvector

## Métodos
1. `determinant()`: Devuelve el determinante de la matriz
2. `dimension()`: Devuelve el tamaño de la matriz
3. `dimension(int rowsize, int columnsize)`: Establece el tamaño de la matriz
4. `invert()`: Devuelve la matriz invertida
5. `product()`: Devuelve el producto de todos los elementos
6. `sum()`: Devuelve la suma de todos los elementos
7. `transpose()`: Devuelve la matriz transpuesta

## Operadores
Los operadores +, *, / y - están disponibles. Sin embargo, ten en cuenta que la multiplicación de dos matrices multiplica una matriz por la otra según la multiplicación de matrices. Lo mismo ocurre con la división.

## Ejemplos
```cpp
// Definimos el número de filas y columnas
fmatrix m(3,3);
fmatrix v(3,1);

// Almacenamos elementos
v[0:0] = 3;
v[1:0] = 0;
v[2:0] = 0;

float angle = 56;

// Función para cargar una matriz con un ángulo dado
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

// Multiplicación de matrices
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

// Calcular el determinante
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

// Inversión de la matriz
inv = m.invert();
```