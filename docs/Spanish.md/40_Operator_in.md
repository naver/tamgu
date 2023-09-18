# Operador in

Este operador tiene un comportamiento rico, por lo que tenemos una sección dedicada para explicarlo. En la descripción anterior, ya discutimos algunos posibles usos de este operador con archivos, vectores, mapas o cadenas. Ahora, veamos cómo también se puede usar con frames.

## "in" dentro de un Frame

Un frame puede tener una función `in`, que se utiliza cuando se aplica el operador `in` a un frame. Si un objeto frame no tiene una función `in`, entonces el operador siempre devuelve falso.

### Ejemplo

```cpp
// Este es un ejemplo de cómo usar el operador in con un mapa.
map dico;
vector lst;
dico = { 'a': 1, 'b': 6, 'c': 4, 'd': 6 };

// Prueba booleana, devuelve verdadero o falso
if (6 in dico)
    print("Como se esperaba\n");

// El receptor es una lista, por lo que devolvemos la lista de índices
lst = 6 in dico;
string s;
for (s in lst)
    print("LST: ", s, "\n");
```

Salida:
```
Como se esperaba
LST: b
LST: d
```

Como podemos ver en este ejemplo, el sistema devuelve información relacionada con el tipo del receptor.

### Ejemplo con un Frame

```cpp
frame testframe {
    int i;

    // El tipo del parámetro puede ser cualquier cosa
    function in(int j) {
        if (i == j)
            return true;
        return false;
    }
}
```

Ten en cuenta que el código ha sido formateado para una mejor legibilidad.