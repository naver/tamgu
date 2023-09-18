# Synchronisation

Tamgu offre une manière simple de mettre les threads en attente. Le processus est très simple à implémenter. Tamgu fournit différentes fonctions à cet effet :

## Méthodes

1. `cast(string)`: Cette instruction libère l'exécution de tous les threads en attente sur la chaîne spécifiée.
2. `cast()`: Cette instruction libère tous les threads, quel que soit leur état de chaîne.
3. `lock(string s)`: Cette instruction verrouille une portion de code pour empêcher deux threads d'accéder aux mêmes lignes en même temps.
4. `unlock(string s)`: Cette instruction supprime un verrou pour permettre à d'autres threads d'accéder au contenu d'une fonction.
5. `waitonfalse(var)`: Cette fonction met un thread en attente jusqu'à ce que la valeur de la variable `var` soit définie sur false (ou zéro, ou toute autre valeur qui renvoie false).
6. `waitonjoined()`: Cette fonction attend que les threads lancés dans le thread actuel se terminent. Ces threads doivent être déclarés avec le drapeau `join`.
7. `wait(string)`: Cette fonction met un thread en attente en utilisant une chaîne comme déclencheur. Le mode d'attente est libéré lorsqu'un `cast` est effectué sur cette chaîne.

### Exemple 1

```java
// Nous utilisons la chaîne "test" comme déclencheur
thread en_attente() {
    wait("test");
    println("Libéré");
}

// Nous faisons un peu de travail, puis nous libérons notre thread en attente
thread comptage() {
    int nb = 0;
    while (nb < 10000)
        nb++;
    cast("test");
    println("Fin du comptage");
}

en_attente();
comptage();
waitonjoined();
println("Sortie");
```

Exécution :

Si nous exécutons le programme ci-dessus, Tamgu affichera dans l'ordre suivant :

```
Fin du comptage
Libéré
Sortie
```

### Exemple 2

```java
int nb = 1000;

thread decompte() {
    while (nb > 1) {
        nb--;
    }
    printlnerr("Fin du comptage", nb);
    nb--;
}

thread attendre() {
    waitonfalse(nb);
    printlnerr("Ok");
}

attendre();
decompte();
waitonjoined();
printlnerr("Fin");
```

### Mutex : verrouillage et déverrouillage

Il y a des cas où il est nécessaire d'empêcher certains threads d'accéder aux mêmes lignes en même temps, par exemple pour s'assurer que deux appels de fonction sont entièrement appliqués avant qu'un autre thread ne prenne le contrôle. Lorsqu'un verrou est défini dans une fonction donnée, les lignes suivantes de cette fonction ne sont plus accessibles aux autres threads jusqu'à ce qu'un déverrouillage soit appelé.

Exemple :

```java
// Nous implémentons notre thread
thread lancement(string n, int m) {
    int i;
    println(n);
    // Nous affichons toutes nos valeurs
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // Nous lançons notre thread
    lancement("Premier", 2);
    lancement("Second", 4);
}
```

Si nous l'exécutons, nous obtenons un affichage assez aléatoire, car les threads s'exécutent dans un ordre indéterminé, connu uniquement du noyau.

```
Premier
Second
0 1
0 1 2 3
```

Cet ordre peut être imposé avec des verrous, qui empêcheront le noyau d'exécuter le même groupe de lignes en même temps.

Nous devons ajouter des verrous dans le code pour empêcher le système de fusionner les lignes dans une sortie terrible :

```java
// Nous réimplémentons notre thread avec un verrou
thread lancement(string n, int m) {
    lock("lancement"); // Nous verrouillons ici, personne ne peut passer
    int i;
    println(n);
    // Nous affichons toutes nos valeurs
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
    unlock("lancement"); // Nous déverrouillons avec la même chaîne, pour permettre le passage.
}
```

Ensuite, lorsque nous exécutons à nouveau ce morceau de code, nous aurons une sortie complètement différente, ce qui est plus conforme à nos attentes :

```
Premier
0 1
Second
0 1 2 3
```

Cette fois, les lignes s'afficheront selon leur ordre dans le code.

Important : Les chaînes de verrouillage sont globales à l'ensemble du code, ce qui signifie qu'un verrou quelque part peut être déverrouillé ailleurs. Cela signifie également qu'un verrou sur une chaîne donnée peut bloquer une autre partie du code qui utiliserait la même chaîne pour verrouiller ses propres lignes. Il est donc recommandé d'utiliser des chaînes très spécifiques pour différencier un verrou d'un autre.

