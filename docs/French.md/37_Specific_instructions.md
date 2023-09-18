# Instructions spécifiques à Tamgu

Tamgu fournit toutes les opérations nécessaires pour gérer différents algorithmes, y compris les déclarations if, else, elif, switch, for et while. Il prend également en charge les opérateurs booléens tels que and (&&), or (||) et xor. Ces opérateurs booléens combinent différents éléments avec une valeur booléenne. Les opérateurs and et or peuvent également être écrits comme && et ||, respectivement.

- L'opérateur `and` réussit si tous les éléments sont vrais.
- L'opérateur `or` réussit si au moins un élément est vrai.
- L'opérateur `xor` réussit si au moins un élément est vrai mais échoue s'ils sont tous vrais.

## Instructions if-elif-else

Les instructions if-elif-else dans Tamgu sont utilisées pour effectuer une branche conditionnelle. La syntaxe est la suivante :

```python
if (expression_booléenne) {
    // bloc de code
}
elif (expression_booléenne) {
    // bloc de code
}
...
else {
    // bloc de code
}
```

## Instruction ifnot

L'instruction `ifnot` dans Tamgu est utilisée pour chaîner différentes instructions et renvoie la valeur de la première instruction qui renvoie autre chose que false, null ou vide. Elle est couramment utilisée pour gérer des séquences complexes d'opérations.

Exemple :

```python
map m1 = { "a": 1, "b": 2, "c": 3 };
map m2 = { "A": 4, "B": 5, "C": 6 };
map m3 = { "aa": 7, "bb": 8, "cc": 9 };
int i = (m1["A"] ifnot m2["B"] ifnot m3["bb"]) + 24;
println(i); // Sortie : 29

function calling(string s) {
    return (m1[s] ifnot m2[s] ifnot m3[s]);
}

println(calling('bb')); // Sortie : 8
```

## Instruction switch

L'instruction `switch` dans Tamgu est utilisée pour répertorier une série de tests pour un seul objet. La syntaxe est la suivante :

```python
switch (expression) (avec fonction) {
    v1: {
        // bloc de code
    }
    v2: {
        // bloc de code
    }
    default: {
        // bloc de code
    }
}
```

Les `v1`, `v2`, etc. peuvent être soit une chaîne de caractères, soit un entier, soit un nombre à virgule flottante. L'expression est évaluée une fois et comparée à `v1`, `v2`, etc. Il est également possible de remplacer la simple comparaison entre les éléments par un appel à une fonction, qui doit renvoyer `true` ou `false`.

Exemple :

```python
// Nous testons si une valeur est supérieure à l'autre
function tst(int i, int j) {
    if (j >= i)
        return true;
    return false;
}

int s = 10;

// Nous testons à travers test
switch (s) avec tst {
    1: println("1");
    2: println("2");
    20: println("20"); // Ce sera l'occurrence sélectionnée
}
```

## Instructions for

Il existe différentes variantes de boucles `for` dans Tamgu. Voici une présentation de chacune d'entre elles.

### `for (expression; booléen; suivant)`

Cette boucle `for` est composée de trois parties : une initialisation, une expression booléenne et une partie de continuation. La syntaxe est la suivante :

```python
for (expression; booléen; suivant) {
    // bloc de code
}
```

Vous pouvez utiliser `continue` ou `break` pour passer à l'élément suivant ou interrompre la boucle en cours.

Exemple :

```python
for (i = 0; i < 10; i += 1)
    print("I=", i, "\n");
```

### `for (var dans conteneur)`

Ce type de boucle `for` est utilisé pour itérer sur un conteneur, tel qu'une chaîne de caractères ou un fichier. La syntaxe est la suivante :

```python
for (var dans conteneur) {
    // bloc de code
}
```

Exemple :

```python
// Boucle à travers un fichier
file f('myfile.txt', "r");
string s;
for (s dans f)
    println(s);

// Boucle à travers un vecteur d'entiers
vector v = [1, 2, 3, 4, 5, 6];
int i;
for (i dans v)
    println(l);
```

### `for (i dans <début, fin, incrémentation>)`

Ce type de boucle `for` est une boucle rapide qui est environ 30% à 50% plus rapide que son équivalent. Chaque valeur de la plage peut être instanciée à l'aide de variables. Cependant, une fois la boucle commencée, aucun élément ne peut être modifié, y compris la variable qui reçoit les différentes valeurs. La syntaxe est la suivante :

```python
for (i dans <début, fin, incrémentation>) {
    // bloc de code
}
```

Exemple :

```python
int i, j = 1;
int v;
time t1;

// Boucle de 0 à 100000, avec une incrémentation de 1.
for (i dans <0, 100000, j>)
    v = i;

time t2;
float diff = t2 - t1;
println("Temps écoulé pour le 'for' rapide :", diff);

time t3;
for (i = 0; i < 100000; i += j)
    v = i;

time t4;
diff = t4 - t3;
println("Temps écoulé pour le 'for' normal :", diff);
```

## Instruction while

L'instruction `while` dans Tamgu est composée d'une seule expression booléenne. La syntaxe est la suivante :

```python
while (booléen) {
    // bloc de code
}
```

Vous pouvez utiliser `continue` ou `break` pour passer à l'élément suivant ou interrompre la boucle en cours.

Exemple :

```python
int i = 10;
while (i > 0) {
    print("I=", i, "\n");
    i -= 1;
}
```

## Instruction do-while

