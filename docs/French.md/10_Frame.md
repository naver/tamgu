# Frame

La classe `Frame` est utilisée pour déclarer des structures de données complexes avec des fonctions. Elle offre les fonctionnalités suivantes :

- Les variables membres peuvent être instanciées dans la frame.
- Une méthode `_initial` peut être déclarée, qui sera automatiquement appelée pour chaque nouvelle instance de cette frame.
- Une sous-frame peut être déclarée dans le corps de la frame, qui hérite automatiquement des méthodes et variables de la frame supérieure.
- La redéfinition d'une fonction est possible dans une sous-frame.
- Des fonctions et variables privées peuvent également être déclarées dans une frame.

## Exemple

```cpp
frame myframe {
    int i = 10; // chaque nouvelle frame instanciera i avec 10
    string s = "initial";
    
    function display() {
        print("DANS MYFRAME : " + s + "\n");
    }
    
    frame mysubframe {
        function display() {
            print("DANS MYSUBFRAME : " + s + "\n");
        }
    }
}
```

## Utilisation d'une frame

Un objet frame est déclaré avec le nom de sa frame en tant que type.

Exemple :

```cpp
myframe first; // crée une première instance
mysubframe subfirst; // crée une instance de sous-frame

// Nous pouvons recréer une nouvelle instance
first = myframe; // équivalent à "new myframe" en C++ ou Java

// Pour exécuter une fonction de la frame
myframe.display();
```

## Fonction `_initial`

Une fonction créatrice peut être associée à une frame. Cette fonction est automatiquement appelée lorsqu'une nouvelle instance de cette frame est créée.

Exemple :

```cpp
frame myframe {
    int i = 10; // chaque nouvelle frame instanciera i avec 10
    string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function display() {
        print("DANS MYFRAME : " + s + "\n");
    }
}

// Une nouvelle instance de myframe est créée
myframe second(10); // les paramètres sont ensuite transmis à la fonction _initial comme en C++
```

## Fonction `_final`

La fonction `_final` est appelée chaque fois qu'un objet frame est supprimé. En général, un objet déclaré dans une fonction ou dans une boucle est supprimé une fois que cette fonction ou cette boucle se termine.

Important :
- Cette fonction n'a pas de paramètres.
- Un appel à cette fonction ne supprime pas l'objet.
- Le contenu de cette fonction ne peut pas être débogué car elle est appelée depuis le destructeur, indépendamment du reste du code.

Exemple :

```cpp
frame myframe {
    int i = 10; // chaque nouvelle frame instanciera i avec 10
    string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function _final() {
        print("DANS MYFRAME : " + s + "\n");
    }
}

int i = 10;
while (i >= 0) {
    // Une nouvelle instance de myframe est créée
    // À la fin de chaque itération, la fonction _final sera appelée
    myframe item(i);
    i--;
}
```

## Ordre d'initialisation

Lorsque des éléments sont déclarés dans une frame, l'appel à la fonction `_initial` se fait de haut en bas jusqu'à ses enfants. De plus, si un élément dans une frame F est instancié dans la fonction `_initial` de cette frame F, alors cette déclaration prend le pas sur toutes les autres déclarations.

Exemple :

```cpp
// Nous déclarons deux frames
frame within {
    int i;
    
    // Avec une fonction de constructeur spécifique
    function _initial(int j) {
        i = j * 2;
        println("within _initial", i);
    }
}

// Cette frame déclare une frame "within" spécifique
frame test {
    int i;
    
    // Dans ce cas, nous déclarons une frame spécifique, dont la déclaration dépend de la variable i
    within w(i);
    
    // Notre fonction _initial pour cette frame...
    function _initial(int k) {
        i = k;
        println("test _initial", k);
    }
}

// Nous créons une instance de test : t1 avec une valeur initiale de 20
test t1(20);
```

## Création dans le constructeur

Il est possible de créer un élément de frame en déclarant son initialisation directement dans la liste des champs de la frame ou dans le constructeur lui-même. Lorsque la construction de l'élément de frame est effectuée dans le constructeur, une simple déclaration suffit ; toute autre déclaration serait redondante.

Exemple :

