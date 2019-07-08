# Introduction

Le document qui suit décrit la façon dont le code de Tamgu (탐구) a été implémenté. Ce document est destiné à toute personne souhaitant découvrir les ressorts internes de l'interpréteur. Si la lecture de ce document peut éclairer sur les choix d'implémention de Tamgu, sa lecture n'est pas obligatoire pour utiliser le langage.


# Organisation du code

L'interpréteur d'un langage informatique est généralement décomposé en trois parties:

* La construction de l'arbre syntaxique à partir du code. Cette opération est aussi appelé "parsing".
* La compilation du code qui transforme l'arbre syntaxique en une structure exécutable
* L'execution du code

## Le "parseur": _codeparse_

La construction de l'arbre syntaxique est effectuée grâce à la classe: _bnf_tamgu_ dont le code est enregistré dans les fichiers: _codeparse.cxx_ et _codeparse.h_.

Attention: Ces deux fichiers n'ont pas été écrits à la main mais générés automatiquement à partir du fichier _tamgu_ présent dans le répertoire: _bnf_. 
Le répertoire _bnf_ contient de plus les scripts en Python pour régéner de nouvelles versions de _codeparse_ si besoin est.

La description du langage _BNF_ utilisé est enregistrée dans le fichier _tamgu_ lui-même.

### Noeuds: x_node.h

Ce fichier contient les classes pour enregistrer l'arbre syntaxique: _x_node_. 

Un _x_node_ contient son nom (_token_), sa valeur (_value_), sa position dans le texte initial (_start_, _end_) ainsi que les noeuds fils.

Les _x_nodes_ sont produits par _codeparse_.

### Segmentation

En revanche, la structure en entrée de _codeparse_ est le résultat d'une segmentation appelée aussi _tokénisation_. Le fichier contient la classe _x_rules_ ainsi que ses dérivations _x_reading_ et _x_wreading_. La classe _x_rules_ permet l'enregistrement et l'utilsation de règles de segmentation.

Ces règles de segmentation découpent le code en entrée en _token_ tout en conservant à la fois sa position dans le fichier et sa nature telle que définie par la règle.

Par exemple:
```
rules.push_back("%d+(.%d+)(e([- +])%d+)=3");
```

Cette règle permet de reconnaître entre autre un nombre à virgule avec exposant. Lorsqu'un tel _token_ est repéré, on renvoie le code "3" qui identifie la nature de ce token comme étant un nombre. On pourra alors faire référence à ce code dans la grammaire BNF pour générer le noeud correct correspondant:

```
# Numbers as parsed by the tokenizer: "3" is the code yielded by the tokenizer.
^3 anumber := .
```

Par défaut, ces règles reconnaissent directement des chaînes de caractères entre quotes ou encore des séparateurs telles que ";", ":".

```
# punctuation as parsed by the tokenizer
^0 punct := .

# String definitions as parsed by the tokenizer
^1 astringdouble := .
^2 astringsimple := .
^5 afullstring := .

# Numbers as parsed by the tokenizer
^3 anumber := .

#token as parsed by the tokenizer
^4 word := .
^4 typename := .
^4 astring := .

#regular expressions. In some cases, the code is a simple character.
^9 atreg := .
^a astreg := .
^b apreg := .
^c aspreg := .
```


## Le compilateur: _codecompile_

Une fois l'arbre syntaxique correctement construit, on le transmet au compilateur. Le compilateur est implémenté sous la forme d'une classe: TamguCode qui contient ce qu'il faut pour traverser l'arbre syntaxique et construire les représentations internes.

### Choix d'une méthode de compilation

La classe TamguGlobal contient un dictionnaire particulier qui associe un nom de token avec une méthode de compilation de TamguCode: _parseFunctions_. La méthode _RecordCompileFunctions_ enregistre toutes ces méthodes dans ce dictionnaire.

La méthode principale lorsque l'arbre syntaxique est compilée est: _Traverse_.

Cette méthode comprend deux paramètres:  le noeud courant dans l'arbre syntaxique et l'objet que l'on est en train de construire.

Cette méthode examine le noeud courant et regarde si son _token_ est associé à une méthode de compilation. Si ce n'est pas le cas, il examine les noeuds fils: Un _token_ peut ne pas être associé à une méthode.

Lorsqu'une méthode de compilation est choisie, celle-ci va enrichir l'object courant avec l'information nécessaire en analysant le noeud courant.

### TamguGlobal

L'interpréteur de Tamgu ne comprend quasiment aucune variable globale à une exception: _globalTamgu_ de type _TamguGlobal_.

La classe _TamguGlobal_ permet de centraliser la création des objets pour effectuer leur destruction. _Tamgu_ ne comprend pas de _Garbage Collector_ à proprement parler, mais garde la trace de nombre d'objets dont l'interpréteur peut avoir besoin pendant l'execution. Par exemple, l'ensemble des instructions est conservé dans le _vecteur atomique tracked_.

