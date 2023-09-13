# Specialized Maps

## 23.1 (Tree|Prime|Bin)Maps

These types are very similar to the "Map" and "TreeMap" with one exception: they use integers (`MapI`, `TreeMapI`, `PrimeMapI`), floats (`MapF`, `TreeMapF`, `PrimeMapF`), or ustrings (`MapU`, `TreeMapU`, `PrimeMapU`) as keys, while "Map", "TreeMap", and "PrimeMap" use strings.

Actually, for consistency reasons, "Map", "TreeMap", and "PrimeMap" can also be named as "Maps", "BinMaps", "TreeMaps", and "PrimeMaps".

**Important**: These maps can only be accessed through the `allmaps` library. If you want to use one of these extended maps, you need to load `allmaps` with `use` beforehand.

```lua
use("allmaps");
TreeMapLL t;
```

However, some maps are already part of the main interpreter and do not require loading the `allmaps` library first. Here is the complete list:

1) All variations for "Map".
2) "TreeMaps" (S|F|I|L), "TreeMapI", "TreeMapIS", "TreeMapSF".
3) "PrimeMaps" (S|F|I|L).

For the others, you need to load `allmaps` first.

## 23.2 Specialized Value Maps

These specific maps have a key, which can be a string, an integer, or a float, and a value, which is also necessary a string, an integer, or a float. The naming convention in this case is:

(Tree|Prime)Map[S|I|F|U][S|I|F|U].

For instance, "TreeMapIF" is a TreeMap whose key is an integer and the value is a float.

These specialized maps should be used as much as possible when the values and keys are basic values. They reduce the memory footprint of applications in a rather important way and are much faster to work with.