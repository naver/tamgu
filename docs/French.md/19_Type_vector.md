# Vecteur de type

Un `vecteur de type` est utilisé pour stocker des objets de n'importe quel type. Il fournit les méthodes suivantes :

## Méthodes

1. `clear()`: Efface le conteneur.
2. `convert()`: Détecte les valeurs numériques dans un vecteur de chaînes et les convertit en nombres réels. Renvoie un objet vecteur.
3. `editdistance(v)`: Calcule la distance d'édition avec le vecteur 'v'.
4. `flatten()`: Aplatit une structure de vecteur.
5. `insert(i,v)`: Insère 'v' à la position 'i'.
6. `join(string sep)`: Produit une représentation sous forme de chaîne pour le conteneur.
7. `json()`: Renvoie une chaîne compatible JSON correspondant au conteneur.
8. `last()`: Renvoie le dernier élément.
9. `merge(v)`: Fusionne 'v' dans le vecteur.
10. `pop(i)`: Efface un élément du vecteur.
11. `product()`: Renvoie le produit des éléments.
12. `∏(v,i,j)`: Multiplie les éléments de 'i' à 'j' (facultatif).
13. `push(v)`: Ajoute une valeur dans le vecteur.
14. `read(string path)`: Lit un fichier dans le vecteur.
15. `remove(e)`: Supprime 'e' du vecteur.
16. `reserve(int sz)`: Réserve une taille de 'sz' éléments potentiels dans le vecteur.
17. `reverse()`: Inverse le vecteur.
18. `size()`: Renvoie le nombre d'éléments dans le conteneur.
19. `shape(d1,d2,d3)`: Renvoie les dimensions d'un tenseur sur un vecteur unidimensionnel.
20. `shuffle()`: Mélange les valeurs dans le vecteur.
21. `sort(bool reverse | function | method)`: Trie les valeurs dans le vecteur.
22. `sortfloat(bool reverse)`: Trie les valeurs dans le vecteur en tant que flottants.
23. `sortint(bool reverse | function)`: Trie les valeurs dans le vecteur en tant qu'entiers.
24. `sortstring(bool reverse)`: Trie les valeurs dans le vecteur en tant que chaînes de caractères.
25. `sortustring(bool reverse | function)`: Trie les valeurs dans le vecteur en tant que chaînes de caractères non signées.
26. `sum()`: Renvoie la somme des éléments.
27. `∑(v,i,j)`: Fait la somme des éléments de 'i' à 'j' (facultatif).
28. `unique()`: Supprime les éléments en double.
29. `write(string file)`: Écrit le contenu du vecteur dans un fichier.

## Initialisation

Un vecteur peut être initialisé avec une structure entre `[]`.

Exemple :
```cpp
vector v = [1,2,3,4,5];
vector vs = ["a","b","v"];
vector vr = range(10,20,2); // vr est initialisé avec [10,12,14,16,18];
vs = range('a','z',2); // vs est initialisé avec ['a','c','e','g','i','k','m','o','q','s','u','w','y']
```

## Fonctions mathématiques

Vous pouvez également appliquer des fonctions mathématiques au contenu d'un vecteur. Voir les types numériques (int, float, long) pour une liste de ces fonctions.

Exemple :
```cpp
fvector fv = [0,0.1..1];
// fv est : [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1]
fv.cos() est : [1,0.995004,0.980067,0.955336,0.921061,0.877583,0.825336,0.764842,0.696707,0.62161,0.540302]
```

## Opérateurs

- `x in vect` : Renvoie true ou une liste d'index, en fonction de la variable réceptrice. Si le vecteur contient des chaînes de caractères, il renverra true ou son index uniquement si la valeur est identique à celle testée. Cette opération n'est pas effectuée dans les chaînes locales.
- `for (s in vect) { ... }` : Parcourt toutes les valeurs. À chaque itération, 's' contient une valeur de 'vect'.
- `+`, `*`, `-`, `/`, etc. : Ajoute, multiplie, soustrait, divise, etc., une valeur à chaque élément d'un vecteur ou ajoute chaque élément d'un vecteur à un autre.
- `&`, `|` : Renvoie l'intersection ou l'union de deux vecteurs.
- `&&&` : Fusionne un vecteur avec une valeur.
- `|||` : Combine un conteneur avec un autre conteneur ou une chaîne avec une chaîne.
- `v = [1,2,3]; vv = [4,5,6]; println(v ||| vv); //[[1,4],[2,5],[3,6]]`
- `::` : Insère une valeur dans un vecteur.
- `10::[1,2,3]` → `[10,1,2,3]`
- `[1,2,3]::10` → `[1,2,3,10]`

## Extraction de variables d'un vecteur

Tamgu fournit une méthode pour lire à partir d'un vecteur en utilisant un motif de vecteur de la forme : `[a1,..,an|tail]`, où `a1,..,an` et `tail` sont des variables ou des valeurs. La `tail` est le reste du vecteur une fois que chaque variable a été assignée. Ces motifs de vecteur peuvent être utilisés de deux manières :

- Dans l'affectation :
  - `[a,b|v] = [1,2,3,4,5]`, alors `a=1`, `b=2` et `v=[3,4,5]`
- Dans les boucles `for..in` :
  - `for ([a,b|v] in [[1,2,3,4],[3,4,5]]) { ... }`
  - Dans la première itération, `a=1`, `b=2` et `v=[3,4]`
  - Dans la deuxième itération, `a=3`, `b=4` et `v=[5]`

## Exemples

```cpp
vector vecteur;
vecteur = [1,2,3,4,5];
print(vecteur[0]); // Affichage : 1
print(vecteur[0:3]); // Affichage : [1,2,3]
vecteur.push(6);
print(vecteur); // Affichage : [1,2,3,4,5,6]
vecteur.pop(1);
print(vecteur); // Affichage : [1,3,4,5,6]
vecteur = vecteur.reverse();
print(vecteur); // Affichage : [6,5,4,3,1]
vecteur.pop();
print(vecteur); // Affichage : [6,5,4,3]
vecteur += 10;
print(vecteur); // Affichage : [16,15,14,13]
```

Exemple (tri des entiers dans un vecteur) :

```cpp
// Cette fonction devrait renvoyer uniquement true ou false
// Le type des paramètres déterminera son comportement, dans ce cas, nous supposons que chaque élément est une chaîne ou converti en chaîne.
function compare(int i, int j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// Le résultat est : [5,10,20]
```

Exemple (tri des entiers dans un vecteur mais vus comme des chaînes de caractères) :

```cpp
function compare(string i, string j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// Le résultat est : [10,20,5]
```

Exemple (tri des chaînes de caractères avec la méthode `size`) :

```cpp
vector v = ["100","1","22"];
v.sort(size);
// Le résultat est : ['1','22','100']
```