```cpp
// Cette frame déclare une frame "within" spécifique
frame test {
    int i;
    
    // Dans ce cas, nous reportons la création effective de l'élément au constructeur : _initial
    within w;
    
    // Notre fonction _initial pour cette frame...
    function _initial(int k) {
        i = k;
        // Nous remplaçons la description précédente par une nouvelle
        // Cette déclaration subsume l'autre ci-dessus
        w = within(100);
        println("test _initial", k);
    }
}

// Nous créons une instance de test : t1 avec une valeur initiale de 20
test t1(20);
```

Important :
Si des paramètres de constructeur sont requis pour "w" et qu'aucune création de cet élément "w" n'est effectuée dans le constructeur, alors Tamgu renverra une erreur indiquant que des paramètres sont manquants.

## Variables communes

Tamgu propose un moyen simple de déclarer des variables de classe. Une variable de classe est une variable dont la valeur est partagée entre toutes les instances d'une frame donnée.

Exemple :

```cpp
frame myframe {
    common int i; // chaque frame aura accès à la même instance commune de cette variable
}

myframe t1;
myframe t2;
t1.i = 10;
t2.i = 15;
println(t1.i, t2.i); // affiche pour les deux variables : 15 15
```

## Fonctions et membres privés

Certaines fonctions ou variables peuvent être déclarées comme privées dans une frame. Une fonction privée ou une variable privée ne peut être accédée que depuis la frame elle-même.

Exemple :

```cpp
frame myframe {
    int i = 10; // chaque nouvelle frame instanciera i avec 10
    
    // variable privée
    private string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    // fonction privée
    private function modify(int x) {
        i = x;
        s = "Modifié avec : " + x; // vous pouvez modifier "s" ici
    }
    
    function display() {
        modify(1000); // vous pouvez appeler "modify" ici
        print("DANS MYFRAME : " + s + "\n");
    }
}

myframe test;

// Instructions illégales sur les membres de frame privés...
test.modify(100); // cette instruction est illégale car "modify" est privée
println(test.s); // cette instruction est illégale car "s" est privée
```

## Sous-framing ou enrichissement d'une frame

Tamgu permet au programmeur d'enrichir ou de sous-framer une frame existante. Une description de frame peut être mise en œuvre en quelques étapes. Par exemple, on peut commencer par une première description puis décider de l'enrichir ultérieurement dans le programme.

### Enrichissement

```cpp
// Nous commençons par une définition limitée d'une frame...
frame myframe {
    int i = 10; // chaque nouvelle frame instanciera i avec 10
}

// Nous ajoutons du code ici après...

// Ensuite, nous enrichissons cette frame avec du code supplémentaire
// Tout ce dont nous avons besoin est d'utiliser la même instruction de frame que ci-dessus, en ajoutant de nouvelles choses
frame myframe {
    function display() {
        println(i);
    }
}
```

### Sous-frames

Une sous-frame est une frame déclarée à l'intérieur d'une autre frame (la frame parent), à partir de laquelle elle hérite à la fois des variables et des fonctions. Une sous-frame peut ensuite remplacer les fonctions de la frame parent par de nouvelles versions et ajouter ses propres variables.

```cpp
// Si nous voulons ajouter des sous-frames...
frame myframe {
    // Nous pouvons maintenant ajouter notre sous-frame...
    frame subframe { ... }
}
```

### Utilisation de la définition supérieure : frame::fonction

Si vous avez besoin d'utiliser la définition de la frame parent à la place du thread actuel, Tamgu fournit un mécanisme très similaire à d'autres langages comme C++ ou Java. Le nom de la fonction doit être précédé du nom de la frame suivi de "::".

Exemple :

```cpp
// Appel de sous-frames...
// Nous définissons une frame de test, dans laquelle nous définissons une frame de sous-test
frame test {
    int i;
    
    function _initial(int k) {
        i = k;
    }
    
    function display() {
        println("Dans test", i);
    }
    
    frame subtest {
        string x;
        
        function display() {
            println("Dans sous-test", i);
            test::display(); // appellera l'autre définition de display depuis test
        }
    }
}

// Nous créons deux objets
test t(1);
subtest st(2);

// Nous appelons ensuite les différentes méthodes
t.display(); // affiche : "Dans test, 1"
st.display(); // affiche : "Dans sous-test, 2" et "Dans test, 2"
st.test::display(); // affiche : "Dans test, 2"
```

## Fonctions système

Les fonctions système sont utilisées pour mapper les différents opérateurs du langage Tamgu sur les méthodes de frame.

### Fonctions de comparaison

