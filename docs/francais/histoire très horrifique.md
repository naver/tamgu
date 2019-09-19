# L’histoire très horrifique du compteur de référence

[English version](https://github.com/naver/tamgu/wiki/6.-The-very-horrific-story-of-the-reference-counter)

Il arrive un moment où dans sa carrière, un informaticien va devoir pénétrer dans un monde d’initiés où l’esprit le plus fort peut se transformer en quelques heures en une épave bavante et balbutiante. Un monde terrifiant qui transforme le moindre bug en une brute épaisse plus dangereuse qu’un loup garou un jour de pleine lune. Un monde fait pour des esprits qui ne sont pas encore nés. Je veux évidemment parler du multithreading. 

## Stabilité

J’avais créé un petit programme Tamgu dont le but était de tester la solidité de mes threads. Dans une fenêtre graphique, une multitude de petits cercles tourbillonnaient sans jamais se toucher, poursuivant un cercle rouge qui rebondissait dans tous les coins de l’écran. Chacun de ces cercles étaient contrôlés par son propre thread. 

Et le programme plantait…

Non pas de façon nette, dans un endroit précis du code. Non, c’eût été trop simple, il plantait de façon aléatoire aux endroits les plus insolites. Mais pire que ça, il ne plantait qu’en mode release. Lorsque je l’exécutais dans le debugger, il fonctionnait parfaitement. 

Et parfois, il ne plantait pas du tout, tournant pendant des heures sans le moindre problème. Si cela ne suffisait comme problème, il lui arrivait aussi de se lancer dans une consommation outrageuse de la mémoire, mettant souvent ma machine à genoux.

## Pas de solutions

J’avais beau imaginer les scénarios les plus alambiqués, me brûler les yeux à suivre pas à pas en mode debug chaque création et chaque destruction d’objets, rien ne me sautait aux yeux. Il y a un étrange plaisir à découvrir l’origine d’un bug, notre esprit s’illumine, un sourire se dessine sur nos lèvres, on pousse un soupir de soulagement. Mais là, plus je tentais de comprendre ce qui se passait plus la frustration s’installait. Je mis des traces partout dans mon code, ce qui eut pour effet _de faire disparaitre les plantages_. De plus en abasourdi, je commençais à soupçonner le compilateur, le système d’exploitation (Windows). Je ne comprenais pas comment la présence d’un affichage pouvait faire disparaitre un bug. 

## Une fois qu’on a éliminé l’impossible…

En désespoir de causes, j’appliquais la règle de Sherlock Holmes et je décidais d’afficher le compteur de référence. Tamgu comme beaucoup d’autres langages utilise un compteur de référence pour traquer le cycle de vie d’un objet. Lorsque ce compteur passe à zéro l’objet est détruit. J’avais jusque là dédaigné ce compteur, un simple entier que l’on incrémentait ou décréméntait selon les besoins. Je ne voyais pas comment il pouvait être une source de problème.

Pourtant, lors d’une exécution, je vis qu’un _compteur de référence était à 2… Il aurait dû être à 1._

Ça paraissait infime comme détail, mais je compris la raison pour laquelle parfois le programme consommait de la mémoire. Si ce compteur était à 2, il ne pouvait pas repasser à 0 plus tard et donc l’objet en question ne pouvait plus être détruit.

## Lumière

Je me replongeais dans le debugger et je tentais de découvrir où ce compteur pouvait avoir subi un incrément de trop. Rien, nada, zilch…

Ma frustration augmenta d’un cran… Peut-être que sous Linux, j’aurai plus de chance… 

Je fis tourner le programme sous « gdb » et de nouveau rien n’y fit. Le programme fonctionnait correctement. La version release en revanche plantait avec encore plus de conviction que sous Windows et à des endroits encore plus improbables.

J’explorais Stack Overflow pour regarder le type d’erreurs les plus communes en multithreading.  Mais, je ne trouvais rien de concluant.

Puis lors d’un test sous Linux, je vis qu’un _compteur de référence était à 0. Il aurait dû être à 1._

Le programme planta juste après…

Je compris soudain que ces deux bugs étaient liés... 

Et la lumière se fit…

## Séquence

En fait, le code plantait parce que les opérations de lecture, de calcul et d'écriture étaient toutes dissociées les unes des autres.

Prenons un exemple simple, comme celui qui suit. Nous supposerons que la valeur initiale de référence est 1.

```
reference++; (thread 1)
reference--; (thread 2)
```

Or, ces deux opérations peuvent être représentées par les six instruction suivantes:

```
1. lecture de la référence (thread 1)
2. incrémentation de la référence (thread 1)
3. écriture de la nouvelle référence (thread 1)
———
4. lecture de la référence (thread 2)
5. décrémentation de la référence (thread 2)
6. écriture de la nouvelle référence (thread 2) 
```

Hélas dans un programme multithreadé, ces instructions peuvent se mélanger librement...

```
Plantage

1. lecture de la référence, on lit 1 (thread 1)
4. lecture de la référence, on lit encore 1 (thread 2)

ARGH...

2. incrémentation de la référence (thread 1) --> reference vaut 2
5. décrémentation de la référence (thread 2) --> référence vaut 0

La décohérence s'accomplit...

3. écriture de la référence (thread 1) --> on écrit 2
6. écriture de la référence (thread 2) --> on écrase 2 avec la valeur 0

L’objet est détruit...

Plantage assuré, l’objet était toujours utilisé…
```

Tant que la cellule mémoire qui contient la référence n’a pas été écrite, elle vaut 1 pour les deux lectures. Les calculs se font en parallèle et conduisent donc à deux valeurs différentes. Selon l’ordre des opérations 3 et 6, cela conduit soit à une destruction de l’objet, soit à son indestructibilité.

```
Indestructible
…
_6_. écriture de la référence (thread 2) --> on écrit 0
_3_. écriture de la référence (thread 1) --> on écrase 0 avec la valeur 2.

L'objet est désormais indestructible, on ne pourra plus le détruire.
```

La solution était pourtant extrêmement simple, il suffisait de déclarer le compteur de référence comme une valeur « atomique ».

```C++
std::atomic<short> reference;
```

Une valeur « atomique » permet de synchroniser les lectures et les écritures dans un programme multithreadé et permet d’échapper à ce bug. Le fait que le programme ne plante pas en mode débug vient de ce que l’exécution étant ralenti, le risque de mélange des instructions est beaucoup plus faible. 

Jamais je n’aurais pensé que de simples opérations mathématiques sur un entier puisse avoir un impact aussi grand sur du code.
