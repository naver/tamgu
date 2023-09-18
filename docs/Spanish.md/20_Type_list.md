# Tipo lista

Una `lista` se utiliza para almacenar objetos de cualquier tipo. Proporciona los siguientes métodos. Es diferente de un `vector` en que permite agregar o eliminar elementos tanto desde el frente como desde la parte posterior, lo que permite la gestión FIFO, LILO, FILO o LIFO de las listas.

## Métodos

1. `clear()`: Borra el contenedor.
2. `first()`: Devuelve el primer elemento.
3. `flatten()`: Aplana una estructura de vector.
4. `insert(i, v)`: Inserta `v` en la posición `i`.
5. `join(string sep)`: Produce una representación de cadena del contenedor.
6. `json()`: Devuelve una cadena compatible con JSON que coincide con el contenedor.
7. `last()`: Devuelve el último elemento.
8. `merge(v)`: Fusiona `v` en la lista.
9. `pop(i)`: Borra un elemento de la lista en la posición `i`.
10. `popfirst(i)`: Elimina y devuelve el primer elemento.
11. `poplast(i)`: Elimina y devuelve el último elemento.
12. `product()`: Devuelve el producto de los elementos.
13. `push(v)`: Inserta un valor en la lista.
14. `pushfirst(v)`: Inserta un valor en la lista en la primera posición.
15. `remove(e)`: Elimina `e` del vector.
16. `reverse()`: Invierte un vector.
17. `size()`: Devuelve el número de elementos, el tamaño del contenedor.
18. `shuffle()`: Mezcla los valores de la lista.
19. `sum()`: Devuelve la suma de los elementos.
20. `unique()`: Elimina elementos duplicados.

## Inicialización

Una lista se puede inicializar con una estructura entre `[]`.

```python
list v = [1, 2, 3, 4, 5];
list vs = ["a", "b", "v"];
```

## Operadores

- `x in vlist`: Devuelve verdadero o una lista de índices, dependiendo de la variable receptora. Si la lista contiene cadenas, devolverá verdadero o su índice solo si el valor es la misma cadena que la probada.
- `for (s in vlist) { ... }`: Recorre todos los valores. En cada iteración, `s` contiene un valor de `vlist`.
- `+`, `*`, `-`, `/`, etc.: Agrega un valor a cada elemento de una lista o agrega cada elemento de una lista a otra.
- `&`, `|`: Intersección o unión de dos listas.

## Tamaño

- Como un entero o un flotante: Devuelve el tamaño de la lista.
- Como una cadena: Devuelve una estructura donde cada elemento está separado de los demás con una coma, similar a la estructura utilizada para inicializar un vector o una lista.

## Índices

Puede utilizar índices con objetos de lista, similar a los vectores. Sin embargo, los índices con listas son bastante ineficientes y deben evitarse.

## Ejemplo

```python
list vlist = [1, 2, 3, 4, 5];
vlist.pushfirst(10);
vlist.pushlast(20); // [10, 1, 2, 3, 4, 5, 20]
vlist.popfirst(); // [1, 2, 3, 4, 5, 20]
vector v = vlist; // Transformar una lista en un vector
```