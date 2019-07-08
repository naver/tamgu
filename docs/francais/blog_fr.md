# Programmation des données

Aujourd'hui, grâce aux algorithmes de Machine Learning, la reconnaissance vocale ou la traduction automatique sont entrées dans la vie de millions de gens. Hélas, pour que ces algorithmes puissent fonctionner de façon optimale, il faut leur fournir des données annotées, beaucoup de données annotées et structurées. 

Ce problème est au coeur de la plupart des équipes d'Intelligence Artificielle à la travers le monde. Comment disposer de suffisamment de données pour entrainer des modèles efficaces et fiables d'apprentissage machine ?

## Tamgu (탐구)

En coréen, _탐구_  (tamgu) signifie _quête, recherche, exploration_.  

Le langage a été conçu pour simplifier la création de nouveaux corpus.

D'abord, Tamgu est un langage moderne qui combine en un seul formalisme, la programmation impérative, fonctionnelle et logique. 

* Programmation impérative: multithreaded, proche de Python dans l'esprit mais avec un typage explicite des variables, Tamgu vous offre tout l'éventail d'un langage de programmation riche et concis.

* Programmation fonctionnelle: inspirée de Haskell, Tamgu vous permet de construire des lambda-fonctions extrêmement compactes.

* Programmation logique: inspirée de Prolog, Tamgu vous permet de construire des routines logiques que vous pouvez intégrer dans votre code.

L'interopérabilité est totale. Un vecteur Tamgu reste le même vecteur qu'il soit utilisé en programmation fonctionnelle ou logique.

## Bibliothèques

Tamgu est écrit en C++. Le langage est conçu de telle façon qu'il est très simple de créer de nouvelles bibliothèques. 

D'ailleurs, Tamgu vient déjà avec de nombreuses bibliothèques, telles que cURL (pour manipuler des pages Web), SQLite (pour manipuler des base de données), FLTK (pour gérer des interfaces graphiques) ou AO et SNDFILE (pour jouer des fichiers audio).

Tamgu fournit aussi un outil pour vous permettre de créer rapidement de nouvelles bibliothèques, de façon à enrichir le langage avec vos propres algorithmes.

## Règles et Lexiques

Tamgu offre aussi la possibilité de créer des lexiques sous la forme de transducteur pouvant contenir des millions de mots aussi bien pour les langues occidentales que les langues asiatiques.

Ces lexiques peuvent être combinées avec des règles quasi-syntaxiques qui peuvent être rajoutées dans le code de façon transparente. Encore une fois l'interopérabilité est maintenue. Grâce à ces règles vous pouvez facilement isoler des motifs récurrents dans vos textes, afin de les annoter automatiquement.

### Apprentissage machine

Tamgu offre aussi quelques bibliothèques d'apprentissage machine pour manipuler des embeddings ou pour appliquer des classificateurs ou des CRFs sur vos données. Ainsi, les règles ci-dessus peuvent par exemple intégrer des capsules basées sur les embeddings pour isoler des mots proches.

## Corpus bruité

Tamgu permet de créer des corpus sur mesure pour vos modèles d'apprentissage. Ainsi, dans le cadre de la campagne d'évaluation en traduction automatique: WMT 2019, nous avons utilisé Tamgu pour construire un corpus bruité. Dans un premier temps, nous avons repéré automatiquement des mots mal écrit dans un corpus de commentaires, en utilisant un lexique du français. Puis nous avons pris un corpus propre et nous avons remplacé automatiquement certains mots par leur versions erronées que nous avions isolés au préalable.


# Exemples

La programmation de base en Tamgu n'est pas difficile. Nous avons essayé de garder un formalisme proche de celui dont les gens ont l'habitude. Cepandant, il faut noter que le langage impose un typage explicite comme en Java ou en C, tout en offrant de nombreuses conversions implicites entre les types. 

```C++

int i=10;
string s='20';

s+=i; //s is now 2010
i+=s; //i is now 30 (automatic conversion of a type into another)

vector v=[1,2,3,4,5,6];

s =  v; //s is now "[1,2,3,4,5,6]"

vector vv = s; // s is converted back into a vector which is stored in vv.

```
Le langage offre de nombreux types de conteneurs, dictionnaires, vecteurs. Il offre aussi la possibilité de créer des classes et leurs dérivations. 

## Fonctions, Frames, Threads

Tamgu offre aussi le moyen d'écrire des fonctions et des classes (appelée 'frame' dans Tamgu). 

```C++

function callme(int i, string s) {...}

frame test {
    int i;
    string s;
    
    //creator
    function _initial(int e, string u) {
        i = e;
        s = u;
    }
    
    function test() {
        return i;
    }
}

//we create an element
test toto(10,"value");
```

Tamgu est aussi un langage multi-threadé. Chaque objet dans Tamgu est _systématiquement_ protégé pour une utilisation potentielle dans des threads.
Tamgu fournit des _lock_, la notion de _join_. 

```C++

//A thread is simply a thread...
joined thread callme(int i, string s) {
    println(i,s);
}

//We call a thread as a normal function...
callme(1,"t1");
callme(2,"t2");
callme(3,"t3");
callme(4,"t4");
callme(5,"t5");
callme(6,"t6");

//We wait for all threads to end...
waitonjoined();

```

Vous pouvez encore transformer n'importe quelle variable en une variable de _thread_:

```C++

long i=1000;

//a simple thread that wait on i to be 0...
thread callme() {
    waitonfalse(i);
    println("Ok");
}

callme();

//We then decrement i... When i is 0, the thread is unlocked...
for (int e = 0; e <2000; e++)
    i--;

```

## La gestion des chaines de caractère

