# Cartes spécialisées

## Maps (Arbres|Premiers|Binaires)

Ces types sont très similaires aux types "Map" et "TreeMap" à une exception près : ils utilisent des entiers (`MapI`, `TreeMapI`, `PrimeMapI`), des flottants (`MapF`, `TreeMapF`, `PrimeMapF`), ou des chaînes de caractères (`MapU`, `TreeMapU`, `PrimeMapU`) comme clés, tandis que "Map", "TreeMap" et "PrimeMap" utilisent des chaînes de caractères.

En fait, pour des raisons de cohérence, "Map", "TreeMap" et "PrimeMap" peuvent également être nommés "Maps", "BinMaps", "TreeMaps" et "PrimeMaps".

**Important** : Ces cartes ne peuvent être accédées qu'à travers la bibliothèque `allmaps`. Si vous souhaitez utiliser l'une de ces cartes étendues, vous devez charger `allmaps` avec `use` au préalable.

```lua
use("allmaps");
TreeMapLL t;
```

Cependant, certaines cartes font déjà partie de l'interpréteur principal et ne nécessitent pas le chargement de la bibliothèque `allmaps` au préalable. Voici la liste complète :

1) Toutes les variations de "Map".
2) "TreeMaps" (S|F|I|L), "TreeMapI", "TreeMapIS", "TreeMapSF".
3) "PrimeMaps" (S|F|I|L).

Pour les autres, vous devez d'abord charger `allmaps`.

## Cartes de valeurs spécialisées

Ces cartes spécifiques ont une clé, qui peut être une chaîne de caractères, un entier ou un flottant, et une valeur, qui est également une chaîne de caractères, un entier ou un flottant. La convention de nommage dans ce cas est :

(Tree|Prime)Map[S|I|F|U][S|I|F|U].

Par exemple, "TreeMapIF" est un TreeMap dont la clé est un entier et la valeur est un flottant.

Ces cartes spécialisées doivent être utilisées autant que possible lorsque les valeurs et les clés sont des valeurs de base. Elles réduisent considérablement l'empreinte mémoire des applications et sont beaucoup plus rapides à utiliser.