L'instruction `do-while` dans Tamgu est similaire à `while`, mais la première itération est effectuée avant le test booléen. La syntaxe est la suivante :

```python
do {
    // bloc de code
}
while (booléen);
```

Exemple :

```python
int i = 10;
do {
    print("I=", i, "\n");
    i -= 1;
}
while (i > 0);
```

## Identifiant unique universel : `uuid()`

La fonction `uuid()` dans Tamgu renvoie un UUID (Identifiant unique universel) conforme à la norme UUID RFC 4122, version 4 (entièrement aléatoire). Elle renvoie une valeur de chaîne de caractères contenant des valeurs hexadécimales.

Exemple :

```python
string s = uuid(); // Exemple de sortie : 80c67c4d-4c4d-14cd-e58d-eb3364cd
```

## Évaluation : `eval(string code)`

La fonction `eval()` dans Tamgu peut évaluer et exécuter du code Tamgu à la volée. Le résultat de l'évaluation est renvoyé en fonction de ce qui a été évalué.

## Évaluation JSON : `evaljson(string code)`

La fonction `evaljson()` dans Tamgu prend une chaîne JSON en entrée et la compile en un vecteur ou une map en fonction de la structure interne.

## Instructions d'affichage

Tamgu fournit plusieurs instructions d'affichage pour afficher les résultats sur le port d'affichage actuel. Les instructions `print` et `println` sont utilisées pour afficher les résultats, tandis que les instructions `printerr` et `printlnerr` affichent les résultats sur la sortie d'erreur standard. Les versions `println` et `printlnerr` ajoutent deux fonctionnalités à la sortie : un espace supplémentaire est ajouté pour les valeurs séparées par une virgule, et un retour chariot est ajouté à la fin de la ligne.

## Instructions d'affichage jointes

Les instructions `printj`, `printjln`, `printjerr` et `printjlnerr` sont utilisées pour afficher des valeurs de conteneurs qui sont "jointes" au préalable. Elles acceptent deux, trois ou quatre arguments. Le premier paramètre doit être un conteneur, et le deuxième doit être une chaîne de séparation. Si le conteneur est une map, un séparateur de clé peut également être fourni en tant que troisième paramètre. Un quatrième paramètre numérique peut être fourni pour ajouter un retour chariot tous les n valeurs.

Exemple :

```python
ivector v = [1..10];
printj(v, "-"); // Sortie : 1-2-3-4-5-6-7-8-9-10

map m = {1: 2, 2: 3, 4: 5, 6: 7};
printjln(m, ",", "-"); // Sortie : 1-2,2-3,4-5,6-7
```

## Fonctions de redirection d'E/S

Les fonctions `ioredirect` et `iorestate` sont utilisées pour capturer la sortie d'erreur ou la sortie standard dans un fichier. La fonction `ioredirect` redirige soit stderr (si `err` est vrai), soit stdout (si `err` est faux) vers un fichier. Elle renvoie un ID, qui est utilisé pour rétablir la sortie normale à l'aide de la fonction `iorestate`.

Exemple :

```python
int o = ioredirect('C:\Tamgu\test\test.txt', true);
printlnerr("Cette chaîne est maintenant stockée dans le fichier : test.txt");
iorestate(o, true); // Retour à la normale
```

## Fonctions de pause et de sommeil

Les fonctions `pause` et `sleep` sont utilisées pour mettre un thread en pause ou en mode de sommeil. La fonction `pause` ne suspend pas l'exécution d'un thread, tandis que la fonction `sleep` le fait. La fonction `pause` prend un nombre à virgule flottante en entrée, représentant la durée de la pause en secondes. Elle peut également prendre un deuxième paramètre booléen pour afficher une petite animation. La fonction `sleep` est basée sur l'instruction de sommeil de l'OS et son comportement dépend de l'implémentation locale. Elle prend un entier en entrée, représentant la durée de sommeil en secondes.

Exemple :

```python
pause(0.1); // Le thread fera une pause de 10 ms
pause(2, true); // Le thread fera une pause de 2s, avec une petite animation
sleep(1); // Le thread dormira pendant 1s (selon la plateforme)
```

## Fonction Emojis

La fonction `emojis()` renvoie une liste de tous les caractères d'emoji conformément à la norme v5.0 beta.

## Fonction de distance GPS

La fonction `GPSdistance` dans Tamgu est utilisée pour calculer la distance entre deux points GPS donnés leur longitude et leur latitude. Le cinquième paramètre est facultatif et représente le rayon de la Terre, avec une valeur par défaut de 6371 km. Cette valeur définit également l'unité utilisée pour calculer la distance finale.

Exemple :

```python
float d = GPSdistance(46.9001, 7.1201, 47.01, 6.1);
// Sortie : d est de 78,382 km
```

## Lire depuis le clavier : `kget()`

La fonction `kget()` dans Tamgu est utilisée pour lire une chaîne de caractères depuis le clavier.

Exemple :

```python
string message = kget("Message :");
```

## Variables persistantes

Vous pouvez créer des variables persistantes dans le contexte d'une interface graphique pour suivre certaines valeurs lors du lancement répété de vos programmes. Les types persistants suivants sont disponibles : `ithrough`, `fthrough`, `uthrough`, `sthrough`, `vthrough` et `mthrough`. Ces variables ne sont jamais réinitialisées entre les exécutions.

Exemple :

```python
// Cette variable gardera une trace du nombre de fois que ce programme a été exécuté
ithrough icount;
icount += 1;
println(icount);
```