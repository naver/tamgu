# Fonction, Autorun, Thread

En Tamgu, une fonction est déclarée en utilisant le mot-clé `function`, suivi de son nom et de ses paramètres. Une valeur peut être renvoyée en utilisant le mot-clé `return`. Les paramètres sont toujours envoyés en tant que valeurs, sauf si le type est `self`. Il convient de noter qu'une fonction ne fournit aucune information de type pour sa valeur de retour.

## 7.1 Imposer un type de retour

Une fonction peut spécifier un type de retour en utilisant le symbole `::` après la liste des arguments. Par exemple:

```
function toto(int i) :: int {
    i += 10;
    return i;
}
```

Dans cet exemple, la fonction `toto` est déclarée pour renvoyer un `int`.

## 7.2 Autorun

Une fonction `autorun` est automatiquement exécutée après sa déclaration. Les fonctions autorun ne sont exécutées que dans le fichier principal. Si vous avez des fonctions autorun dans un fichier appelé depuis un autre fichier, ces fonctions ne seront pas exécutées.

Exemple:

```
autorun waitonred() {
    println("Chargement:", _paths[1]);
}
```

## 7.3 Thread

Lorsqu'une fonction `thread` est lancée, elle est exécutée dans un thread système indépendant.

Exemple:

```
thread toto(int i) {
    i += 10;
    return i;
}

int j = toto(10);
print("J=" + j + "\n");
```

Sortie:

```
J=20
```

### Thread protégé

Un thread `protected` empêche deux threads d'accéder aux mêmes lignes de code en même temps. Un thread protégé définit un verrou au début de son exécution, qui est libéré une fois que la fonction est exécutée. Cela garantit que les appels différents à une fonction protégée seront effectués séquentiellement et non simultanément. Protected doit être utilisé pour le code qui n'est pas réentrant.

Exemple:

```
// Nous implémentons un thread synchronisé
protected thread launch(string n, int m) {
    int i;
    println(n);
    // Nous affichons toutes nos valeurs
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // Nous lançons notre thread
    launch("Premier", 2);
    launch("Second", 4);
    println("Fin");
}

principal();
```

Sortie:

```
Fin
Premier
0 1
Second
0 1 2
```

### Thread exclusif

Un thread `exclusive` est similaire à un thread protégé, mais avec une différence. Dans le cas d'un thread protégé, la protection est au niveau de la méthode, tandis qu'avec un thread exclusif, elle est au niveau du cadre. Cela signifie que les threads exclusifs ne permettent qu'à un seul thread d'avoir accès à l'objet de cadre à la fois.

Exemple:

```
// Ce cadre expose deux méthodes
frame disp {
    // Exclusif
    exclusive thread edisplay(string s) {
        println("Exclusif:", s);
    }
    // Protégé
    protected thread pdisplay(string s) {
        println("Protégé:", s);
    }
}

// Nous implémentons également un cadre de tâche
frame task {
    string s;
    // Avec une instance "disp" spécifique
    disp cx;

    function _initial(string x) {
        s = x;
    }

    // Nous appelons notre instance locale avec protected
    function pdisplay() {
        cx.pdisplay(s);
    }

    // Nous appelons notre instance locale avec exclusive
    function edisplay() {
        cx.edisplay(s);
    }

    // Nous appelons l'instance globale avec exclusive
    function display(disp c) {
        c.edisplay(s);
    }
}

// L'instance commune
disp c;
vector v;
int i;
string s = "T";

for (i = 0; i < 100; i++) {
    s = "T" + i;
    task t(s);
    v.push(t);
}

// Dans ce cas, l'affichage sera ordonné car protected n'est pas réentrant
// Seul un pdisplay peut s'exécuter à la fois
for (i = 0; i < 100; i++)
    v[i].pdisplay();

// Dans ce cas, l'affichage sera un mélange de tous les edisplay travaillant en parallèle
// Puisque exclusive ne protège que les méthodes à l'intérieur d'une seule instance, et que nous avons différentes
// instances dans ce cas...
for (i = 0; i < 100; i++)
    v[i].edisplay();

// Dans ce dernier cas, nous avons un seul objet "disp" commun partagé par toutes les "task"
// L'affichage sera à nouveau ordonné comme avec protected, car cette fois-ci, nous tombons sur la même
// instance "c disp".
for (i = 0; i < 100; i++)
    v[i].display(c);
```

### Joint et Waitonjoined

Un thread peut être déclaré comme joint, ce qui signifie que le thread principal attendra l'achèvement de tous les threads qui ont été lancés avant de terminer son propre code. Vous pouvez utiliser `waitonjoined()` pour attendre que ces threads se terminent. Vous pouvez utiliser autant de `waitonjoined()` que nécessaire dans différents threads. `waitonjoined()` n'attend que les threads joints qui ont été lancés dans un thread donné.

