# Moteur d'inférence

Tamgu intègre un moteur d'inférence, qui peut être librement mélangé avec des instructions Tamgu régulières. Ce moteur d'inférence est très similaire à Prolog mais avec quelques particularités :

a) Les prédicats n'ont pas besoin d'être déclarés au préalable pour que Tamgu distingue les prédicats des fonctions normales. Cependant, si vous avez besoin d'utiliser un prédicat qui sera implémenté ultérieurement dans le code, vous devez le déclarer au préalable.

b) Vous n'avez pas besoin de déclarer des variables d'inférence. Cependant, leurs noms sont très différents des noms traditionnels de Prolog : ils doivent être précédés d'un "?".

c) Chaque clause d'inférence se termine par un "." et non par un ";".

d) Les termes peuvent être déclarés au préalable (en tant que variables de terme). Cependant, si vous ne souhaitez pas les déclarer, vous devez précéder leur nom d'un "?" comme pour les variables d'inférence.

e) Des probabilités peuvent être attachées aux prédicats, qui sont utilisées pour choisir en premier le chemin avec les probabilités les plus élevées.

**Note** : Pour une description adéquate du langage Prolog, veuillez consulter la documentation appropriée.

## Types

Tamgu expose trois types spécifiques pour les objets d'inférence :

### Prédicat

Ce type est utilisé pour déclarer des prédicats, qui seront utilisés dans les clauses d'inférence. Ce type expose les méthodes suivantes :

1. `name()`: renvoie l'étiquette du prédicat
2. `size()`: renvoie le nombre d'arguments
3. `_trace(bool)`: active ou désactive la trace pour ce prédicat lorsqu'il est appelé.

### Terme

Ce type est utilisé pour déclarer des termes, qui seront utilisés dans les clauses d'inférence (voir l'exemple NLP ci-dessous).

### Autres types d'inférence : Liste et Map Associatif

Tamgu fournit également les listes traditionnelles à la Prolog, qui peuvent être utilisées avec l'opérateur "|" pour gérer la décomposition des listes (voir l'exemple NLP ci-dessous pour une démonstration de cet opérateur).

**Exemple :**

