# Propriétés fonctionnelles des objets

C++ est une carapace rigide avec un corps tout mou.  C’est un langage où les choix les plus robustes peuvent être ruinés par un « cast » horrible. C++ transforme la gestion mémoire en un champ miné, où la moindre erreur peut rendre le code incontrôlable et le faire planter aux endroits les plus insolites. Mais C++ est aussi un langage d’une grande souplesse qui peut s’interfacer avec la plupart des langages existants tels que Java ou Python. Choisir C++, c’est comme conduire une voiture de course des années trente, une puissance colossale sans airbags et sans ceinture de sécurité sur des pneus de vélo.
C’est cette souplesse et cette efficacité qui m’a amené à choisir C++ pour implanter mon langage de programmation, en dépit du danger constant de « memory leaks » et autres joyeusetés du langage. 
Tamgu est le dernier né de toute une série d’expérimentation en C++ que j’ai mené pendant plusieurs années pour construire le langage de programmation que je recherchais. J’avais besoin de vitesse et d’efficacité pour être capable de construire et de modifier de très gros corpus textuels et C++ me semblait le seul langage à même de me fournir cette puissance. En revanche, avant d’aboutir à Tamgu, j’ai dû batailler avec le langage pour isoler un mode de fonctionnement qui permette une gestion mémoire contrôlée et efficace. 

# Machine Virtuelle

La plupart des langages modernes comme Java ou Python fonctionne sur le principe d’une machine virtuelle  conçue pour exécuter un pseudo-langage machine appelé « opcode » ou « byte code », tandis que la gestion mémoire est contrôlée parallèlement par un « garbage collector ». Ce choix est très efficace mais il pose en revanche certains problèmes lorsque l’on veut étendre ces langages avec des bibliothèques externes compatible. En particulier le code de ces bibliothèques s’exécute généralement _en dehors de la machine virtuelle_, tandis que leurs structures de données internes échappent le plus souvent au _garbage collector_.

## Machine C++

Dans le cas de Tamgu, s’il existe une machine virtuelle, celle-ci se confond avec l’exécution de l’interpréteur en C++. Dans Tamgu, chaque élément du langage est implémenté sous la forme d’un objet C++. Il suffit de surcharger certaines méthodes particulières pour spécialiser leur comportement. En particulier, tous ces objets surchargent la méthode « Get » dont l’application selon le type de l’objet conduit par exemple à l’exécution d’une boucle ou d’une fonction. En revanche, un « Get » sur un objet nombre ne fera que renvoyer ce nombre lui-même.

### Evaluation

Ces objets dérivent tous de la même classe mère: _Tamgu_ ce qui signifie qu’une séquence d’instructions peut être représentée par un simple vecteur:

```C++
vector<Tamgu*> instructions;
```

Un programme Tamgu est donc enregistré en mémoire sous la forme d’objets encapsulés. Une fonction, par exemple, est représentée par un objet _TamguFunction_ qui contient justement une déclaration comme celle ci-dessus. Chacun des éléments de ce vecteur peut être à son tour une boucle, une affectation, un appel de fonction ou une déclaration de variable. Une boucle _while_ sera représenté par un objet _TamguWhile_ qui contiendra à son tour le même type de vecteur, lequel pourra contenir des boucles, des affectations, des appels de fonction etc. 

Ainsi, un programme Tamgu est compilé sous la forme d’une structure arborescente imbriquée d’objets C++.

Pour exécuter un programme Tamgu, il suffit de parcourir une liste d’objets _Tamgu_ et d’évaluer chacun d’entre eux en appelant sa méthode _Get_. Au niveau le plus haut, un programme Tamgu se résume à un simple vecteur d’instructions. La boucle principale ressemble à ceci:

```C++
Tamgu* o;
for (auto& a : instructions)
o = a.Get();
o->Release();
```

Une méthode _Get_ renvoie systématiquement au niveau supérieur une valeur. Si cette valeur « o » n’a pas été sauvegardée dans une variable ou dans un conteneur, l’appel de la méthode _Release_ la détruira. Le nettoyage des objets se fait donc en passant, grâce à la magie de la pile d’exécution de l’interpréteur. Ce mécanisme permet de contrôler de façon précise la création et la destruction des structures de données.

### Exécution sans état

De plus, comme le montre l’exemple ci-dessus, cette exécution ne fait appel à aucune variable globale, elle ne modifie aucun pointeur dans une pile d’exécution parallèle. L’exécution ne change aucun état global, elle est « stateless », ce qui permet d’assurer à la fois réentrance et robustesse. D’ailleurs, cette approche réduit aussi le code nécessaire à l’exécution des instructions à très peu de lignes de C++. Si l’on examine le fichier « codeexecute.cxx » qui contient le code de la plupart des instructions Tamgu, on pourra observer que la majorité des méthodes _Get_ ne dépasse pas la dizaine de lignes. Le code est réduit à de courtes méthodes simples à lire et à débogguer.

### Fonctionnel

Exécution sans état, réentrance, robustesse, ces bonnes propriétés sont en fait dues à un choix d’implémentation qui s’inspire de la programmation fonctionnelle. Si l’on prend l’exemple d’un langage comme Lisp, la moindre instruction dans ce langage est conçue comme une fonction.

Prenons un exemple très simple celui du « if » et comparons le à sa version dans Tamgu:

```LISP
(if (test) then_exp else_exp)
```

Et en C++:

```C++
Class TamguIf : public Tamgu {
Tamgu* instructions[3];

//La méthode « Get ».
Tamgu* Get() {
Tamgu* a = instructions[0]->Get();
if (a == aTrue)
return instructions[1]->Get();
else
return instructions[2]->Get();
}
};
```

