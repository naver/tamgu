# Type list

Une `list` est utilisée pour stocker des objets de n'importe quel type. Elle fournit les méthodes suivantes. Elle est différente d'un `vector` car elle permet d'ajouter ou de supprimer des éléments à la fois à l'avant et à l'arrière, ce qui permet une gestion FIFO, LILO, FILO ou LIFO des listes.

## Méthodes

1. `clear()`: Efface le conteneur.
2. `first()`: Renvoie le premier élément.
3. `flatten()`: Aplatit une structure de vecteur.
4. `insert(i, v)`: Insère `v` à la position `i`.
5. `join(string sep)`: Produit une représentation sous forme de chaîne de caractères du conteneur.
6. `json()`: Renvoie une chaîne de caractères compatible JSON correspondant au conteneur.
7. `last()`: Renvoie le dernier élément.
8. `merge(v)`: Fusionne `v` dans la liste.
9. `pop(i)`: Supprime un élément de la liste à la position `i`.
10. `popfirst(i)`: Supprime et renvoie le premier élément.
11. `poplast(i)`: Supprime et renvoie le dernier élément.
12. `product()`: Renvoie le produit des éléments.
13. `push(v)`: Ajoute une valeur à la liste.
14. `pushfirst(v)`: Ajoute une valeur à la liste en première position.
15. `remove(e)`: Supprime `e` du vecteur.
16. `reverse()`: Inverse un vecteur.
17. `size()`: Renvoie le nombre d'éléments, la taille du conteneur.
18. `shuffle()`: Mélange les valeurs de la liste.
19. `sum()`: Renvoie la somme des éléments.
20. `unique()`: Supprime les éléments en double.

## Initialisation

Une liste peut être initialisée avec une structure entre `[]`.

```python
list v = [1, 2, 3, 4, 5];
list vs = ["a", "b", "v"];
```

## Opérateurs

- `x in vlist`: Renvoie vrai ou une liste d'indexes, selon la variable de réception. Si la liste contient des chaînes de caractères, elle renverra vrai ou son index uniquement si la valeur est la même chaîne de caractères que celle testée.
- `for (s in vlist) { ... }`: Parcourt toutes les valeurs. À chaque itération, `s` contient une valeur de `vlist`.
- `+`, `*`, `-`, `/`, etc. : Ajoute une valeur à chaque élément d'une liste ou ajoute chaque élément d'une liste à une autre.
- `&`, `|`: Intersection ou union de deux listes.

## Taille

- En tant qu'entier ou flottant : Renvoie la taille de la liste.
- En tant que chaîne de caractères : Renvoie une structure où chaque élément est séparé des autres par une virgule, similaire à la structure utilisée pour initialiser un vecteur ou une liste.

## Indexes

Vous pouvez utiliser des indexes avec des objets de type liste, de la même manière que pour les vecteurs. Cependant, les indexes avec les listes sont plutôt inefficaces et doivent être évités.

## Exemple

```python
list vlist = [1, 2, 3, 4, 5];
vlist.pushfirst(10);
vlist.pushlast(20); // [10, 1, 2, 3, 4, 5, 20]
vlist.popfirst(); // [1, 2, 3, 4, 5, 20]
vector v = vlist; // Transforme une liste en un vecteur
```