```tamgu
prédicat alist;
// dans cette clause, C est le reste de la liste...
alist([?A,?B|?C],[?A,?B],?C) :- true.
v=alist([1,2,3,4,5], ?X,?Y);
println(v); // [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

Tamgu fournit également une map associative, qui est implémentée comme une map Tamgu, mais dans laquelle l'ordre des arguments est significatif.

**Exemple :**

```tamgu
prédicat assign,avalue;
avalue(1,1) :- true.
avalue(10,2) :- true.
avalue(100,3) :- true.
avalue("fin",4) :- true.
assign({?X:?Y,?Z:?V}) :- avalue(?X,1), avalue(?Y,2), avalue(?Z,3), avalue(?V,4).
vector v=assign(?X);
println(v); // [assign({'100':'fin','1':10})]
```

Comme vous pouvez le voir dans cet exemple, les clés et les valeurs peuvent dépendre de variables d'inférence. Cependant, l'ordre dans lequel ces clés:valeurs d'association sont déclarées est important. Ainsi, {?X:?Y,?Z:?V} est différent de {?Z:?V,?X:?Y}.

### PredicateVar

Ce type est utilisé pour gérer les prédicats afin d'explorer leurs noms et valeurs. Un PredicateVar peut être vu comme une fonction, dont les paramètres sont accessibles par leur position dans la liste d'arguments sous forme de vecteur. Ce type expose les méthodes suivantes :

1. `map()`: renvoie le prédicat sous forme de map : [name:name,'0':arg0,'1':arg1,...]
2. `name()`: renvoie le nom du prédicat
3. `query(predicate|name,v1,v2,v3)`: construit et évalue un prédicat à la volée
4. `remove()`: supprime le prédicat de la mémoire
5. `remove(db)`: supprime le prédicat de la base de données db
6. `size()`: renvoie le nombre d'arguments
7. `store()`: stocke le prédicat en mémoire
8. `store(db)`: stocke la valeur du prédicat dans la base de données db
9. `vector()`: renvoie le prédicat sous forme de vecteur : [name,arg0,arg1,...]

Il convient de noter que la méthode "predicate", qui existe à la fois pour une map et un vecteur, transforme le contenu d'un vecteur ou d'une map en prédicat tant que leur contenu imite la sortie du prédicat de vector() et map().

**Exemple :**

```tamgu
vector v=['female','mary'];
predicatevar fem;
fem=v.predicate(); // nous transformons notre vecteur en prédicat
fem.store(); // nous le stockons dans la base de faits
v=fem.query(female,?X); // Nous construisons une requête de prédicat à la volée
v=fem.query(female,'mary'); // Nous construisons une requête de prédicat avec une chaîne de caractères
```

## Clauses

Une clause est définie comme suit :

```tamgu
prédicat(arg1,arg2,...,argn) :- pred(arg...),pred(arg,...), etc. ;
```

Fait dans une base de connaissances. Un fait peut être déclaré dans un programme avec l'instruction suivante :

```tamgu
prédicat(val1,val2,...).
```

ou

```tamgu
prédicat(val1,val2,...) :- true.
```

est en réalité équivalent à la syntaxe ci-dessus.

**loadfacts(chemin d'accès)** : chargement d'une grande base de connaissances

Si vous disposez d'un fichier contenant une très grande base de connaissances, ne contenant que des faits, il est plus efficace d'utiliser `loadfacts` avec le chemin d'accès au fichier pour accélérer le chargement. Cette instruction doit être placée au tout début de votre fichier. Elle commencera à charger au moment de la compilation.

**Disjonction**

Tamgu accepte également les disjonctions dans les clauses avec l'opérateur ";", qui peut être utilisé à la place de "," entre les prédicats.

**Exemple :**

```tamgu
prédicat mere,pere;
mere("jeanne","marie").
mere("jeanne","rolande").
pere("bertrand","marie").
pere("bertrand","rolande").
prédicat parent;
// Nous déclarons ensuite notre règle en tant que disjonction...
parent(?X,?Y) :- mere(?X,?Y);pere(?X,?Y).
parent._trace(true);
vector v=parent(?X,?Y);
println(v);
```

**Résultat :**

```
r:0=parent(?X,?Y) --> parent(?X6,?Y7)
e:0=parent(?X8,?Y9) --> mere(?X8,?Y9)
k:1=mere('jeanne','marie').
success:2=parent('jeanne','marie')
k:1=mere('jeanne','rolande').
success:2=parent('jeanne','rolande')
[parent('jeanne','marie'),parent('jeanne','rolande')]
```

**Couper, Échouer et Arrêter**

Tamgu fournit également un couper, qui est exprimé avec le traditionnel "!". Il fournit également le mot-clé fail, qui peut être utilisé pour forcer l'échec d'une clause. Stop arrête toute l'évaluation.

**Fonctions**

Tamgu fournit également certaines fonctions régulières du langage Prolog telles que :

**Fonction asserta(pred(...))**

Cette fonction affirme (insère) un prédicat au début de la base de connaissances. Notez que cette fonction ne peut être utilisée que dans une déclaration de clause.

**assertz(pred(...))**

Cette fonction affirme (insère) un prédicat à la fin de la base de connaissances. Notez que cette fonction ne peut être utilisée que dans une déclaration de clause.

**retract(pred(...))**

Cette fonction supprime un prédicat de la base de connaissances. Notez que cette fonction ne peut être utilisée que dans une déclaration de clause.

**retractall(pred)**

Cette fonction supprime toutes les instances du prédicat "pred" de la base de connaissances. Si retractall est utilisé sans aucun paramètre, alors il nettoie toute la base de connaissances. Notez que cette fonction ne peut être utilisée que dans une déclaration de clause.

**Fonction : predicatedump(pred) or findall(pred)**

Cette fonction, lorsqu'elle est utilisée sans aucun paramètre, renvoie tous les prédicats stockés en mémoire sous forme de vecteur. Si vous fournissez le nom d'un prédicat en tant que chaîne de caractères, alors il renvoie sous forme de vecteur tous les prédicats avec le nom spécifié.

**Exemple :**

```tamgu
// Notez que vous devez déclarer "parent" si vous voulez l'utiliser dans un assert
predicate parent;
adding(?X,?Y) :- asserta(parent(?X,?Y)).
adding("Pierre","Roland");
println(predicatedump(parent));
```

**Nom de prédicat universel**

Lors de la recherche de faits dans une base de connaissances, il est possible de faire une requête sans nom de prédicat spécifique. Dans ce cas, on peut utiliser : "_" à la place du nom de prédicat.

**Exemple :**

```tamgu
// Notre base de connaissances
father("george","sam").
father("george","andy").
mother("andy","mary").
mother("sam","christine").
// Notre règle, elle correspondra à tous les faits ci-dessus
parent(?A,?B) :- _(?A,?B).
```

Nous pouvons également utiliser certaines variables spécifiques : _0.._9, qui peuvent renvoyer le nom du prédicat avec lequel il a correspondu.

**Exemple :**

```tamgu
// Notre règle, elle correspondra à tous les faits ci-dessus
parent(?A,?B,?P) :- _1(?A,?B), ?P is _1.
```

Ce code renvoie :

```
parent("andy","mary","mother")
parent("sam","christine","mother")
parent("george","sam","father")
parent("george","andy","father")
```

**Récursion terminale**

La récursion terminale est un mécanisme qui transforme une récursion en un processus itératif. Pour activer le mécanisme de récursion terminale, vous devez modifier le nom du dernier élément de la règle en ajoutant un "#". La récursion terminale est alors activée si ce dernier élément correspond à la règle actuelle.

La récursion terminale transforme le parcours d'un vecteur, par exemple, en un processus itératif. Cependant, à la fin du processus, la valeur initiale du vecteur est perdue.

**Exemple :**

```tamgu
vector v = [1..10];
// Fin de la récursion
traverse([],0).
// Le dernier élément est marqué pour la récursion terminale
traverse([?X|?Y],?A) :- println(?X,?Y), traverse#(?Y,?A).
vector vv = traverse(v,?A);
println(vv);
```

**Résultat :**

```
1 [2,3,4,5,6,7,8,9,10]
2 [3,4,5,6,7,8,9,10]
3 [4,5,6,7,8,9,10]
4 [5,6,7,8,9,10]
5 [6,7,8,9,10]
6 [7,8,9,10]
7 [8,9,10]
8 [9,10]
9 [10]
10 [] // Le premier argument est maintenant []
[traverse([],0)]
```

**Fonction de rappel**

Un prédicat peut être déclaré avec une fonction de rappel, dont la signature est la suivante :

```tamgu
function OnSuccess(predicatevar p, string s) {
    println(s,p);
    return true;
}
```

Le deuxième argument de la fonction correspond au paramètre donné à parent dans la déclaration. Si la fonction renvoie true, alors le moteur d'inférence essaie d'autres solutions ; sinon, il s'arrête.

**Résultat :**

Si nous exécutons notre exemple ci-dessus, nous obtenons :

```
Parent: parent('John','Mary')
Parent: parent('John','Peter')
```

**DCG**

Tamgu accepte également les règles DCG (Definite Clause Grammar) avec quelques modifications par rapport à la définition originale. Premièrement, les variables Prolog doivent être indiquées avec ?V comme dans les autres règles. Troisièmement, les atomes ne peuvent être déclarés que sous forme de chaînes de caractères.

**Exemple :**

```tamgu
prédicat sentence,noun_phrase,verb_phrase;
term s,np,vp,d,n,v;
sentence(s(np(d("the"),n("cat")),vp(v("eats"),np(d("a"),n("bat"))))) --> [].
```

**Lancement d'une évaluation**

Les évaluations sont lancées exactement de la même manière qu'une fonction. Vous pouvez bien sûr fournir autant de variables d'inférence que vous le souhaitez, mais vous ne pouvez lancer qu'un seul prédicat à la fois, ce qui impose que votre expression soit d'abord déclarée comme une clause si vous voulez qu'elle inclue plus d'un prédicat.

**Important** : Si la variable destinataire est un vecteur, alors toutes les analyses possibles seront fournies. L'arbre d'évaluation sera entièrement traversé. Si la variable destinataire est autre chose, alors chaque fois qu'une solution est trouvée, l'évaluation s'arrête.

**Mappage des méthodes en prédicats**

La plupart des méthodes d'objet sont mappées en prédicats de manière très simple. Par exemple, si une chaîne de caractères exporte la méthode "trim", alors un "p_trim" avec deux variables est créé. Chaque méthode est mappée en un prédicat de cette manière. Pour chaque méthode, nous ajoutons un préfixe : "p_" pour transformer cette méthode en prédicat. Le premier argument de ce prédicat est l'objet principal de la méthode, tandis que le dernier paramètre est le résultat de l'application de cette méthode à cet objet. Ainsi, si s est une chaîne de caractères, s.trim() devient p_trim(s,?X), où ?X est le résultat de l'application de trim à s. Si ?X est unifié, alors le prédicat vérifiera si ?X est identique à s.trim().

**Exemple :**

```tamgu
compute(?X,?Y) :- p_log(?X,?Y).
between(?X,?B,?E), succ(?X,?Y).
```

**Erreurs courantes avec les variables Tamgu**

Si vous utilisez des variables courantes dans les prédicats, telles que la chaîne de caractères s, l'entier s, ou tout autre type de variables, vous devez vous rappeler que ces variables sont utilisées dans les prédicats comme des valeurs de comparaison. Un exemple pourrait clarifier un peu ce que nous voulons dire.

**Exemple 1**

```tamgu
string s="test";
string sx="other";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

