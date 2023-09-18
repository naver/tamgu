# Mapas Especializados

## 23.1 Mapas (Árbol|Primo|Binario)

Estos tipos son muy similares a los "Map" y "TreeMap" con una excepción: utilizan enteros (`MapI`, `TreeMapI`, `PrimeMapI`), flotantes (`MapF`, `TreeMapF`, `PrimeMapF`), o cadenas de texto (`MapU`, `TreeMapU`, `PrimeMapU`) como claves, mientras que "Map", "TreeMap" y "PrimeMap" utilizan cadenas de texto.

De hecho, por razones de consistencia, "Map", "TreeMap" y "PrimeMap" también pueden ser nombrados como "Maps", "BinMaps", "TreeMaps" y "PrimeMaps".

**Importante**: Estos mapas solo se pueden acceder a través de la biblioteca `allmaps`. Si desea utilizar uno de estos mapas extendidos, debe cargar `allmaps` con `use` antes.

```lua
use("allmaps");
TreeMapLL t;
```

Sin embargo, algunos mapas ya son parte del intérprete principal y no requieren cargar primero la biblioteca `allmaps`. Aquí está la lista completa:

1) Todas las variaciones de "Map".
2) "TreeMaps" (S|F|I|L), "TreeMapI", "TreeMapIS", "TreeMapSF".
3) "PrimeMaps" (S|F|I|L).

Para los demás, debe cargar `allmaps` primero.

## 23.2 Mapas de Valor Especializados

Estos mapas específicos tienen una clave, que puede ser una cadena de texto, un entero o un flotante, y un valor, que también es necesario que sea una cadena de texto, un entero o un flotante. La convención de nomenclatura en este caso es:

(Tree|Prime)Map[S|I|F|U][S|I|F|U].

Por ejemplo, "TreeMapIF" es un TreeMap cuya clave es un entero y el valor es un flotante.

Estos mapas especializados deben ser utilizados tanto como sea posible cuando los valores y las claves son valores básicos. Reducen de manera importante la huella de memoria de las aplicaciones y son mucho más rápidos de trabajar.