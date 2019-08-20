# TAMGU

Aujourd'hui, grâce aux algorithmes d'Apprentissage Machine, la reconnaissance vocale ou la traduction automatique sont entrées dans la vie de millions de gens. Hélas, pour que ces algorithmes puissent fonctionner de façon optimale, il faut leur fournir des données annotées, beaucoup de données annotées et structurées.
Ce problème est au coeur de la plupart des équipes d'Intelligence Artificielle à la travers le monde. Comment disposer de suffisamment de données pour entrainer des modèles efficaces et fiables d'apprentissage machine ?
 
L’une des solutions est de générer ces corpus automatiquement grâce à des outils sur mesure. Christopher Ré (PR à Stanford) a d’ailleurs donné un nom à cette approche: le Data Programming. On oppose désormais les corpus annotés à la main par des êtres humains aux corpus créés semi-automatiquement part des moyens artificiels: Gold Standards vs Silver Standards. 
Si le groupe de Christopher Ré a produit quant à lui son propre ensemble d’outils (Snorkel), nous avons quant à nous décidé d’adresser ce problème sous un angle très différent. 

## Grammaires formelles

Mon principal axe de recherche depuis le début de mes études est le domaine des grammaires formelles. Pendant de nombreuses années, j’ai participé à la construction d’un moteur de règles qui a été largement utilisé dans des campagnes d’évaluation ou encore des projets européens et français. Mais, le monde de la recherche étant ce qu’il est, ces méthodes dites symboliques ont peu à peu perdu de leur intérêt au bénéfice d’approches statistiques. Jusqu’à l’arrivée du Data Programming. Par un étrange paradoxe, si les règles linguistiques du passé ont perdu de leur lustre, le Data Programming leur a donné une nouvelle jeunesse. Car détecter des patterns de mots, c’est appliquer des règles linguistiques, certes beaucoup plus pauvres que celles que nous manipulions il y a 10 ans. Mon expérience dans la fabrication de moteur de règles a trouvé là un nouvel espace d’étude. En revanche, cette fois, au lieu d’un outil spécialisé, j’ai décidé de créer un langage de programmation le plus ouvert possible, offrant tout ce qu’un langage moderne se doit d’offrir avec une gestion intégrée de règles lexicales puissantes.

## Tamgu

Tamgu est né de ces réflexions. Il est aussi né de mes discussions avec les gens du monde de l'Apprentissage Machine pour identifier les problèmes les plus fréquents et la façon de les résoudre. En quelques mots, de façon très simplifiée, l'Apprentissage Machine a besoin de corpus annotés dans lesquels des patterns récurrents de mots sont identifiés et étiquetés. Il a encore besoin de corpus synthétiques dans lesquels des données textuelles sont générées en suivant des grammaires spécifiques. En particulier, en traduction automatique, il peut être très intéressant de disposer de corpus bruités, des corpus où l’on introduit de façon artificielle des erreurs connues de façon à ce que le modèle puisse apprendre à les reconnaitre et à les traduire correctement.

### FIL

Ainsi donc, un tel langage doit à la fois fournir de quoi annoter un corpus, mais aussi de quoi générer de nouveaux textes, le tout en offrant l’éventail le plus riche possible d’instructions en tout genre pour éviter de restreindre l’utilisateur dans son travail.

Tamgu est un langage moderne qui appartient à une classe particulière de langages de programmation: les langages FIL: Fonctionnel, Impératif et Logique.

La majorité des langages sont aujourd’hui sont des langages IF, Impératif avec une touche de Fonctionnel comme peuvent l’être Kotlin ou Swift. D’autres sont franchement fonctionnels comme  Haskell ou Lisp. Enfin, certains sont purement Logiques comme Prolog.

### Fonctionnel

Or le choix d’un langage FIL pose des problèmes particuliers d’interopérabilité. Pourtant, il suffit d’examiner les objets utilisés par toutes ces approches pour découvrir un point commun: ils manipulent peu ou prou les mêmes objets: des nombres, des chaines de caractères et des conteneurs. Aussi, je me suis librement inspiré de Haskell pour la partie fonctionnelle, qui s’est révélé le langage idéal pour écrire des lambdas fonctions compactes et efficaces. Je l’ai adapté en autorisant par exemple le langage à manipuler directement des dictionnaires Tamgu (des maps) ou encore à itérer sur des variables extérieures.

```C++

//Nous créons: [1,2,3,4,5,6,7,8,9,10] 
vector v = [1..10];

//Le ‘<...>’ identifie une expression Haskell
vector vv = <map (+1) v>; //Nous ajoutons 1 à chaque élément

```

## Prédicats

Prolog a été longtemps le langage de choix pour construire des générateurs de texte. La syntaxe du langage permet d’écrire en quelques lignes des grammaires riches et complexes. L’une des limitations de Prolog est par exemple la difficulté à gérer de gros lexiques, ce que l’intégration dans un environnement FIL permet de résoudre en déportant cette gestion vers des objets adaptés. Pour la partie logique, il a fallu là aussi que je modifie légèrement la syntaxe du langage pour le rendre interopérable avec le reste de Tamgu. Par exemple, les variables sont identifiées dans Prolog par une majuscule au début de leur nom, tandis que les mots en minuscule sont considérés comme des atomes immutables, ce qui pose problème dans le cadre d’un langage FIL qui n’a aucune de ces restrictions. Je me suis donc inspiré de la syntaxe de SPARQL où les variables dans les expressions logiques commencent avec un « ? ». Ainsi, en identifiant différemment les variables, j’ai pu remplacer les atomes par des chaînes de caractère. Quant aux vecteurs Tamgu, ils sont ré-interprétés de façon transparente comme des vecteurs Prolog. Plus exactement, les vecteurs Tamgu possèdent désormais la capacité de s’unifier dans le cadre d’une exécution à la Prolog. L’unification par défaut est par ailleurs ramené à une simple comparaison d’égalité entre deux objets, ce qui permet alors d’utiliser n’importe quel objet Tamgu dans une expression logique. 