Exécution :

```
r:0=comp(s,3) --> comp(other,?X172) --> Fail
```

Cette clause a échoué car s et sx ont des valeurs différentes.

**Exemple 2**

```tamgu
string s="test"; // maintenant ils ont les mêmes valeurs
string sx="test";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

Exécution :

```
r:0=comp(s,3) --> comp(test,?X173)
e:0=comp(test,3) --> println(s)test
success:1=comp('test',3)
```

Faites attention lorsque vous concevez vos clauses pour utiliser des variables externes comme sources de comparaison et non comme instantiation. Si vous voulez passer une valeur de chaîne de caractères à votre prédicat, alors le paramètre correspondant à cette chaîne doit être une variable de prédicat.

**Exemple 3**

```tamgu
string sx="test";
predicate comp;
comp._trace(true);
comp(?s,3) :- println(?s).
comp(sx,?X);
```

Exécution :

```
r:0=comp(?s,3) --> comp(test,?X176)
e:0=comp('test',3) --> println(?s177:test)test
success:1=comp('test',3)
```

**Exemples**

**Tour de Hanoi**

Le programme suivant résout le problème de la tour de Hanoi pour vous.

```tamgu
predicate move;
move(1,?X,?Y,_) :- println('Move the top disk from ',?X,' to ',?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
predicatevar res;
res=move(3,"left","right","centre");
println(res);
```

**Ancêtre**

Avec ce programme, vous pouvez trouver l'ancêtre féminin commun entre les relations parentales de différentes personnes.

```tamgu
predicate ancestor,parent,male,female,test;
ancestor(?X,?X) :- true.
ancestor(?X,?Z) :- parent(?X,?Y),ancestor(?Y,?Z).
parent("george","sam") :- true.
parent("george","andy") :- true.
parent("andy","mary") :- true.
male("george") :- true.
male("sam") :- true.
male("andy") :- true.
female("mary") :- true.
test(?X,?Q) :- ancestor(?X,?Q), female(?Q).
test._trace(true);
vector v=test("george",?Z);
println(v);
```

**Exemple NLP**

Cet exemple correspond aux clauses qui ont été générées à partir de la grammaire DCG précédente donnée en exemple.

```tamgu
predicate sentence,noun_phrase,det,noun,verb_phrase,verb;
term P,SN,SV,dét,nom,verbe;
sentence(?S1,?S3,P(?A,?B)) :- noun_phrase(?S1,?S2,?A), verb_phrase(?S2,?S3,?B).
noun_phrase(?S1,?S3,SN(?A,?B)) :- det(?S1,?S2,?A), noun(?S2,?S3,?B).
verb_phrase(?S1,?S3,SV(?A,?B)) :- verb(?S1,?S2,?A), noun_phrase(?S2,?S3,?B).
det(["the"|?X],?X,dét("the")) :- true.
det(["a"|?X],?X,dét("a")) :- true.
noun(["cat"|?X],?X,nom("cat")) :- true.
noun(["dog"|?X],?X,nom("dog")) :- true.
verb(["eats"|?X],?X,verbe("eats")) :- true.
vector v;
v=sentence(?X,[],?A);
println("All the sentences that can be generated:",v);
// nous analysons une phrase
v=sentence(["the","dog","eats","a","bat"],[],?A);
println("The analysis:",v);
```

**Tour de Hanoi animée**

Le code ci-dessous affiche une animation dans laquelle les disques sont déplacés d'une colonne à une autre. Il fusionne à la fois les graphiques et les prédicats.

```tamgu
predicate move;
map columns={'left':[70,50,30],'centre':[],'right':[]};
function disk(window w,int x,int y,int sz,int position) {
    int start=x+100-sz;
    int level=y-50*position;
    w.rectanglefill(start,level,sz*2+20,30,FL_BLUE);
}
function displaying(window w,self o) {
    w.drawcolor(FL_BLACK);
    w.font(FL_HELVETICA,40);
    w.drawtext("Left",180,200);
    w.drawtext("Centre",460,200);
    w.drawtext("Right",760,200);
    w.rectanglefill(200,300,20,460,FL_BLACK);
    w.rectanglefill(100,740,220,20,FL_BLACK);
    w.rectanglefill(500,300,20,460,FL_BLACK);
    w.rectanglefill(400,740,220,20,FL_BLACK);
    w.rectanglefill(800,300,20,460,FL_BLACK);
    w.rectanglefill(700,740,220,20,FL_BLACK);
    vector left=columns['left'];
    vector centre=columns['centre'];
    vector right=columns['right'];
    int i;
    for (i=0;i<left;i++)
        disk(w,100,740,left[i],i+1);
    for (i=0;i<centre;i++)
        disk(w,400,740,centre[i],i+1);
    for (i=0;i<right;i++)
        disk(w,700,740,right[i],i+1);
}
window w with displaying;
function moving(string x,string y) {
    columns[y].push(columns[x][-1]);
    columns[x].pop();
    w.redraw();
    pause(0.5);
    return true;
}
move(1,?X,?Y,_) :- moving(?X,?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
thread hanoi() {
    move(3,"left","right","centre");
}
function launch(button b,self o) {
    hanoi();
}
button b with launch;
w.begin(50,50,1000,800,"HANOI");
b.create(20,20,60,30,FL_Regular,FL_NORMAL_BUTTON,"Launch");
w.end();
w.run();
```