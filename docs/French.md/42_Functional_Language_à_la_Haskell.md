# Taskell

Taskell est un langage fonctionnel inspiré de Haskell. Il offre des fonctionnalités similaires à Haskell, mais avec un accent particulier sur une sélection de fonctions. Bien que Taskell ne se comporte pas comme un compilateur Haskell complet, il intègre certains aspects intéressants du langage Haskell.

## Avant de commencer : de nouveaux opérateurs

Avant de plonger dans le langage, permettez-nous de vous présenter quelques opérateurs spécifiques qui enrichissent le monde de Taskell.

### Déclarations de plage : [a..b]

Pour se conformer au langage Haskell, Taskell a ajouté une nouvelle façon de déclarer une plage d'éléments en utilisant l'opérateur "..". Par exemple, `[1..10]` définit le vecteur `[1,2,3,4,5,6,7,8,9,10]`.

### Pas

Par défaut, le pas est de 1, mais il est possible de définir un pas différent. Vous pouvez définir le pas directement avec un ":" à la fin de l'expression. Par exemple, `[1..10:2]` définit le vecteur `[1,3,5,7,9]`. Vous pouvez également définir le pas en fournissant le prochain élément dans la définition. Par exemple, `[1,3..10]` définit le vecteur `[1,3,5,7,9]`. Cela fonctionne également avec les caractères. Par exemple, `[‘a’,’c’..’g’]` définit le vecteur `['a','c','e','g']`. Le même vecteur pourrait également être défini avec `[‘a’..’g’:2]`.

### Plages infinies

Taskell propose également une notion de plage infinie d'éléments. Il existe deux cas : vous pouvez soit ignorer le premier élément de l'ensemble, soit le dernier élément. `[1..]` définit un vecteur infini qui commence à 1, en avant : `[1,2,3,4…`. `[..1]` définit un vecteur infini qui commence à 1, en arrière : `[1,0,-1,-2,-3…`. Vous pouvez également utiliser différents pas. `[1..:2]` définit un vecteur infini qui commence à 1, en avant : `[1,3,5…`. `[..1:2]` définit un vecteur infini qui commence à 1, en arrière : `[1,-1,-3…`. `[1,3..]` définit un vecteur infini qui commence à 1, en avant : `[1,3,5…`. `[..-1,1]` définit un vecteur infini qui commence à 1, en arrière : `[1,-1,-3…`.

### Trois nouveaux opérateurs : &&&, ||| et ::

Ces trois opérateurs sont utilisés pour concaténer une liste d'éléments ensemble ou pour ajouter un élément à un vecteur.

#### Fusion : &&&

Cet opérateur est utilisé pour fusionner différents éléments en un vecteur. Si l'un des éléments n'est pas une liste, il est simplement fusionné dans la liste actuelle. Par exemple, `vector v= 7 &&& 8 &&& [1,2]; println (v);` donnerait `[7,8,1,2]`. Cet opérateur est similaire à "++" en Haskell. Étant donné que cet opérateur était déjà défini dans Tamgu, il a été modifié en "&&&".

#### Combinaison : ||| (c1 ||| c2)

Cet opérateur combine les valeurs d'un conteneur avec un autre conteneur, ou d'une chaîne avec une autre chaîne. Lorsque les conteneurs sont des conteneurs de valeurs, l'opérateur "+" est utilisé pour ajouter ou concaténer les valeurs de c1 avec c2. Par exemple, `vector v= [1,2,3]; vector vv = [4,5,6]; println (v ||| vv); //[1,4],[2,5],[3,6]` serait le résultat. De même, `ivector iv= [1,2,3]; ivector ivv = [4,5,6]; println (iv ||| ivv); //[5,7,9]` serait le résultat. La même opération peut être effectuée avec des chaînes, des maps ou des maps avec des valeurs entières.

#### Ajout : ::

Cet opérateur fusionne l'élément dans le vecteur actuel. Par exemple, `1::v` donnerait `[1,7,8,1,2]`, et `v::12` donnerait `[1,7,8,1,2,12]`.

## Bases

### Déclaration d'une instruction de type Taskell

Toutes les instructions Taskell dans Tamgu doivent être déclarées entre "<..>", que le compilateur interne de Tamgu utilise pour détecter une formule Taskell. Par exemple, `vector v=<map (+1) [1..10]>;` ajoute 1 à chaque élément du vecteur.

### La structure la plus simple

La structure la plus simple pour un programme Taskell est de retourner une valeur telle que `<1>`.

Vous pouvez également retourner un calcul tel que `<3+1>;`, auquel cas le système renverra une seule valeur atomique.

### Utilisation de >, <, |, << et >>

Ces opérateurs peuvent causer des problèmes lorsqu'ils sont utilisés à l'intérieur d'une formule Taskell, car ils peuvent confondre le compilateur avec l'ouverture ou la fermeture de crochets Taskell. Pour éviter ce problème, vous devez insérer ces expressions entre parenthèses.

Par exemple, `<x | x < - [-5..5], (x > 0)>` donne `[1,2,3,4,5]`, et `< (x << 1) | x <- [0..5]>` donne `[0,2,4,6,8,10]`.

### Itération

Le langage Taskell offre un moyen pratique et efficace de représenter des listes. Dans Tamgu, ces listes sont implémentées sous forme de "vecteurs", qui peuvent être échangés entre différentes structures.

L'instruction Taskell la plus basique a la forme suivante : `<x | x < - v, Boolean>`. Elle renvoie une liste en résultat, avec les étapes suivantes :

1. Nous ajoutons x à notre liste de résultats actuelle.
2. Nous obtenons x en itérant à travers v, indiqué par `x <- v`.
3. Nous mettons une contrainte booléenne, qui peut être omise.

La raison pour laquelle elle renvoie une liste est due à l'itération dans l'expression. Par exemple, `<x | x < - [-5..5], x!=0>` donne `[-5,-4,-3,-2,-1,1,2,3,4,5]`.