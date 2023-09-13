# Specialized Maps in Tamgu

## 23.1 (Tree|Prime|Bin)Map[s|i|f|u|l]

These types are very similar to the "Map" and "TreeMap" with one exception: they use integer (MapI, TreeMapI, PrimeMapI), float (MapF, TreeMapF, PrimeMapF), or ustring (MapU, TreeMapU, PrimeMapU) as keys, while "Map", "TreeMap", and "PrimeMap" use strings. 

Actually, for consistency reasons, "Map", "TreeMap", and "PrimeMap" can also be named "Maps", "BinMaps", "TreeMaps", and "PrimeMaps".

**Important:** 

These maps can only be accessed through the "allmaps" library. If you want to use one of these extended maps, you need to load "allmaps" with the "use" command beforehand.

```javascript
use("allmaps");
```

Example usage:

```javascript
TreeMapLL t;
```

However, some maps are already part of the main interpreter and do not require loading the "allmaps" library first. Here is the complete list:

1) All variations for Map.
2) TreeMaps[S|F|I|L], TreeMapI, TreeMapIS, TreeMapF.
3) PrimeMaps[S|F|I|L].

For the others, you need to load "allmaps" first.

## 23.2 Specialized Value Maps

These specific maps have a key, which can be a string, an integer, or a float, and a value, which is also a string, an integer, or a float. The naming convention in this case is:

(Tree|Prime)Map[S|I|F|U][S|I|F|U].

For instance, TreeMapIF is a TreeMap whose key is an integer and the value is a float.