```PROLOG

concat([],?X,?X).
concat([?H|?T],?Y, [?H|?Z]) :- concat(?T,?Y,?Z).

//L'appel aux prédicats se fait comme un appel de fonction dans le code
v=concat(['english','russian','french'],['spanish'],?L);
println(v); //['english','russian','french','spanish']

```
 
## Base du langage

La partie « impérative » du langage est composé des modules traditionnels de la plupart des langages existants. Nous pouvons déclarer des variables, des fonctions, des threads (et même des micro-threads), des classes. 

```C++
function appelmoi(int i, string s) {...}

thread monthread(int i, string u) {...}

frame maclasse {...}
```

A la différence de Python, le langage exige en revanche que les variables soient déclarés avec un type. J’ai aussi tenté de simplifier la manipulation des threads en protégeant de façon implicite toutes les variables potentiellement en danger (principalement les conteneurs). Tamgu offre un éventail très riche de types différents: strings, floats, integers, vectors, maps. 

```C++

int i = 10;
float f = 1.234;
 
string s = "The dog is <eating> a bone.";

vector v = [1,2,3];

map m = {‘a’:1, ‘b’:2};

``` 

### La gestion des chaines de caractère

En particulier, Tamgu fournit un impressionnant arsenal pour manipuler vos chaînes de caractères. Tout d’abord, il reconnait de façon dynamique l’encodage d’une chaîne, même si celle-ci mêle par accident des encodages différents.
Il existe en Tamgu une multitude de façon d’accéder au contenu d’une chaine.

* Avec des indexes : str[1], str[2 :4], str[-2 :]
* Avec des sous-chaines : str["beg" :"end"]
* Avec des expressions régulières : str[r"ab%d+"]

Vous pouvez aussi enchaîner les descriptions: str["a":"e"][1:-1]
Mais surtout, vous pouvez modifier le contenu d'une chaine de cette façon:

```C++
string s="The dog is <eating> a bone.";

s["<":">"][1:-1] = "biting"; //la chaîne contient maintenant: The dog is <biting> a bone.

La première partie isole la sous-chaîne entre '<..>' et la remplace par "biting"
```
## Lexiques et Règles

Enfin et surtout, Tamgu offre un mécanisme de lexiques basés sur les transducteurs. Les transducteurs sont la solution la plus efficace pour encoder un lexique. Ils offrent à la fois compacité et vitesse d’accès. La version que j’ai implémenté permet en plus d’identifier un mot en parcourant le transducteur avec une distance d’édition. De cette façon, il est possible de reconnaitre des mots comprenant des erreurs courantes telles que la commutation de deux caractères, l’absence d’un caractère ou au contraire la présence d’une caractère surnuméraire. 

Mais surtout, ces lexiques peuvent être couplés avec des règles hors-contextes, lesquelles peuvent être _écrites directement dans le code_. Il devient dès lors possible de décrire votre propre vocabulaire, d’ajouter au besoin des lexiques généraux de l’anglais ou du français puis d’écrire des règles pour identifier des patterns complexes. Dans l’exemple qui suit, nous définissons quelques mots de vocabulaire auxquels nous associons l’étiquette _food_. Puis nous créons une simple règle qui détecte la séquence _the food_. Nous créons un _annotator_ qui va être automatiquement associé à ces règles et nous l’appliquons sur notre phrase.

```C++
//Nous définissions nos règles lexicales (commençant avec un "@")
@food <- burger.
@food <- tartare.


//Notre règle: si un mot de type "food" est trouvé dans la phrase, on renvoie une étiquette "meal" 
meal <- "the", #food.

//L'objet dont nous avons besoin pour analyser la phrase
annotator r;

//a sentence
string sentence="Here, the burger and the tartare are delicious."
vector v = r.parse(sentence); 

//Result: v =  [['meal',[10,16]],['meal',[25,32]]]
//Ce qui donne: deux 'meal' aux positions 10-16 et 25-32...
```

Ainsi, en quelques lignes, nous pouvons décrire un lexique couplé à des règles qui nous permet de détecter dans le texte les positions des éléments textuels qui nous intéresse.

L’exemple est très simple, mais on peut augmenter le vocabulaire à loisir, il est compilé sous la forme d’un transducteur à la volée, et aussi le nombre de règles. Encore une fois, l’interopérabilité entre ce mécanisme et le langage FIL est totale.

## Libraries

Pour terminer, Tamgu n’est pas un langage figé. Son architecture a cette particularité que l’implémentation d’une librairie externe obéit exactement au mêmes règles, à la même dérivation plus exactement, qu’un objet interne. Autrement dit, implémenter un mécanisme d’embedding basé sur Word2Vec correspond peu ou prou à l’implémentation du type _string_. A la différence de Java ou de Python qui n’autorisent que l’implémentation de méthodes externes,  il y a une correspondance directe entre un objet Tamgu et son implémentation sous la forme d’un objet C++. 
Tamgu offre ainsi des librairies qui encapsulent  cURL, liblinear, word2vec, SQLite, FLTK (GUI) ou Wapiti (CRF). 

## Conclusion

Tamgu a été conçu pour rendre les tâches de nettoyage ou de création de corpus aussi simples que possible. Vous pouvez désormais en quelques lignes de code identifier des patterns complexes ou encore générer du texte en appliquant des grammaires.