### Threads protégés

L'exemple ci-dessus aurait pu être réécrit avec exactement le même comportement en utilisant une fonction protégée.

```java
// Nous réimplémentons notre thread en tant que fonction protégée
protected thread lancement(string n, int m) {
    int i;
    println(n);
    // Nous affichons toutes nos valeurs
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}
```

Cette fonction donnera exactement la même sortie que celle ci-dessus. Les threads protégés implémentent un verrou au tout début de l'exécution et le libèrent une fois que la fonction est terminée. Cependant, l'avantage d'utiliser des verrous plutôt qu'une fonction protégée est la possibilité d'être beaucoup plus précis sur les lignes qui doivent être protégées.

### Sémaphores : waitonfalse

Si les fonctions ci-dessus sont utiles dans un contexte multithread, elles ne sont pas suffisantes dans certains cas. Tamgu fournit des fonctions qui sont utilisées pour synchroniser les threads sur les valeurs des variables. Ces fonctions ne peuvent être associées qu'à des types simples tels que Boolean, integer, float ou string.

Le rôle de ces deux fonctions est de faire attendre un thread jusqu'à ce qu'une variable spécifique atteigne une valeur false. False est automatiquement renvoyé lorsque une variable numérique a la valeur 0, lorsque une chaîne est vide, ou lorsque une variable booléenne est définie sur false.

`waitonfalse(var)`: Cette fonction mettra un thread en attente jusqu'à ce que la variable `var` atteigne la valeur false.

Exemple :

```java
// Tout d'abord, nous déclarons une variable stopby
// Important : sa valeur initiale doit être différente de 0
int stopby = 1;

// Nous implémentons notre thread
thread lancement(int m) {
    // Nous réinitialisons stopby avec le nombre de boucles
    stopby = m;
    int i;
    // Nous affichons toutes nos valeurs
    for (i = 0; i < m; i++) {
        print(i, " ");
        // Nous décrémentons notre variable stopby
        stopby--;
    }
}

function principal() {
    // Nous lançons notre thread
    lancement(10);
    // Nous attendons que stopby atteigne 0...
    waitonfalse(stopby);
    println("Fin");
}

principal();
```

L'exécution retardera l'affichage de "Fin" jusqu'à ce que chaque `i` ait été affiché à l'écran.

```
0 1 2 3 4 5 6 7 8 9 Fin
```

Si nous supprimons le `waitonfalse`, la sortie sera assez différente :

```
Fin 0 1 2 3 4 5 6 7 8 9
```

Comme nous pouvons le voir dans cet exemple, Tamgu affichera d'abord le message "Fin" avant d'afficher toutes les autres valeurs. Le `waitonfalse` synchronise `principal` et `lancement` ensemble.

Remarque : L'exemple ci-dessus aurait pu être implémenté avec `wait` et `cast` comme ci-dessous :

```java
// Nous implémentons notre thread
thread lancement(int m) {
    int i;
    // Nous affichons toutes nos valeurs
    for (i = 0; i < m; i++)
        print(i, " ");
    cast("fin");
}

function principal() {
    // Nous lançons notre thread
    lancement(10);
    wait("fin");
    println("Fin");
}

principal();
```

Cependant, il faut se rappeler qu'un seul `cast` peut être effectué à la fois pour libérer les threads. Avec une variable synchrone, le `waitonfalse` peut être déclenché par différents threads, pas seulement celui qui effectuerait un `cast`.

### `waitonjoined()` avec le drapeau `join`

Lorsqu'un thread doit attendre que d'autres threads se terminent avant de continuer, la solution la plus simple est de déclarer chacun de ces threads comme `join` puis d'utiliser la méthode `waitonjoined()`.

Différents threads peuvent attendre sur un ensemble différent de threads joints en même temps.

Exemple :

```java
// Un premier thread avec un join
join thread jaffichage(string s) {
    print(s + "\r");
}

// Qui est lancé à partir de ce thread également "join"
join thread lancement(int x) {
    int i;
    for (i = 0; i < 5000; i++) {
        string s = "Thread:" + x + "=" + i;
        jaffichage(s);
    }
    // Nous attendons que nos threads locaux se terminent
    waitonjoined();
    println("Fin:" + x);
}

// Nous en lançons deux
lancement(0);
lancement(1);
// Et nous attendons qu'ils se terminent...
waitonjoined();
println("Terminaison");
```