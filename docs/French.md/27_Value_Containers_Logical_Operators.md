# Conteneurs de valeurs et opérateurs logiques

Ce document donne un aperçu des conteneurs de valeurs et des opérateurs logiques dans le contexte des chaînes de caractères et des valeurs numériques.

## Conteneurs de chaînes de caractères

Les conteneurs de valeurs dont il est question ici sont des implémentations spécifiques de vecteurs et de maps pour les chaînes de caractères, les nombres flottants et les entiers. Lors de l'utilisation des opérateurs logiques avec ces conteneurs, la façon dont ils sont traités dépend des valeurs stockées dans le conteneur.

Pour les chaînes de caractères, les opérateurs logiques fonctionnent comme des opérateurs d'ensemble. L'opérateur `&` donne l'intersection entre deux conteneurs de chaînes de caractères, l'opérateur `|` donne l'union de deux conteneurs de chaînes de caractères, et l'opérateur `^` donne les valeurs non communes entre deux chaînes de caractères.

Exemple :

```python
svector sv = ["a", "b", "c", "d", "e", "h"];
svector svv = ["e", "f", "g", "h"];

println("And:", sv & svv);  # Résultat : ['e', 'h']
println("XOR:", sv ^ svv);  # Résultat : ['f', 'g', 'a', 'b', 'c', 'd']
println("OR:", sv | svv);   # Résultat : ['a', 'b', 'c', 'd', 'e', 'h', 'f', 'g']
```

## Conteneurs de maps

Pour les conteneurs de maps, le même ensemble d'opérateurs logiques peut être utilisé. Cependant, la façon dont ils sont traités est légèrement différente. L'opérateur `&` donne l'intersection entre deux conteneurs de maps, l'opérateur `|` donne l'union de deux conteneurs de maps, et l'opérateur `^` donne les valeurs non communes entre deux maps.

Exemple :

```python
smap sm = {"a": 1, "b": 2, "c": 3, "d": 4, "e": 5, "h": 6};
smap smm = {"e": 5, "f": 2, "g": 3, "h": 4};

println("And:", sm & smm);  # Résultat : {'e': 5} (h a une valeur différente)
println("XOR:", sm ^ smm);  # Résultat : {'f': 2, 'g': 3, 'a': 1, 'b': 2, 'c': 3, 'd': 4}
println("OR:", sm | smm);   # Résultat : {'a': 1, 'b': 2, 'c': 3, 'd': 4, 'e': 5, 'h': 6, 'f': 2, 'g': 3}
```

## Valeurs numériques

Pour les valeurs numériques, les opérateurs logiques fonctionnent au niveau binaire, plutôt qu'au niveau des ensembles.

Exemple :

```python
ivector iv = [1, 2, 3, 4, 5, 6, 7, 8, 9];
ivector vi = [2, 4, 6, 8, 10, 12, 14, 16, 18];

println("And:", iv & vi);  # Résultat : [0, 0, 2, 0, 0, 4, 6, 0, 0]
println("XOR:", iv ^ vi);  # Résultat : [3, 6, 5, 12, 15, 10, 9, 24, 27]
println("OR:", iv | vi);   # Résultat : [3, 6, 7, 12, 15, 14, 15, 24, 27]
```

Veuillez noter que les valeurs de sortie peuvent varier en fonction de l'implémentation spécifique et du langage de programmation utilisé.