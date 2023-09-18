# Type Appel

Cet objet est utilisé pour suivre les fonctions, qui peuvent ensuite être exécutées. L'appel est fait en utilisant le nom de variable comme fonction.

## Exemple

```c++
int display(int e) {
    print("AFFICHAGE:", e, "\n");
    e += 10;
    return e;
}

int myfunc;
myfunc = display;
int i = myfunc(100);   // affichage: AFFICHAGE:100
print("I=", i, "\n");   // affichage: I=110
```

Dans l'exemple ci-dessus, la fonction `display` prend un paramètre entier `e`, affiche "AFFICHAGE:" suivi de la valeur de `e`, incrémente `e` de 10, et renvoie la valeur mise à jour de `e`. La variable `myfunc` est ensuite assignée à la fonction `display`. Enfin, la variable `myfunc` est appelée avec l'argument `100`, et le résultat est stocké dans la variable `i`. La sortie sera "AFFICHAGE:100" et "I=110".