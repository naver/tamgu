# Dictionaires spécialisées

## maps (tree|prime|bin)

Ces types sont très similaires aux types "map" et "treemap" à une exception près : ils utilisent des entiers (`mapi`, `treemapi`, `primemapi`), des flottants (`mapf`, `treemapf`, `primemapf`), ou des chaînes de caractères (`mapu`, `treemapu`, `primemapu`) comme clés, tandis que "map", "treemap" et "primemap" utilisent des chaînes de caractères.

En fait, pour des raisons de cohérence, "map", "treemap" et "primemap" peuvent également être nommés "maps", "Binmaps", "treemaps" et "primemaps".

**Important** : Ces dictionnaires ne peuvent être accédées qu'à travers la bibliothèque `allmaps`. Si vous souhaitez utiliser l'un de ces dictionnaires étendus, vous devez charger `allmaps` avec `use` au préalable.

```lua
use("allmaps");
treemapll t;
```

Cependant, certaines dictionnaires font déjà partie de l'interpréteur principal et ne nécessitent pas le chargement de la bibliothèque `allmaps` au préalable. Voici la liste complète :

1) Toutes les variations de "map".
2) "treemaps" (s|f|i|l|u), "treemapi", "treemapis", "treemapsf".
3) "primemaps" (s|f|i|l|u).

Pour les autres, vous devez d'abord charger `allmaps`.

## Dictionnaire de valeurs spécialisées

Ces dictionnaires spécifiques ont une clé, qui peut être une chaîne de caractères, un entier ou un flottant, et une valeur, qui est également une chaîne de caractères, un entier ou un flottant. La convention de nommage dans ce cas est :

(tree|prime)map[s|f|i|l|u][s|f|i|l|u].

Par exemple, "treemapif" est un treemap dont la clé est un entier et la valeur est un flottant.

Ces dictionnaires spécialisées doivent être utilisées autant que possible lorsque les valeurs et les clés sont des valeurs de base. Elles réduisent considérablement l'empreinte mémoire des applications et sont beaucoup plus rapides à utiliser.