Tamgu propose également une façon d'aider à définir des fonctions de comparaison spécifiques entre différents éléments de frame. Ces fonctions ont un nom spécifique, bien qu'elles soient déclenchées par les opérateurs suivants : `>`, `<`, `==`, `!=`, `<=` et `>=`.

Chaque fonction a un seul paramètre qui est comparé avec l'élément courant.

Voici une liste de ces fonctions :

1. `equal` : function ==(frame b);
2. `different` : function !=(frame b);
3. `inferior` : function <(frame b);
4. `superior` : function >(frame b);
5. `inferior equal` : function <=(frame b);
6. `superior equal` : function >=(frame b);

Chacune de ces fonctions doit renvoyer true ou false en fonction de son test.

Exemple :

```cpp
// Implémentation d'un opérateur de comparaison dans une frame
frame comp {
    int k;
    
    // Nous implémentons l'opérateur inférieur
    function <(autre b) {
        if (k < b)
            return true;
        return false;
    }
}

// Nous créons deux éléments
comp one;
comp two;

// one est 10 et two est 20
one.k = 10;
two.k = 20;

// one est inférieur à two et la méthode inférieure ci-dessus est appelée
if (one < two)
    println("OK");
```

### Fonctions arithmétiques

Tamgu offre un mécanisme pour implémenter des fonctions spécifiques pour les différents opérateurs numériques. Ces fonctions doivent avoir deux opérandes, sauf pour `++` et `--`. Elles doivent renvoyer un élément de la même frame que ses arguments.

1. `plus` : function +(frame a, frame b);
2. `minus` : function -(frame a, frame b);
3. `multiply` : function *(frame a, frame b);
4. `divide` : function /(frame a, frame b);
5. `power` : function ^^(frame a, frame b);
6. `shift left` : function <<(frame a, frame b);
7. `shift right` : function >>(frame a, frame b);
8. `mod` : function %(frame a, frame b);
9. `or` : function |(frame a, frame b);
10. `xor` : function ^(frame a, frame b);
11. `and` : function &(frame a, frame b);
12. `++` : function ++();
13. `--` : function --();

Exemple :

```cpp
frame test {
    int k;
    
    function ++() {
        k++;
    }
    
    // Il est important de créer une nouvelle valeur, qui est retournée par la fonction
    function +(test a, test b) {
        test res;
        res.k = a.k + b.k;
        return res;
    }
}

test a, b, c;
c = a + b; // Nous appellerons alors notre implémentation de plus ci-dessus.
```

### Fonctions de nombre et de chaîne de caractères

Vous pouvez également interpréter un objet frame comme une chaîne de caractères ou un nombre dans une expression arithmétique, par exemple. Dans ce cas, vous pouvez implémenter des fonctions avec le nom du type d'objet que vous souhaitez que votre instance de frame soit interprétée comme.

Exemple :

```cpp
frame test {
    int k;
    string s;
    
    // Nous retournons une instance de "test" en tant que chaîne de caractères...
    function string() {
        return s;
    }
    
    // Nous retournons une instance de "test" en tant que float...
    function float() {
        return k;
    }
    
    // Nous retournons une instance de "test" en tant qu'entier...
    function int() {
        return k;
    }
}
```

### Intervalles et index

Il est également possible d'utiliser un objet frame comme un vecteur ou une map. Il est alors possible d'accéder aux éléments via un intervalle ou de définir une valeur via un index. Pour utiliser un objet de cette manière, le développeur doit implémenter les fonctions suivantes :

1. function [](self idx, self value) : Cette fonction insère un élément dans un vecteur à la position idx.
2. function [](self idx) : Cette fonction renvoie la valeur à la position idx.
3. function [:](self left, self right) : Cette fonction renvoie les valeurs entre les positions left et right.

Exemple :

```cpp
frame myvect {
    vector kj;
    
    // Cette fonction insère une valeur dans le vecteur à la position idx
    function [](int idx, self value) {
        kj[idx] = value;
    }
    
    // Cette fonction renvoie la valeur à la position idx
    function [](int idx) {
        return kj[idx];
    }
    
    // Cette fonction renvoie la valeur entre l et r.
    function [:](int l, int r) {
        return kj[l:r];
    }
}

myvect test;
test[0] = 10; // Nous appelons la fonction [](...)
test[1] = 5; // Nous appelons la fonction [](...)
// Nous appelons la fonction [:](...)
println(test[0], test[1], test[0:]); // Affichage : 10 5 [10, 5]
```