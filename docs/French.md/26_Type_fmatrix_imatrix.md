# fmatrix et imatrix

Ces types sont utilisés pour manipuler des matrices. Vous définissez la taille de la matrice lors de sa création, et vous pouvez stocker des éléments en redéfinissant l'opérateur ":". Cet opérateur est utilisé pour définir les lignes et les colonnes de la valeur à stocker. Les matrices ne peuvent stocker que des nombres à virgule flottante.

Pour stocker un élément v à la ligne r et à la colonne c, utilisez la syntaxe `m[r:c]=v`.

Pour récupérer une ligne ou une colonne spécifique en tant que fvector, utilisez la syntaxe suivante :
- `m[r:]` renvoie la ligne r en tant que fvector
- `m[:c]` renvoie la colonne c en tant que fvector

## Méthodes
1. `determinant()`: Renvoie le déterminant de la matrice
2. `dimension()`: Renvoie la taille de la matrice
3. `dimension(int rowsize, int columnsize)`: Définit la taille de la matrice
4. `invert()`: Renvoie la matrice inversée
5. `product()`: Renvoie le produit de tous les éléments
6. `sum()`: Renvoie la somme de tous les éléments
7. `transpose()`: Renvoie la matrice transposée

## Opérateurs
Les opérateurs +, *, / et - sont tous disponibles. Cependant, notez que la multiplication de deux matrices multiplie une matrice par l'autre selon la multiplication matricielle. Il en va de même pour la division.

## Exemples
```cpp
// Nous définissons le nombre de lignes et de colonnes
fmatrix m(3,3);
fmatrix v(3,1);

// Nous stockons des éléments
v[0:0] = 3;
v[1:0] = 0;
v[2:0] = 0;

float angle = 56;

// Fonction pour charger une matrice avec un angle donné
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

// Multiplication de matrices
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

// Calcul du déterminant
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

// Inversion de la matrice
inv = m.invert();
```