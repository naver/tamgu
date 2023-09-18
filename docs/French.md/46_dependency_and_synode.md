# Dépendances et Synodes

Les dépendances sont un objet linguistique fondamental qui est devenu essentiel dans le traitement automatique du langage naturel moderne. Tamgu offre une implémentation spécifique de ces dépendances basée sur le moteur de prédicats. L'objectif de cette implémentation est de prendre en entrée la sortie d'un analyseur de dépendances (comme l'analyseur Stanford) et d'effectuer des analyses supplémentaires dessus.

Les dépendances sont évaluées comme des prédicats qui relient des nœuds syntaxiques. Notre système fournit un deuxième type appelé synode, qui représente un nœud dans un arbre de constituants.

## Synode

Un synode est un nœud syntaxique défini par un ensemble de caractéristiques (implémenté comme une mapss) et sa position dans l'arbre de constituants. Un synode possède les méthodes suivantes :

1. _initial(map m) : Crée un nœud syntaxique avec les caractéristiques données.
2. addchild(synode) : Ajoute un nœud enfant.
3. addnext(synode) : Ajoute un nœud suivant.
4. addprevious(synode) : Ajoute un nœud précédent.
5. after(synode) : Renvoie vrai si le nœud se trouve après le nœud donné sous le même parent.
6. attributes() : Renvoie les attributs de caractéristiques sous forme de vecteur.
7. before(synode) : Renvoie vrai si le nœud se trouve avant le nœud donné sous le même parent.
8. child() : Renvoie le premier nœud enfant ou vérifie s'il correspond au paramètre.
9. children() : Renvoie la liste des enfants d'un nœud donné ou vérifie si le nœud est un enfant.
10. definitions(mapss) : Définit les définitions de caractéristiques valides pour tous les synodes.
11. last() : Renvoie le dernier nœud enfant ou vérifie s'il correspond au paramètre.
12. nbchildren() : Renvoie le nombre d'enfants directs.
13. next(synode) : Renvoie le nœud suivant ou vérifie s'il correspond au paramètre.
14. parent() : Renvoie le nœud parent ou vérifie s'il correspond au paramètre.
15. previous(synode) : Renvoie le nœud précédent ou vérifie s'il correspond au paramètre.
16. precede(synode) : Renvoie vrai si le nœud se trouve avant (n'importe où dans l'arbre).
17. sibling(synode) : Vérifie si le nœud est un frère (soit une sœur, soit un descendant).
18. sisters(synode) : Renvoie la liste des nœuds sœurs ou vérifie si le nœud est un nœud sœur.
19. succeed(synode) : Renvoie vrai si le nœud se trouve après (n'importe où dans l'arbre).
20. test(string attribute) : Vérifie si un attribut fait partie de la structure de caractéristiques.
21. values() : Renvoie les valeurs des caractéristiques sous forme de vecteur.

## Création d'un arbre de constituants

Un arbre de constituants est construit de haut en bas. Lors de l'utilisation de la fonction `addchild`, elle ajoute un nœud enfant sous le nœud actuel, et chaque appel ultérieur à cette fonction ajoute un nouvel enfant après le précédent.

Exemple :

```cpp
// Nous créons notre nœud NP
synode np({"pos": "np"});

// Puis trois nœuds lexicaux
synode det({"pos": "det", "surface": "the"});
synode adj({"pos": "adj", "surface": "big"});
synode noun({"pos": "noun", "surface": "dog"});

// Nous les ajoutons sous np, un après l'autre
np.addchild(det);
np.addchild(adj);
np.addchild(noun);

// Nous affichons les nœuds de manière indentée
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 4);
    Display(x.next(), i);
}

Display(np, 0);
```

Résultat :
```
#0['pos': 'np']
    #0['pos': 'det', 'surface': 'the']
    #0['pos': 'adj', 'surface': 'big']
    #0['pos': 'noun', 'surface': 'dog']
```

Notez le `#0`, qui indique que le synode n'est pas une variable de dépendance.

## Type de dépendance

Une dépendance est une relation entre deux synodes. Les dépendances peuvent être créées directement en utilisant le type `dependency`, qui peut ensuite être stocké dans la base de connaissances avec `assertz`, ou avec une règle de dépendance. Une dépendance est composée d'un nom, d'un ensemble de caractéristiques et d'une liste d'arguments.

Méthodes :
1. _initial([name, features, arg1, arg2, ...]) : Crée une dépendance avec un nom (une chaîne de caractères), un ensemble de caractéristiques (une mapss) et une liste d'arguments, chacun étant de type synode.
2. features() : Renvoie les caractéristiques de la dépendance.
3. name() : Renvoie le nom de la dépendance.
4. rule() : Renvoie l'ID de la règle qui a créé cette dépendance.

Exemple :

```cpp
// Nous créons deux nœuds lexicaux
synode det({"pos": "det", "surface": "the"});
synode noun({"pos": "noun", "surface": "dog"});

dependency d(["DET", {"direct": "+"}, det, noun]);

// Nous l'ajoutons à la base de connaissances
assertz(d);

println(d);
```

Résultat :
```
DET['direct': '+']({"pos": "det", "surface": "the"}, {"pos": "noun", "surface": "dog"})
```

## Règle de dépendance

Une règle de dépendance suit le modèle suivant :

```
If ([^|~] dep[features] (#x[features], #y) and/or dep(#w, #z)...)
    depres(#n, #nn), ..., depres(#n, #nn) / ~ / #x[..], #x[..].
```

Dans cette règle, `x`, `y`, `w`, `z`, `n`, `nn` sont des entiers qui représentent des objets synode. La règle fait correspondre les dépendances dans la base de connaissances aux dépendances stockées dans la règle. Si une correspondance est trouvée, de nouvelles dépendances sont créées en utilisant les mêmes variables.

La règle peut mélanger des appels de fonctions et des prédicats avec des dépendances. Le symbole `^` indique que la dépendance sera modifiée. Une seule dépendance peut être modifiée à la fois dans une règle. Le symbole `~` représente la négation. S'il est placé devant une dépendance, cela signifie que la dépendance ne doit pas être présente dans la base de connaissances.

Si la sortie de la règle est remplacée par `~`, la règle s'appliquera, mais aucune dépendance ne sera créée.

Fait :
La façon la plus simple d'ajouter une dépendance à la base de connaissances est de l'insérer en tant que fait :

```
dep[features](#1, #2).
```

## Caractéristiques

La structure de caractéristiques dans une règle de dépendance suit des règles spécifiques :
- Les guillemets sont facultatifs autour des attributs et des valeurs.
- Le symbole `+` est la valeur par défaut pour les attributs avec une seule valeur.

Opérateurs :
- `attribute` : Vérifie l'existence de l'attribut.
- `attribute: value` : Compare l'attribut avec la valeur.
- `attribute: ~` : L'attribut ne doit avoir aucune valeur.
- `attribute ~: value` : L'attribut ne doit pas avoir la valeur spécifiée.
- `attribute = value` : Attribue à l'attribut la valeur spécifiée.
- `attribute = ~` : Supprime l'attribut de la structure de caractéristiques.
- `attribute -: gram` : Compare l'attribut avec une expression régulière TREG (Tamgu Regular Expression).
- `attribute ~ -: gram` : Compare l'attribut avec une expression régulière TREG, qui doit échouer.

Stop :
Le prédicat `stop` peut être utilisé pour arrêter l'évaluation d'une grammaire. Lorsque le moteur d'évaluation rencontre ce prédicat, il arrête d'évaluer la grammaire plus loin, en sautant les règles restantes.

```cpp
If (stop) ~.
```

_dependencies() :
Cette méthode déclenche une analyse de dépendance en appliquant des règles à la base de connaissances.

_setvalidfeatures(mapss features) :
Cette méthode définit des contraintes sur les caractéristiques valides pouvant être utilisées pour les synodes et les dépendances. Les caractéristiques sont définies sous forme de paires attribut/valeur dans une map. Si un attribut peut prendre n'importe quelle valeur, comme le lemme d'un mot, la valeur doit être une chaîne vide. La valeur par défaut est `+`.

Exemple :

```cpp
mapss feats = {'Obl': '+', 'lemma': '', 'c_person': '+', 'CR4': '+', 'Punct': '+', 'surface': ''};
_setvalidfeatures(feats);
```

## Exemple

```cpp
// Nous affichons les nœuds de manière indentée
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 5);
    if (i) // lorsque i==0, c'est la racine de notre arbre, nous ne voulons pas afficher ses sœurs
        Display(x.next(), i);
}

// Nous préparons notre arbre de constituants
synode np1 = {"bar": 2};
synode np2({"bar": 2});
synode vp({"bar": 2});
synode s({"bar": 3});
synode v({"word": "eats", "pers": 3, "pres": "+", "verb": "+"});
synode d1({"word": "the", "det": "+"});
synode n1({"word": "dog", "noun": "+"});
synode d2({"word": "a", "det": "+"});
synode n2({"word": "bone", "noun": "+"});

s.addchild(np1);
s.addchild(vp);
vp.addchild(v, np2);
np1.addchild(d1, n1);
np2.addchild(d2, n2);

// Il est possible d'ajouter ou de modifier des caractéristiques existantes comme si un synode était une map
vp["pos"] = "verb";
np1["pos"] = "noun";
np2["pos"] = "noun";

// Nos dépendances initiales
subj(v, n1).
obj(v, n2).
det(n1, d1).
det(n2, d2);

// Nous pouvons également le créer d'une manière différente
dependency dpe(["mydep", {}, n1, n2]);

// Mais ensuite, nous devons l'ajouter à la base de connaissances nous-mêmes
assertz(dpe);

// Cette fonction est appelée à partir d'une règle ci-dessous. Le #x devient un objet synode.
// La fonction renvoie vrai pour éviter que la règle échoue.
function DTree(synode n) {
    Display(n, 0);
    println("---------------------------");
    return true;
}

// Une règle simple qui inverse les nœuds
If (subj(#1, #2)) inverted(#2, #1).

// Une règle qui utilise des contraintes sur les nœuds
If (subj(#1[pres, pers: 3], #2) and obj(#1, #3)) arguments(#2, #1, #3).

// Nous ajoutons des caractéristiques à une dépendance
If (^subj(#1, #2)) subj[direct=+](#1, #2).

// Nous utilisons _ pour parcourir toutes les dépendances avec deux arguments, avec une contrainte que deux nœuds sont différents
If (_(#1, #2) && obj(#1, #3) && #2 != #3) link(#2, #3).

// Nous utilisons les variables de dépendance _1 et _2 pour éviter de créer une dépendance entre les mêmes arguments
If (_1(#1, #2) && obj_2(#1, #3) && _1 != _2) other(#2, #3).

// Nous marquons un nœud à travers une règle de dépendance, et nous pouvons également utiliser des contraintes dans la structure
If (subj(#1, #2) and obj(#1, #3) and #2[noun:+, subject=+]) ~.

// Nous pouvons également écrire cette règle en utilisant des guillemets
If (subj(#1, #2) and obj(#1, #3) and #3["object"] = "+") ~.

// Dans ce cas, nous accédons au parent du nœud #1 en utilisant le prédicat p_parent
// Ensuite, nous appelons DTree pour l'afficher... DTree doit renvoyer vrai, sinon la règle échouera
// Le #3 est automatiquement transformé en un objet synode lorsque la fonction est appelée
If (det(#1, #2) and p_parent(#1, #3) and DTree(#3)) ~.

// Nous utilisons un TREG comme contrainte dans notre règle
If (obj(#1[word -: "e%a+"], #2)) Verb(#1).

// Nous lançons notre analyseur de dépendances
_dependencies();

// Nous rassemblons toutes les dépendances dans la base de connaissances
vector res = predicatedump();

Display(s, 0);
println("---------------------------");
printjln(res);
```

Résultat :
```
s
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
---------------------------
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
#0['bar': '3']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
#0['bar': '2', 'pos': 'verb']
    #0['word': 'eats', 'pers': '3', 'pres': '+', 'verb': '+']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
other({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
subj['direct': '+']({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"word": "dog", "noun": "+", "subject": "+"})
inverted({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
obj({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
det({"word": "dog", "noun": "+", "subject": "+"}, {"word": "the", "det": "+"})
det({"object": "+", "word": "bone", "noun": "+"}, {"word": "a", "det": "+"})
arguments({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
link({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
Verb({"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
```