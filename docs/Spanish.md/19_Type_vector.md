# Tipo vector

Un `vector de tipo` se utiliza para almacenar objetos de cualquier tipo. Proporciona los siguientes métodos:

## Métodos

1. `clear()`: Limpia el contenedor.
2. `convert()`: Detecta valores numéricos en un vector de cadenas y los convierte en números reales. Devuelve un objeto vector.
3. `editdistance(v)`: Calcula la distancia de edición con el vector 'v'.
4. `flatten()`: Aplana una estructura de vector.
5. `insert(i,v)`: Inserta 'v' en la posición 'i'.
6. `join(string sep)`: Produce una representación de cadena para el contenedor.
7. `json()`: Devuelve una cadena compatible con JSON que coincide con el contenedor.
8. `last()`: Devuelve el último elemento.
9. `merge(v)`: Fusiona 'v' en el vector.
10. `pop(i)`: Borra un elemento del vector.
11. `product()`: Devuelve el producto de los elementos.
12. `∏(v,i,j)`: Multiplica los elementos de 'i' a 'j' (opcional).
13. `push(v)`: Inserta un valor en el vector.
14. `read(string path)`: Lee un archivo en el vector.
15. `remove(e)`: Elimina 'e' del vector.
16. `reserve(int sz)`: Reserva un tamaño de 'sz' elementos potenciales en el vector.
17. `reverse()`: Invierte el vector.
18. `size()`: Devuelve el número de elementos en el contenedor.
19. `shape(d1,d2,d3)`: Devuelve las dimensiones de un tensor en un vector unidimensional.
20. `shuffle()`: Mezcla los valores en el vector.
21. `sort(bool reverse | function | method)`: Ordena los valores en el vector.
22. `sortfloat(bool reverse)`: Ordena los valores en el vector como flotantes.
23. `sortint(bool reverse | function)`: Ordena los valores en el vector como enteros.
24. `sortstring(bool reverse)`: Ordena los valores en el vector como cadenas.
25. `sortustring(bool reverse | function)`: Ordena los valores en el vector como cadenas no firmadas.
26. `sum()`: Devuelve la suma de los elementos.
27. `∑(v,i,j)`: Suma los elementos de 'i' a 'j' (opcional).
28. `unique()`: Elimina elementos duplicados.
29. `write(string file)`: Escribe el contenido del vector en un archivo.

## Inicialización

Un vector se puede inicializar con una estructura entre `[]`.

Ejemplo:
```cpp
vector v = [1,2,3,4,5];
vector vs = ["a","b","v"];
vector vr = range(10,20,2); // vr se inicializa con [10,12,14,16,18];
vs = range('a','z',2); // vs se inicializa con ['a','c','e','g','i','k','m','o','q','s','u','w','y']
```

## Funciones matemáticas

También se pueden aplicar funciones matemáticas al contenido de un vector. Consulte los tipos numéricos (int, float, long) para obtener una lista de estas funciones.

Ejemplo:
```cpp
fvector fv = [0,0.1..1];
// fv es: [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1]
fv.cos() es: [1,0.995004,0.980067,0.955336,0.921061,0.877583,0.825336,0.764842,0.696707,0.62161,0.540302]
```

## Operadores

- `x in vect`: Devuelve verdadero o una lista de índices, según la variable receptora. Si el vector contiene cadenas, devolverá verdadero o su índice solo si el valor es igual al que se está probando. Esta operación no se realiza dentro de cadenas locales.
- `for (s in vect) { ... }`: Recorre todos los valores. En cada iteración, 's' contiene un valor de 'vect'.
- `+`, `*`, `-`, `/`, etc.: Añade, multiplica, resta, divide, etc., un valor a cada elemento de un vector o añade cada elemento de un vector a otro.
- `&`, `|`: Devuelve la intersección o unión de dos vectores.
- `&&&`: Fusiona un vector con un valor.
- `|||`: Combina un contenedor con otro contenedor o una cadena con una cadena.
- `v = [1,2,3]; vv = [4,5,6]; println(v ||| vv); //[[1,4],[2,5],[3,6]]`
- `::`: Inserta un valor en un vector.
- `10::[1,2,3]` → `[10,1,2,3]`
- `[1,2,3]::10` → `[1,2,3,10]`

## Extracción de variables de un vector

Tamgu proporciona un método para leer de un vector utilizando un patrón de vector de la forma: `[a1,..,an|tail]`, donde `a1,..,an` y `tail` son variables o valores. La `tail` es el resto del vector una vez que se ha asignado cada variable. Estos patrones de vector se pueden utilizar de dos maneras:

- En asignación:
  - `[a,b|v] = [1,2,3,4,5]`, luego `a=1`, `b=2`, y `v=[3,4,5]`
- En bucles `for..in`:
  - `for ([a,b|v] in [[1,2,3,4],[3,4,5]]) { ... }`
  - En la primera iteración, `a=1`, `b=2`, y `v=[3,4]`
  - En la segunda iteración, `a=3`, `b=4`, y `v=[5]`

## Ejemplos

```cpp
vector vect;
vect = [1,2,3,4,5];
print(vect[0]); // Muestra: 1
print(vect[0:3]); // Muestra: [1,2,3]
vect.push(6);
print(vect); // Muestra: [1,2,3,4,5,6]
vect.pop(1);
print(vect); // Muestra: [1,3,4,5,6]
vect = vect.reverse();
print(vect); // Muestra: [6,5,4,3,1]
vect.pop();
print(vect); // Muestra: [6,5,4,3]
vect += 10;
print(vect); // Muestra: [16,15,14,13]
```

Ejemplo (ordenar enteros en un vector):

```cpp
// Esta función solo debe devolver true o false
// El tipo de los parámetros determinará su comportamiento, en este caso, suponemos que cada elemento es una cadena o se convierte en una cadena.
function compare(int i, int j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// El resultado es: [5,10,20]
```

Ejemplo (ordenar enteros en un vector pero vistos como cadenas):

```cpp
function compare(string i, string j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// El resultado es: [10,20,5]
```

Ejemplo (ordenar cadenas con el método `size`):

```cpp
vector v = ["100","1","22"];
v.sort(size);
// El resultado es: ['1','22','100']
```