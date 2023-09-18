# Iterator y Riterator

Estos iteradores se utilizan para iterar sobre objetos de varios tipos como string, vector, map y rule.

Un riterator es un iterador inverso que permite la iteración desde el final de la colección.

## Métodos

1. `begin()`: Inicializa el iterador al principio de la colección.
2. `end()`: Devuelve verdadero cuando se alcanza el final de la colección.
3. `key()`: Devuelve la clave del elemento actual.
4. `next()`: Se mueve al siguiente elemento de la colección.
5. `value()`: Devuelve el valor del elemento actual.

## Inicialización

Un iterador puede ser inicializado mediante una simple asignación.

## Ejemplo

```cpp
vector<int> v = {1, 2, 3, 4, 5};
iterator it = v;

for (it.begin(); it.end(); it.next()) {
    print(it.value(), ",");
}
```

## Salida

```
1, 2, 3, 4, 5,
```