### Valeurs atomiques

La plupart des structures de données (maps, vectors, strings, etc.) sont protégées dans les threads contre l'accès concurrentiel grâce à un verrou. Cependant, ces verrous peuvent être coûteux en termes de temps et d'espace. Pour remédier à cela, Tamgu fournit des structures de données sans verrouillage (types atomiques) tels que `a_int`, `a_string` ou `a_mapii`. Ces structures de données permettent une utilisation plus efficace de la machine, car les threads accédant à ces types ne sont pas mis en attente. Cependant, leur implémentation les rend plus lents que les autres types dans un contexte non threadé. Ils sont utiles uniquement lorsqu'ils sont partagés entre les threads.

### Opérateur de flux '<<<'

Lorsque vous lancez un thread, le résultat de ce thread ne peut pas être directement stocké dans une variable en utilisant l'opérateur `=` car un thread vit sa propre vie sans aucun lien avec le code appelant. Tamgu fournit un opérateur spécifique pour cette tâche: `<<<`, également connu sous le nom d'opérateur de flux. Un flux est une variable qui est connectée au thread de telle manière que les valeurs renvoyées par le thread peuvent être stockées dans cette variable. Bien sûr, la variable doit exister indépendamment du thread.

Exemple:

```cpp
// Nous créons un thread en tant que thread "join", afin de pouvoir utiliser waitonjoined.
// Ce thread renvoie simplement 2 * x
joined thread Test(int i) {
    return (i * 2);
}

// Notre fonction de lancement, qui lancera 10 threads
function launch() {
    // Nous déclarons d'abord notre variable de stockage de map dans cette fonction
    treemapi m;
    int i = 0;

    // Nous lançons ensuite 10 threads et stockons le résultat de chacun dans m à une position spécifique
    for (i in <0,10,1>)
        m[i] <<< Test(i);

    // Nous attendons que tous les threads se terminent
    waitonjoined();

    // Nous affichons notre valeur finale
    println(m); // {0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```

## 7.4 Définitions multiples

Tamgu permet de définir plusieurs fonctions portant le même nom mais différant dans leur définition de paramètre. Par exemple, vous pouvez implémenter une `display(string s)` et une `display(int s)`. Dans ce cas, le système choisira la fonction la plus appropriée en fonction de la liste des arguments de l'appel de fonction.

Exemple:

```cpp
function testmultipledeclaration(string s, string v) {
    println("String:", s, v);
}

function testmultipledeclaration(int s, int v) {
    println("Int:", s, v);
}

// Nous déclarons nos variables
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// Dans ce cas, le système choisira la bonne fonction en fonction de sa liste d'arguments
testmultipledeclaration(s1, s2); // L'implémentation de la chaîne de caractères
testmultipledeclaration(i, j); // L'implémentation de l'entier
```

## 7.5 Arguments par défaut

Tamgu permet de déclarer des arguments par défaut dans une fonction. Vous pouvez définir une valeur pour un paramètre qui peut être omis lors de l'appel.

Exemple:

```cpp
function acall(int x, int y = 12, int z = 30, int u = 43) {
    println(x + y + z + u);
}

acall(10, 5); // Le résultat est: 88 = 10 + 5 + 30 + 43
```

Remarque: seuls les derniers paramètres dans une liste de déclaration peuvent être facultatifs.

## 7.6 Drapeaux spécifiques: Private & Strict

Les fonctions peuvent également être déclarées avec deux drapeaux spécifiques: `private` et `strict`.

### Private

Lorsqu'une fonction est déclarée comme `private`, elle ne peut pas être accédée depuis un fichier Tamgu externe. Si un programme Tamgu est chargé à partir d'un autre programme Tamgu, les fonctions privées sont inaccessibles depuis le chargeur.

Exemple:

```cpp
// Cette fonction est invisible depuis les chargeurs externes
private function toto(int i) {
    i += 10;
    return i;
}
```

### Strict

Par défaut, lorsqu'une fonction est déclarée en Tamgu, le langage essaie de convertir chaque argument de la fonction appelante en les paramètres attendus par l'implémentation de la fonction. Cependant, dans certains cas, un contrôle de paramètre plus strict est nécessaire. Le drapeau `strict` permet de résoudre ce problème en demandant un contrôle strict des paramètres pour une fonction.

Exemple:

```cpp
strict function teststrictdeclaration(int s, int v) {
    println("Int:", s, v);
}

// Nous déclarons nos variables
string s1 = "s1";
string s2 = "s2";

// Dans ce cas, le système échouera à trouver la bonne fonction pour ces paramètres
// et renverra un message d'erreur
teststrictdeclaration(s1, s2); // Aucune implémentation de chaîne de caractères
```