# Tipo (Árbol, Binario, Primo) Mapa

Un mapa es una tabla hash que utiliza cualquier cadena o cualquier elemento que se pueda analizar como una cadena como clave. El mapa en Tamgu convierte cualquier clave en una cadena, lo que significa que "123" y 123 se consideran como una única clave única.

**Nota:**
- Un treemap es similar a un mapa, con la diferencia de que las claves en un treemap se ordenan automáticamente.
- Un binmap también es similar a un mapa, pero las claves son cortas y sus valores están entre 0 y 65535. Las claves siempre se ordenan. Binmap también es la forma más rápida de acceder a los elementos.
**Nota:** binmap está disponible a través de la biblioteca "allmaps" (ver mapa especializado a continuación).
- Un primemap es un hash-map donde las claves se organizan utilizando números primos. La ventaja de este mapa es que puedes iterar a lo largo del orden en el que se almacenaron los valores en el mapa.

## Métodos
1. `clear()`: Borra el contenedor.
2. `find(value)`: Comprueba si un valor pertenece al mapa y devuelve 'true' o las claves correspondientes.
3. `invert()`: Devuelve un mapa con las claves/valores invertidos.
4. `items()`: Devuelve un vector de pares {clave:valor}.
5. `join(string sepkey, string sepvalue)`: Produce una representación de cadena para el contenedor.
6. `json()`: Devuelve una cadena compatible con JSON que coincide con el contenedor.
7. `keys()`: Devuelve las claves del contenedor del mapa como un vector.
8. `merge(v)`: Fusiona v en el vector.
9. `pop(key)`: Borra un elemento del mapa.
10. `product()`: Devuelve el producto de los elementos.
11. `size()`: Devuelve el número de elementos, tamaño del contenedor.
12. `sum()`: Devuelve la suma de los elementos.
13. `test(key)`: Comprueba si la clave pertenece al contenedor del mapa.
14. `values()`: Devuelve los valores del contenedor del mapa como un vector.

## Inicialización
Un mapa se puede inicializar con una descripción como: { "k1":v1, "k2":v2...}
```cpp
map toto = { "a":1, "b":2 };
```

## Operador
- `x in amap`: Devuelve verdadero o una lista de índices, según la variable receptora. Si el mapa contiene valores de cadena, entonces el sistema devolverá verdadero o su índice solo si un valor es la misma cadena que la probada. En este caso, la operación "in" no se realiza dentro de las cadenas locales.
- Importante: `x` se prueba con las claves del mapa como para `test`.
- `for (s in amap) { ... }`: Bucle entre todas las claves. En cada iteración, "s" contiene una clave de `amap`.
- `+`, `*`, `-`, `/` etc.: Agrega un valor a cada elemento de un mapa o agrega cada elemento de un mapa a otro a lo largo de las claves.
- `&`, `|`: Intersección o unión de dos mapas a lo largo de las claves.

## Índices
- `map[key]`: Devuelve el elemento cuya clave es `key`. Si `key` no es una clave del mapa, entonces devuelve null.
- Como un entero o un flotante: Devuelve el tamaño del mapa.
- Como una cadena: Devuelve una cadena que imita la estructura de inicialización del mapa.

## Ejemplo
```cpp
map vmap;
vmap["toto"] = 1;
vmap[10] = 27;
print(vmap); // muestra: {'10':27,'toto':1}
```

## Prueba de claves
Hay diferentes formas de probar si un mapa posee una clave específica. La primera forma es usar el operador `test`, que devolverá verdadero o falso. La otra forma es capturar el error cuando se proporciona un índice incorrecto con el contenedor. Sin embargo, es más rápido y eficiente usar `test` en lugar de la igualdad anterior.
```cpp
if (m.test("ee"))
    println("ee no es una clave en m");
```
Si quieres evitar una excepción cada vez que se utiliza una clave incorrecta, coloca `erroronkey(false)` al principio de tu código. En ese caso, se devolverá un valor vacío en lugar de una excepción.
```cpp
if (m["ee"] == empty)
    println("ee no es una clave en m");
```