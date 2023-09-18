# Contenedores de Valores y Operadores Lógicos

Este documento proporciona una descripción general de los contenedores de valores y los operadores lógicos en el contexto de cadenas de texto y valores numéricos.

## Contenedores de Cadenas de Texto

Los contenedores de valores discutidos aquí son implementaciones específicas de vectores y mapas para cadenas de texto, números decimales y enteros. Al utilizar operadores lógicos con estos contenedores, la forma en que se procesan depende de los valores almacenados en el contenedor.

Para las cadenas de texto, los operadores lógicos funcionan como operadores de conjuntos. El operador `&` produce la intersección entre dos contenedores de cadenas de texto, el operador `|` produce la unión de dos contenedores de cadenas de texto, y el operador `^` produce los valores no comunes entre dos cadenas de texto.

Ejemplo:

```python
svector sv = ["a", "b", "c", "d", "e", "h"];
svector svv = ["e", "f", "g", "h"];

println("And:", sv & svv);  # Salida: ['e', 'h']
println("XOR:", sv ^ svv);  # Salida: ['f', 'g', 'a', 'b', 'c', 'd']
println("OR:", sv | svv);   # Salida: ['a', 'b', 'c', 'd', 'e', 'h', 'f', 'g']
```

## Contenedores de Mapas

Para los contenedores de mapas, se pueden utilizar los mismos operadores lógicos. Sin embargo, la forma en que se procesan es ligeramente diferente. El operador `&` produce la intersección entre dos contenedores de mapas, el operador `|` produce la unión de dos contenedores de mapas, y el operador `^` produce los valores no comunes entre dos mapas.

Ejemplo:

```python
smap sm = {"a": 1, "b": 2, "c": 3, "d": 4, "e": 5, "h": 6};
smap smm = {"e": 5, "f": 2, "g": 3, "h": 4};

println("And:", sm & smm);  # Salida: {'e': 5} (h tiene un valor diferente)
println("XOR:", sm ^ smm);  # Salida: {'f': 2, 'g': 3, 'a': 1, 'b': 2, 'c': 3, 'd': 4}
println("OR:", sm | smm);   # Salida: {'a': 1, 'b': 2, 'c': 3, 'd': 4, 'e': 5, 'h': 6, 'f': 2, 'g': 3}
```

## Valores Numéricos

Para valores numéricos, los operadores lógicos funcionan a nivel binario, en lugar de a nivel de conjunto.

Ejemplo:

```python
ivector iv = [1, 2, 3, 4, 5, 6, 7, 8, 9];
ivector vi = [2, 4, 6, 8, 10, 12, 14, 16, 18];

println("And:", iv & vi);  # Salida: [0, 0, 2, 0, 0, 4, 6, 0, 0]
println("XOR:", iv ^ vi);  # Salida: [3, 6, 5, 12, 15, 10, 9, 24, 27]
println("OR:", iv | vi);   # Salida: [3, 6, 7, 12, 15, 14, 15, 24, 27]
```

Ten en cuenta que los valores de salida pueden variar dependiendo de la implementación específica y el lenguaje de programación utilizado.