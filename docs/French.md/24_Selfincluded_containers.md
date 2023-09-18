# Conteneurs auto-inclus

Dans Tamgu, il existe deux types de conteneurs : les conteneurs de valeurs et les conteneurs d'objets.

## Conteneurs de valeurs
Les conteneurs de valeurs incluent svector, ivector et mapii. Ces conteneurs contiennent des valeurs.

## Conteneurs d'objets
Les conteneurs d'objets incluent vector, map, treemap, table et list. Ces conteneurs contiennent des objets.

## Boucles dans les conteneurs d'objets
Il est possible d'inclure un conteneur à l'intérieur d'un autre conteneur, voire d'inclure un conteneur en lui-même. Cependant, lorsque cela se produit, la structure entière ne peut pas être affichée. À la place, le système remplacera l'auto-référence par "[...]" ou "{...}" s'il s'agit d'une map.

Exemple :

```cpp
vector v = [1..5];
v.push(v);
println(v);
```

Sortie : `v est : [1,2,3,4,5,[...]]`, où "[...]" indique une auto-référence.

Il est important de noter que si un conteneur se contient lui-même, il peut y avoir un problème lors du parcours de la structure, car cela pourrait entraîner une boucle infinie.

## Marquage des méthodes
Tamgu fournit trois méthodes pour aider à résoudre ce problème :

a) `mark(byte v)`: Cette méthode place un marqueur sur un conteneur d'objets.

b) `mark()`: Cette méthode renvoie le marqueur d'un conteneur d'objets.

c) `unmark()`: Cette méthode supprime tous les marqueurs d'un conteneur d'objets.

Exemple :

```cpp
vector v = [10..50:10];
v.push([60..80:10]);
v.push(v);
println(v);

function traversal(self v, int i) {
  if (i == v.size()) {
    println('Fin du vecteur');
    return;
  }
  if (v[i].mark() == 1) {
    println("Déjà analysé");
    return;
  }
  println(i, v[i]);
  v[i].mark(1);
  if (v[i].iscontainer()) {
    println("Cette valeur est un conteneur...");
    traversal(v[i], 0);
  }
  traversal(v, i + 1);
}

// Nous marquons notre vecteur pour éviter de le parcourir deux fois
v.mark(1);
traversal(v, 0);

// Nous supprimons tous les éléments marqués en une seule fois
v.unmark();

// Sortie :
// [10,20,30,40,50,[60,70,80],[...]], il contient une auto-référence
// L'exécution
// 0 10
// 1 20
// 2 30
// 3 40
// 4 50
// 5 [60,70,80]
// Cette valeur est un conteneur...
// 0 60
// 1 70
// 2 80
// Fin du vecteur
// Déjà analysé
// fin
```