Tamgu fournit un impressionnant arsenal pour manipuler vos chaînes de caractères. Tout d’abord, il reconnait de façon dynamique l’encodage d’une chaîne, même si celle-ci mêle par accident des encodages différents. 

Il existe en Tamgu une multitude de façon d’accéder au contenu d’une chaine. 

* Avec des indexes :                        _str[1], str[2 :4], str[-2 :]_
* Avec des sous-chaines :               _str["beg" :"end"]_
* Avec des expressions régulières : _str[r"ab%d+"]_

Vous pouvez aussi enchaîner les descriptions: _str["a":"e"][1:-1]_

Mais surtout, vous pouvez modifier le contenu d'une chaine de cette façon:

```C++
string s="The dog is <eating> a bone.";

s["<":">"][1:-1] = "biting"; //the string is now: The dog is <biting> a bone.

The first part isolates the substring between '<..>', 
then removes them to focus on the content of
this substring and replaces it with a new value.

```

### Lambdas

Les lambdas en Tamgu sont écrites dans un formalisme largement emprunté à _Haskell_. Elles vous permettent de décrire de façon très compacte des manipulations complexes sur les conteneurs et les chaines de caractère en particulier. Les méthodes les plus communes de Haskell sont toutes accessibles: _map, filter, fold, scan , take, drop,  takeWhile, dropWhile, zip, repeat and cycle_ . Vous pouvez aussi enchainer plusieurs méthodes avec l'opérateur ".". Enfin, le langage offre la possibilité d'écrire des fonctions que vous pouvez appeler depuis votre code. 

```Haskell

vector v = [1..10]; //v contains [1,2,3,4,5,6,7,8,9,10]
vector vres;

vres = <x | x <- v, (x < 5)>; //we only keep elements inferior to 5
vres = <filter (<5)  v>; //Same...

vres = <map (+1) v>; //vres is [2,3,4,5,6,7,8,9,10,11]

vres = <takeWhile (<15) . map (*2) v>; 
//we can chain methods: vres is[2,4,6,8,10,12,14], we stop at 14, since 2*8 = 16 > 15

```

#### Une fonction pour trier des éléments

```Haskell

v.shuffle(); // we shuffle the elements in the vector...

<fastsort([]) = []>;  //if the list is empty, we return an empty "list"
<fastsort([fv:v]) =  mn &&& fv &&& mx where //we merge the different list...
    let mn = fastsort(<filter (<=fv) v>), //we apply our "sort" on the list that contains the elements smaller than fv
    let mx = fastsort(<filter (>fv) v>) //we apply our "sort" on the list that contains the elements larger than fv
>;
    
//we can call a functional definition as regular code
vres=fastsort(v); 

```


### Prolog

Tamgu offre enfin le moyen d'exécuter du code Prolog directement dans votre propre code:

```Prolog

concat([],?X,?X).
concat([?H|?T],?Y, [?H|?Z]) :- concat(?T,?Y,?Z).

//You can mix freely your predicate definition with some regular tamgu code
v=concat(['english','russian','french'],['spanish'],?L);
println(v); //['english','russian','french','spanish']

```

## Programmation des données

Tamgu a été conçu pour la _programmation des données_. Par exemple, vous pouvez directement intégrer des règles dans votre code et détecter dans un texte des mots ou des patterns de mots en une seule opération.

```C++

//We define some lexical rules (starting with a "@")
@food <- burger.
@food <- tartare.


//Our rule: if a "food" word is found in a sentence, then we return a "meal" label 
meal <- "the", #food.

//We need a specific object to scan a sentence
annotator r;

//a sentence
string sentence="Here, the burger and the tartare are delicious."
vector v = r.parse(sentence); 

//Result: v =  [['meal',[10,16]],['meal',[25,32]]]
//It reads: two 'meal' were found at position 10-16 and position 25-32...

```

Nous avons utilisé ce mécanisme pour générer une entrée _bruitée_ pour entraîner notre modèle pour le _workshop_ WMT 2019 pour la tâche _traduction robuste_. 

### Génération

Vous pouvez aussi générer vos propres phrases, grâce au module DCG de notre Prolog...

```Prolog

sentence(s(?NP,?VP)) --> noun_phrase(?NP), verb_phrase(?VP).
noun_phrase(np(?D,?N)) --> det(?D), noun(?N).
verb_phrase(vp(?V,?NP)) --> verb(?V), noun_phrase(?NP).
det(d("the")) --> ["the"].
det(d("a")) --> ["a"].

noun(n("bird")) --> ["bird"].
noun(n("cat")) --> ["cat"].
verb(v("catches")) --> ["catches"].

//sentence._trace(true);
//we generate all possible interpretations...
vector vr=sentence(?Y,[],?X);
printjln(vr);

```
Ce qui nous donne toutes les variations suivantes :

```Prolog
sentence(['the','bird','catches','the','bird'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['the','bird','catches','the','cat'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['the','bird','catches','a','bird'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['the','bird','catches','a','cat'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('a'),n('cat')))))
sentence(['the','cat','catches','the','bird'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['the','cat','catches','the','cat'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['the','cat','catches','a','bird'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['the','cat','catches','a','cat'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('a'),n('cat')))))
sentence(['a','bird','catches','the','bird'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['a','bird','catches','the','cat'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['a','bird','catches','a','bird'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['a','bird','catches','a','cat'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('a'),n('cat')))))
sentence(['a','cat','catches','the','bird'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['a','cat','catches','the','cat'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['a','cat','catches','a','bird'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['a','cat','catches','a','cat'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('a'),n('cat')))))

```

Notez que nous avons construit un arbre syntaxique pour chaque phrase générée.

