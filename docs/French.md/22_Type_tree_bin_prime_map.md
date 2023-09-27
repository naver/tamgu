# Type (tree, bin, prime) Map

Une map est une table de hachage qui utilise une chaîne de caractères ou tout élément pouvant être analysé comme une chaîne de caractères comme clé. La map dans Tamgu convertit toutes les clés en une chaîne de caractères, ce qui signifie que "123" et 123 sont considérés comme une clé unique.

**Note:**
- Un treemap est similaire à une map, à la différence que les clés dans un treemap sont automatiquement triées.
- Un binmap est également similaire à une map, mais les clés sont courtes et leurs valeurs sont comprises entre 0 et 65535. Les clés sont toujours triées. Binmap est également le moyen le plus rapide pour accéder aux éléments.
**Note:** binmap est disponible via la bibliothèque "allmaps" (voir la map spécialisée ci-dessous).
- Un primemap est une table de hachage où les clés sont organisées en utilisant des nombres premiers. L'avantage de cette map est que vous pouvez itérer dans l'ordre dans lequel les valeurs ont été stockées dans la map.

## Méthodes
1. `clear()`: Vide le conteneur.
2. `find(value)`: Vérifie si une valeur appartient à la map et renvoie 'true' ou les clés correspondantes.
3. `invert()`: Renvoie une map avec les clés/valeurs inversées.
4. `items()`: Renvoie un vecteur de paires {clé:valeur}.
5. `join(string sepkey, string sepvalue)`: Produit une représentation sous forme de chaîne de caractères pour le conteneur.
6. `json()`: Renvoie une chaîne de caractères compatible avec JSON correspondant au conteneur.
7. `keys()`: Renvoie les clés de la map sous forme de vecteur.
8. `merge(v)`: Fusionne v dans le vecteur.
9. `pop(key)`: Supprime un élément de la map.
10. `product()`: Renvoie le produit des éléments.
11. `size()`: Renvoie le nombre d'éléments, la taille du conteneur.
12. `sum()`: Renvoie la somme des éléments.
13. `test(key)`: Vérifie si la clé appartient au conteneur de la map.
14. `values()`: Renvoie les valeurs du conteneur de la map sous forme de vecteur.

## Initialisation
Une map peut être initialisée avec une description telle que: { "k1":v1, "k2":v2...}
```cpp
map toto = { "a":1, "b":2 };
```

## Opérateur
- `x in amap`: Renvoie true ou une liste d'indexes, en fonction de la variable de réception. Si la map contient des valeurs de chaîne de caractères, alors le système renverra true ou son index uniquement si une valeur est la même chaîne de caractères que celle testée. L'opération "in" n'est pas effectuée dans ce cas au sein des chaînes locales.
- Important: `x` est testé par rapport aux clés de la map comme pour `test`.
- `for (s in amap) { ... }`: Boucle parmi toutes les clés. À chaque itération, "s" contient une clé de `amap`.
- `+`, `*`, `-`, `/` etc.: Ajoute une valeur à chaque élément d'une map ou ajoute chaque élément d'une map à une autre en fonction des clés.
- `&`, `|`: Intersection ou union de deux maps en fonction des clés.

## Index
- `map[key]`: Renvoie l'élément dont la clé est `key`. Si `key` n'est pas une clé de la map, alors renvoie null.
- Comme un entier ou un flottant: Renvoie la taille de la map.
- Comme une chaîne de caractères: Renvoie une chaîne de caractères qui imite la structure d'initialisation de la map.

## Exemple
```cpp
map vmap;
vmap["toto"] = 1;
vmap[10] = 27;
print(vmap); // affiche: {'10':27,'toto':1}
```

## Test des clés
Il existe différentes façons de tester si une map possède une clé spécifique. La première façon est d'utiliser l'opérateur `test`, qui renverra true ou false. L'autre façon est de capturer l'erreur lorsqu'un mauvais index est fourni avec le conteneur. Cependant, il est plus rapide et plus efficace d'utiliser `test` au lieu de l'égalité mentionnée ci-dessus.
```cpp
if (m.test("ee"))
    println("ee n'est pas une clé dans m");
```
Si vous voulez éviter une exception chaque fois qu'une mauvaise clé est utilisée, placez `erroronkey(false)` au début de votre code. Dans ce cas, une valeur vide sera renvoyée à la place d'une exception.
```cpp
if (m["ee"] == empty)
    println("ee n'est pas une clé dans m");
```