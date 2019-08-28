# Propriétés fonctionnelles des objets

C++ est une conque, un coquillage rigide avec un corps tout mou.  C’est un langage où les choix les plus robustes peuvent être ruinés par un « cast » horrible. C’est un langage qui transforme la gestion mémoire en un champ miné, où la moindre étourderie peut faire planter le code aux endroits les plus insolites. Mais C++ est aussi un langage d’une grande souplesse qui peut s’interfacer avec la plupart des langages existants tels que Java ou Python. Choisir C++, c’est comme conduire une voiture de course des années trente, une puissance colossale sans airbags et sans ceinture de sécurité sur des pneus de vélo. Ça peut faire mal.

Quand j’ai commencé à travaillé sur Tamgu, en 2010, il n’y avait guère d’alternative, à part peut-être C. Le code de Tamgu a subi un grand nombre de réécriture massive à travers les années mais lors de la dernière itération, j’ai fait table rase du code existant et je suis reparti sur une architecture totalement différente. En particulier, j'ai beaucoup travaillé sur le moyen de réduire les aléas de la programmation C++ au maximum, en m'inspirant de la programmation fonctionnelle.

## Tabula Rasa

Il faut souvent moins de temps pour réécrire complètement un système depuis zéro qu’il n’en faut pour débogguer un code ancien et le faire vivre envers et contre tout. Car si le code actuel repose sur une architecture nouvelle, l’expérience accumulée permet d’aller très vite sur les invariants. Cela fait plus de cinquante ans que des archéologues s’échinent à reconstruire le Parthénon alors qu’il n’a fallu que 11 ans aux Grecs de l’antiquité pour l’édifier. 

Cette dernière version propose une unification de la gestion des objets qu’ils soient natifs ou issus d’une bibliothèque externe. Désormais, tous les objets manipulés par Tamgu sont implémentés de la même façon. C’est là une différence fondamentale avec les langages interprétés les plus courants.

## Machine Virtuelle

En effet, la majorité des langages de programmation actuels Java, Python, Kotlin ou Perl fonctionne sur le principe d’une « machine virtuelle » conçue pour exécuter un pseudo-langage machine appelé « opcode ». Il suffit dès lors de compiler les instructions de ces langages sous la forme de routines en « opcode » qui seront ensuite exécutées par la machine virtuelle. De plus, la gestion de la mémoire est effectuée via un « garbage collector » éliminant la majorité des problèmes de « memory leak » et autres joyeusetés de la programmation C++. 

Cependant, cette approche introduit un hiatus entre le fonctionnement des objets de base du langage et ceux issus des bibliothèques externes. De ce fait, une fonction externe s’exécute _en dehors de la machine virtuelle_. Si cette fonction manipule de plus ses propres structures de données internes, celles-ci échapperont à la gestion mémoire du « garbage collector ».
Débogguer une bibliothèque externe s’apparente souvent à parcourir un labyrinthe dans le noir avec une canne d’aveugle.

## Classe Tamgu

Tamgu fonctionne sur un principe radicalement différent. Tamgu introduit une classe commune _Tamgu_ qui se décompose en deux classes distinctes: _TamguTracked_ et _TamguReference_. 

* _TamguTracked_ est la classe mère de tous les objets statiques du langage, en particulier les instructions telles les boucles ou les fonctions. Chaque instance est systématiquement enregistrée dans une liste globale d’où le nom afin de pouvoir nettoyer la mémoire en une seule itération.

* _TamguReference_ est la classe mère de l’ensemble des données volatiles du langage telles que les string, les nombres ou les conteneurs.

### Get

Tous ces objets surchargent une méthode « Get » dont le rôle est d’effectuer l’évaluation de l’objet. Un « Get » sur une instance nombre ne fera que renvoyer l’instance elle-même, tandis qu’un « Get » sur un objet instruction provoquera son exécution. Chaque « Get » renvoie nécessairement une valeur.

## Compilation

La compilation consiste a transformer chaque instruction du langage en un objet particulier. Par exemple, un « if » est compilé en un objet _TamguIf_, un « while » en un objet _TamguWhile_ et une fonction en un objet _TamguFunction_.

### Récursive

De plus, la construction se fait de façon récursive. Par exemple, un objet _TamguFunction_ comprend une liste d’instructions que l’on peut écrire en C++ sous la forme suivante:

```C++
vector<Tamgu*> instructions;
```

La première étape lors de la compilation d’un programme est la création d’un arbre syntaxique dont chaque noeud correspond à un objet Tamgu. Il suffit dès lors de suivre l'arbre de façon récursive pour créer à chaque étape les objets correspondants et les ranger dans les objets parents. Ainsi, lorsque l'on construit la représentation d'une fonction, chaque objet construit à partir des sous-noeuds directs dans l'arbre sera rangé dans _instructions_.

### LISP

Cette façon de faire s’apparente à Lisp où chaque instruction est en fait un appel de fonction.

```LISP
(if (test) then else)
```