Cette classe garde aussi la trace des _threads_ et gère leur création ou leur suppression. _TamguGlobal_ gère aussi la pile des variables.

### Objets

Tamgu fait la différence entre deux types d'objets: les instructions et les structures de données.

Cependant, ces objets dérivent tous de la classe: _Tamgu_.

Les instructions dérivent en particulier de la classe: _TamguTracked_ dont les instances sont systématiquement sauvegardées dans le vector: _tracked_.
Les structures de données dérivent de _TamguReference_. Ces objets ne sont pas systématiquement enregistrés dans _tracked_ sauf s'ils sont déclarés directement dans le code.

Les objets numériques par exemple dérivent de _TamguReference_. Les conteneurs en revanche dérivent de _TamguObject_ qui apporte la notion de _lock_ pour protéger les objets lors de leur manipulation dans des threads.


## Exécution: _codeexecute.cxx_

La méthode prinicpale est: _Run_. _Run_ parcourt le vecteur des instructions et exécute chacune d'entre elles. L'exécution se fait en appelant la méthode: _Get_ qui prend trois paramètres en entrée. Le premier paramètre est le _contexte_ qui correspond par exemple dans le cas d'une affectation à l'objet qui reçoit la valeur. Le second paramètre dépend du contexte d'exécution. Par exemple, dans le cas d'un _Get_ sur un conteneur, il correspond à un index. Le troisième paramètre est l'identifiant de thread. Celui-ci permet d'accéder aux informations particulières correspondant à un _thread_ donné.

Tous les objets exposent une méthode _Get_. 
* Pour une structure de données, il s'agit le plus souvent de renvoyer sa valeur.
* Pour une instruction, il s'agit de l'exécuter.

Les objets exposent aussi la méthode: _Put_ dont le but est de modifier la valeur d'une structure de données. Cette méthode n'est généralement pas implémentée pour les instructions.

### Instructions

Chaque instruction: _if, while, for, case etc._ correspondant à une classe spécifique, chacune avec une implémentation particulière pour sa méthode _Get_.

La plupart des instructions dérivent de la méthode: _TamguInstruction_ qui introduit la notion de vecteur d'instructions. Les objets qui dérivent de cette méthode peuvent ainsi itérer sur le contenu.

Les fonctions et les threads dérivent de la classe _TamguDeclaration_. Cette classe offre l'infrastructure nécessaire pour enregistrer des variables. Plus exaactement, la classe _TamguDeclaration_ permet la création et l'enregistrement local de variables. 

Important: Ces classes sont toutes immuables.


### Structures de données

Les structures de données se différencient des instructions sur plusieurs points. Tout d'abord, chaque structure de données est associée à une _référence_. Cette _référence_ permet de gérer le cycle de vie de cette structure de données. Certains objets, comme les conteneurs ou les chaînes de caractères dérivent de _TamguObject_ qui fournit un _lock_ pour les protéger lors d'une execution dans un _thread_.

### Objet

Chaque structure de données est une classe indépendante dérivée de _TamguReference_, enregistrée dans son propre fichier. Chaque objet expose a minima les méthodes suivantes:

* Les deux méthodes qui suivent permettent d'enregistrer les méthodes associées à un objet. _InitialisationModule_ en particulier est appelé au démarrage pour enregistrer l'ensemble des objets de base et leurs méthodes. La méthode _AddMethod_ permet d'associer un nom de méthode côté _Tamgu_ avec une méthode de la classe correspondante.
** static void AddMethod(TamguGlobal* g, string name, objectMethod func, unsigned long arity, string infos);
** static bool InitialisationModule(TamguGlobal* g, string version);

* Les deux méthodes qui suivent permettent de renvoyer la valeur d'un objet ou de la modifier 
** Tamgu* Get(Tamgu* context, Tamgu* value, short idthread); 
** Tamgu* Put(Tamgu* context, Tamgu* value, short idthread); 

* Les méthodes qui suivent permettent de renvoyer des valeurs atomiques:
** long Integer();
** string String();
** etc.

* Les méthodes qui suivent permettent de comparer des valeurs entre elles
** Tamgu* less(Tamgu*)
** Tamgu* lessequal(Tamgu*)
** Tamgu* more(Tamgu*)
** Tamgu* moreequal(Tamgu*)
** Tamgu* same(Tamgu*)
** Tamgu* different(Tamgu*)

* Les méthodes qui suivent permettent d'effectuer des opérations avec d'autres valeurs
** Tamgu* minus(Tamgu*)
** Tamgu* plus(Tamgu*)
** Tamgu* multiply(Tamgu*)
** Tamgu* divide(Tamgu*)
etc.


### Template

Le répertoire _template_ vous permet de construire vos propres objets et vos propres bibliothèques. Il suffit de donner le nom de votre objet au script et il génère automatiquement un répertoire dans lequel il place les fichiers source et include ainsi que les Makefiles. Il vous suffit alors de remplir ces templates avec votre propre code.