* « if » dans Lisp est une fonction à trois arguments qui retourne soit l’évaluation de « then_exp » soit celle de « else_exp » selon l’évaluation de « test ».

* « TamguIf » est un objet constitué d’une liste de trois éléments qui fonctionnent sur le même principe que le « if » de Lisp, à la différence près que pour évaluer les éléments on appellera « Get », là où Lisp appelle de façon implicite la méthode « eval ».

En évaluant récursivement une liste emboitée d’objets C++, on peut facilement reproduire le comportement d’une liste emboîtée de fonctions.

Cette façon de faire rend aussi le langage facilement extensible et facilement optimisable. Il suffit de rajouter des objets instructions supplémentaires pour enrichir le langage. On peut aussi parfois remplacer certaines séquences récurrentes d’instructions par un seul objet plus compact et plus rapide. Mais il existe un dernier aspect important de cette approche, le fait que objets internes et externes puissent être gérés de façon homogène par le moteur d’exécution.

## Homogénéité

Comme on vient de le voir, Tamgu encapsule l’ensemble de ses éléments sous la forme d’objets C++ dont certaines méthodes sont  alors surchargés pour spécialiser leur comportement. C’est aussi le cas lorsqu’un utilisateur veut créer une bibliothèque compatible avec Tamgu. Dans ce cas, le programmeur doit construire sa propre dérivation de la classe _Tamgu_ pour encapsuler son code personnel. Or, pour des raisons évidentes de robustesse, seule la machine virtuelle peut avoir le droit de vie ou de mort sur un objet. Par conséquent, il faut cacher au programmeur tout ce qui relève du cycle de vie d’un objet, aussi bien pour la classe qu’il développe que pour les objets qui pourraient apparaître dans son code lors de l’exécution.

Faire en sorte que la machine virtuelle seule ait accès à la création et à la destruction d’un objet requiert quelques ajustements.

### Factory

Tout d’abord, il faut s’interroger sur le mécanisme de création d’un objet. Dans Python, lorsque l’on crée une bibliothèque externe _toto_, celle-ci doit souvent fournir une méthode _totonew_ dont le but est de créer des objets dont le cycle de vie sera compatible avec la machine virtuelle Python. Dans le cas de Tamgu, on crée un objet en fournissant son type et une variable:

```C++
toto myvar;
```
Cette déclaration suffit à créer un objet, qu’il soit interne ou issu d’une bibliothèque externe compatible.

Tamgu gère la création d’objet sur la forme d’une _factory_. Une _factory_ est une table où l’on range une instance de tous les objets correspondant à tous les types de Tamgu. Chaque type est associé à un identifiant numérique qui correspond à la case où une instance de l’objet C++ correspondant est rangé. Par exemple, à l’index 23 dans cette table se trouve une instance de la classe _Tamgustring_. Cet identifiant 23 est aussi associé à un type Tamgu: _string_. De plus, chaque objet surcharge une méthode _Newinstance_ dont le rôle est de créer une nouvelle instance de l’objet en question.

```C++
string s; //string a comme identifiant 23
```

Lorsque la déclaration de _s_ est exigée, l’index de _string_  (ici 23)  permet d’accéder à une instance de _Tamgustring_ dans la table de la _factory_. Il ne reste plus qu’à appeler la méthode _Newinstance_ pour disposer d’un nouvel objet que l’on associera à _s_.

### Déclaration

La table de la _factory_ est vide à l’initialisation. Ce sont les objets eux-mêmes qui viennent enregistrer une instance d’eux-mêmes et se voient attribuer en séquence un identifiant numérique unique. Il suffit de jeter un coup d’oeil aux fichiers _objectrecording.cxx_ et _containerrecording.cxx_ pour le vérifier. Chaque objet dispose d’une méthode de classe:  InitialisationModule qui enregistre dans la table de la _factory_ cette première instance. Chaque fois qu’on lance le moteur, la table de la _factory_ est ré-initialisée intégralement par les objets de base qui font partie intégrante de l’interpréteur.

Dans le cas d’une bibliothèque externe compatible, le mécanisme est à peine plus élaboré. Tout d’abord chaque bibliothèque implémente une dérivation de la classe _Tamgu_. Le programmeur surcharge alors les différentes méthodes de sa propre classe pour la spécialiser. En revanche, il ajoute aussi une fonction "C" dont le nom est « nombib_InitialisationModule ». En toute fin de chargement, Tamgu exécute alors cette fonction dont le rôle est _d’appeler la méthode de classe InitialisationModule_ pour qu’une instance de l’objet implémenté vienne s’enregistrer dans la _factory.

A la fin du chargement, l’origine de l’objet devient dès lors indiscernable de celles des autres objets. Pour le moteur, créer une variable de type _string_ ou de type _toto_ obéit exactement aux mêmes règles. Alors que dans Python ou Java, le code externe s’exécuter en dehors de la _machine virtuelle_, ici, il est au contraire intégré dynamiquement comme une extension naturelle de la machine virtuelle.

## Conclusion

Le choix d’implémenter sous la forme d’objets emboîtées les instructions de Tamgu offre de multiples avantages en terme de maintenance et de compréhension du code. Cette approche permet de conserver la sémantique des instructions tout en gardant le code petit et lisible, offrant une grande simplicité de lecture et de déboggage. Il permet aussi de conserver une homogénéité d’interprétation entre les objets internes et externes, évitant par là le danger d’une exécution s’effectuant à l’extérieur de la machine virtuelle.