* « if » dans Lisp est une fonction à trois arguments qui retourne soit l’évaluation de « then » soit celle de « else » selon l’évaluation de « test ».

* « TamguIf » est un objet constitué d’une liste de trois éléments qui fonctionnent sur le même principe que le « if » de Lisp, à la différence près que pour évaluer les éléments on appellera « Get », là où Lisp appelle de façon implicite la méthode « eval ».

On retrouve la même idée dans la définition d’une fonction:

```LISP

(defun toto(x y)
    (if (< x y)
        (+ x y)
        (* x y)
    )
)
```
* Une fonction Lisp est composée d’une liste de paramètres et d’une liste d’instructions. Celle-ci doit de plus renvoyer une valeur.

* Une fonction « TamguFunction » est un objet composé d’une liste de paramètres et d’une liste d’instructions. L’exécution d’une fonction Tamgu consiste à apparier les paramètres et les arguments, puis à parcourir la liste des instructions en appliquant la méthode « Get » sur chacun d’entre eux pour les évaluer.

```C++
for (auto& a : instructions)
    a.Get();
```

Cette définition récursive d’objets ressemble furieusement à la définition récursive des appels de fonction dans Lisp.

## Garbage Collector

Tamgu n’a pas de mécanisme de « garbage collector ».  Il garde en mémoire la trace de certains objets telles que les instructions ou les constantes, mais il n’existe pas de mécanisme de destruction asynchrone des objets. 
Pour mieux comprendre le mécanisme de gestion de la mémoire, il suffit de réécrire le code ci-dessus:

```C++
Tamgu* o;
for (auto& a : instructions) {
    o = a.Get();
    o->Release();
}
```

Le destin d’une structure de données est d’être rangé quelque part soit dans une variable soit dans un conteneur. Lorsqu’une donnée n’a pas été utilisée, « Release » la détruit. Dans le cas contraire, « Release » la laisse intacte.

Ce mécanisme permet de contrôler appel après appel le cycle de vie d’une structure de données afin de s’en débarrasser au moment opportun.  On peut dès lors se passer d’un « garbage collector », dont l’exécution induit parfois des disruptions intempestives.

### Bibliothèques dynamiques

Une bibliothèque externe dans Tamgu est une simple dérivation de la classe _TamguReference_ avec une surcharge des méthodes adéquates pour l’exécution de son code. Par conséquent, les instances provenant de cette classe s’exécutent exactement de la même façon que le reste des objets natifs. Il n’existe donc aucun hiatus entre le code interne et le code externe. Une fonction enregistrera dans sa liste d’instructions un objet externe de la même façon qu’un objet interne. La création et la destruction d'un objet externe obéira au même cycle de vie qu'un objet interne.

## Propriétés fonctionnelles des objets

Il faut malgré tout noter que cette approche est plus gourmande en mémoire que les « opcodes » à ceci près que les instructions des langages ne sont pas mappés sur un seul opcode mais sur une routine composée d’opcode. Notons de plus que dans Tamgu les objets ont une sémantique beaucoup plus riche qu’un seul opcode.

En revanche, cette façon de faire permet de bénéficier de propriétés que l’on attribue généralement aux approches fonctionnelles. 

### Exécution sans état

Les méthodes « Get » ne dépendent que de leur contexte local pour s’exécuter. Si l’on reprend le code du « Get » de _TamguFunction_, on peut voir qu’il n’est nul besoin de faire référence à la moindre variable globale. Pas de pointeur dans une pile d’exécution locale, aucune référence à un état extérieur du programme. Les appels s’enchainent récursivement et chaque retour est évalué localement. 

### Robustesse

Comme il n’y a pas de gestion globale du déroulement du code, les effets de bord sont très rares. De plus l’association objet/instruction conduit à du code très local et surtout très petit. Si l’on examine le code C++ dédié aux instructions Tamgu dans le fichier « codeexecute.cxx » on peut noter que les méthodes « Get » dépassent rarement la dizaine de lignes. Cette parcimonie du code associée à l’absence d’effets de bord rend le déboggage très simple.

### Réentrance

Enfin, l’absence d’états permet aussi de rendre le code facilement réentrant. L’intégration du multithreading dans Tamgu se résume à moins de 250 lignes de code, essentiellement la préparation et le nettoyage du contexte d’exécution. Le reste des instructions peut s’exécuter sans aucune difficulté et sans modification particulière dans un thread.

## Conclusion

Le choix d’implémenter sous la forme d’objets emboîtées les instructions de Tamgu offre de multiples avantages en terme de maintenance et de compréhension du code. Cette approche permet de conserver la sémantique des instructions tout en gardant le code petit et lisible, offrant une grande simplicité de lecture et de déboggage. Il permet aussi de conserver une homogénéité d’interprétation entre les objets internes et externes, évitant par là le danger d’une exécution s’effectuant à l’extérieur de la machine virtuelle. De plus, le suivi très précis du cycle de vie des objets permet de réduire au maximum les risques de "memory leaks" ou de "pointer null exception".

