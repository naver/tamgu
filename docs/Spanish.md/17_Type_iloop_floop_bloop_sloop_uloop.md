# Variables de bucle

Las variables de bucle se utilizan para definir variables cuyos valores evolucionan dentro de un intervalo dado. Hay cinco tipos de variables de bucle: `iloop`, `floop`, `bloop`, `sloop` y `uloop`.

## iloop

El tipo `iloop` hace bucle entre enteros.

## floop

El tipo `floop` hace bucle entre flotantes.

## bloop

El tipo `bloop` hace bucle entre `true` y `false`.

## sloop

El tipo `sloop` hace bucle entre cadenas de texto.

## uloop

El tipo `uloop` hace bucle entre ustrings.

## Inicialización

Para inicializar un bucle, se puede utilizar un vector o un rango.

Por ejemplo, para inicializar una variable `iloop` llamada `il` que hace bucle entre los valores 1, 3, 5, 7 y 9, se puede utilizar la siguiente sintaxis:

```cpp
iloop il = [1,3..10];
```

Si se inicializa un bucle con un entero, el valor se considerará como una nueva posición dentro del vector asociado. El valor 0 reiniciará el bucle al primer elemento y el valor -1 reiniciará el bucle al último elemento.

Por ejemplo, si se establece `il` en 3, la variable se actualizará a 7 y el siguiente valor será 9.

```cpp
il = 3; // la variable ahora es 7. El siguiente valor será 9.
```

## Como un vector

Se puede devolver el valor del vector de un bucle utilizando el método `vector` o almacenando su contenido en un vector.

## Función

También se puede asociar una función con una variable de bucle, que se llamará cuando se alcance el último valor del vector inicial antes de volver a hacer bucle. La función tiene la siguiente firma:

```cpp
function callback(loopVar, int pos);
```

Aquí tienes un ejemplo:

```cpp
iloop i = [1..4];
for (int k in <0,10,1>) {
    print(i," ");
    i++;
}
```

El sistema imprimirá: `1 2 3 4 1 2 3 4 1 2`.