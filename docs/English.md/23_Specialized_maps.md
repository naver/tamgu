# Specialized maps

## (tree|prime|bin)maps

These types are very similar to the "map" and "treemap" with one exception: they use integers (`mapI`, `treemapI`, `primemapI`), floats (`mapF`, `treemapf`, `primemapf`), or ustrings (`mapu`, `treemapu`, `primemapu`) as keys, while "map", "treemap", and "primemap" use strings.

Actually, for consistency reasons, "pap", "treemap", and "primemap" can also be named as "maps", "Binmaps", "treemaps", and "primemaps".

**Important**: These maps can only be accessed through the `allmaps` library. If you want to use one of these extended maps, you need to load `allmaps` with `use` beforehand.

```lua
use("allmaps");
treemapll t;
```

However, some maps are already part of the main interpreter and do not require loading the `allmaps` library first. Here is the complete list:

1) All variations for "map".
2) "treemaps" (s|f|i|l), "treemapi", "treemapis", "treemapsf".
3) "primemaps" (s|f|i|l).

For the others, you need to load `allmaps` first.

## Specialized Value maps

These specific maps have a key, which can be a string, an integer, or a float, and a value, which is also necessary a string, an integer, or a float. The naming convention in this case is:

(tree|prime)map[s|i|f|u][s|i|f|u].

For instance, "treemapif" is a treemap whose key is an integer and the value is a float.

These specialized maps should be used as much as possible when the values and keys are basic values. They reduce the memory footprint of applications in a rather important way and are much faster to work with.
