# Variables de boucle

Les variables de boucle sont utilisées pour définir des variables dont les valeurs évoluent dans un intervalle donné. Il existe cinq types de variables de boucle : `iloop`, `floop`, `bloop`, `sloop` et `uloop`.

## iloop

Le type `iloop` boucle parmi les entiers.

## floop

Le type `floop` boucle parmi les nombres à virgule flottante.

## bloop

Le type `bloop` boucle entre `true` et `false`.

## sloop

Le type `sloop` boucle parmi les chaînes de caractères.

## uloop

Le type `uloop` boucle parmi les ustrings.

## Initialisation

Pour initialiser une boucle, vous pouvez utiliser un vecteur ou une plage de valeurs.

Par exemple, pour initialiser une variable `iloop` nommée `il` qui boucle entre les valeurs 1, 3, 5, 7 et 9, vous pouvez utiliser la syntaxe suivante :

```cpp
iloop il = [1,3..10];
```

Si vous initialisez une boucle avec un entier, la valeur sera considérée comme une nouvelle position dans le vecteur associé. La valeur 0 réinitialisera la boucle au premier élément, et la valeur -1 réinitialisera la boucle au dernier élément.

Par exemple, si vous définissez `il` à 3, la variable sera mise à jour à 7, et la prochaine valeur sera 9.

```cpp
il = 3; // la variable est maintenant 7. La prochaine valeur sera 9.
```

## Comme un vecteur

Vous pouvez obtenir la valeur du vecteur d'une boucle en utilisant la méthode `vector` ou en stockant son contenu dans un vecteur.

## Fonction

Vous pouvez également associer une fonction à une variable de boucle, qui sera appelée lorsque la dernière valeur du vecteur initial est atteinte avant de boucler à nouveau. La fonction a la signature suivante :

```cpp
function callback(loopVar, int pos);
```

Voici un exemple :

```cpp
iloop i = [1..4];
for (int k in <0,10,1>) {
    print(i," ");
    i++;
}
```

Le système affichera : `1 2 3 4 1 2 3 4 1 2`.