# Tamgu : Une Introduction

Le langage Tamgu emprunte de nombreux concepts à d'autres langages, principalement C++ et Python. Par conséquent, il est assez facile à apprendre pour quelqu'un ayant une connaissance de base de ces langages.

## Quelques éléments du langage

Un programme Tamgu contient des déclarations de variables, des déclarations de fonctions et des déclarations de cadres (ou classes). Les variables et les fonctions peuvent être déclarées n'importe où, sauf à l'intérieur des boucles.

### Commentaires

Les commentaires sur une seule ligne sont introduits par `//`. Par exemple :

```cpp
// C'est un commentaire
```

Les commentaires sur un bloc de lignes sont insérés entre `/@` et `@/`. Par exemple :

```cpp
/@
Ceci est
un bloc de commentaires.
@/
```

### Fonctions

Une fonction est déclarée avec le mot-clé `function`, suivi d'un nom et de certains paramètres.

### Cadres

Un cadre est déclaré avec le mot-clé `frame`, suivi d'un nom. Un sous-cadre est simplement déclaré comme un cadre à l'intérieur d'un cadre.

### Pré-déclarations de fonctions et de cadres

La pré-déclaration de fonctions et de cadres n'est pas nécessaire en Tamgu, car l'interpréteur parcourt d'abord le code pour détecter toutes les fonctions et tous les cadres et les déclare au préalable. Par conséquent, le code suivant est valide :

```cpp
// Nous appelons call2 à partir de call1
function call1(int x, int y) {
    call2(x, y);
}

// call2 est déclarée après call1
function call2(int x, int y) {
    println(x, y);
}
```

## Fonctions Système

### Sortie : `_exit()`

Cette fonction est utilisée pour quitter le programme.

### Erreur sur Clé : `_erroronkey(bool)`

Par défaut, toute tentative d'accès à une valeur dans une map avec une clé inconnue ne génère pas d'exception. La fonction `_erroronkey(bool)`, qui doit être placée au tout début de votre code, modifie ce comportement.

### Taille de la Pile : `_setstacksize(taille)`

La taille de la pile est initialement fixée à 1000 appels de fonctions. Vous pouvez modifier cette valeur avec cette fonction. Cependant, si la taille de votre pile est trop grande, votre programme risque de planter car elle pourrait devenir plus grande que la taille réelle de la pile de votre système.

### Nombre de Threads : `_setthreadlimit(nb)`

Le nombre de threads réels pouvant s'exécuter en parallèle est initialement fixé à 1000 sur Windows et 500 sur d'autres systèmes. Vous pouvez modifier cette valeur pour augmenter le nombre de threads pouvant s'exécuter en parallèle.

### Fonctionnalités Valides : `_setvalidfeatures(mapss fonctionnalités)`

Cette méthode est utilisée pour mettre des contraintes sur les fonctionnalités valides qui peuvent être utilisées pour les synodes et les dépendances. Voir la section synode, dépendance pour plus de détails.

### Nombre de Threads à Joindre Ensemble : `_setjoinedlimit(nb)`

Par défaut, jusqu'à 256 threads peuvent être "joins" ensemble. Vous pouvez modifier ce nombre avec cette fonction.

### Variables d'Environnement Initiales : `_setenv(nomvar, valeur)`

Il est possible de définir des variables d'environnement au moment du lancement avec cette fonction.

### Version de Tamgu : `_version()`

Renvoie une chaîne de caractères avec des informations sur la version de Tamgu.

### Affichage Miroir : `_mirrordisplay(bool)`

Cette fonction est utilisée pour définir l'affichage miroir à partir d'une interface graphique. Lorsqu'elle est activée, "print" affiche les valeurs à la fois sur la sortie de l'interface graphique et sur la sortie de la fenêtre de commande.

### Compatibilité des Arguments de Fonction : `_loosecompatibility(bool v)`

Par défaut, un appel à une fonction qui nécessite des paramètres entiers ou des chaînes avec des arguments qui ne le sont pas échoue à la compilation. Cependant, il est possible de relâcher cette contrainte avec `_loosecompatibility(true)`. Alors, l'appel ci-dessus sera accepté et les arguments seront automatiquement convertis en leurs bonnes valeurs. Les nombres et les chaînes seront automatiquement convertis les uns en les autres.

### Gestion de la Mémoire : `_poolstats()`

Cette fonction est utilisée pour retourner l'état actuel des différents pools d'objets. Pour des raisons d'efficacité, certains objets sont gérés dans des pools, ce qui permet la réutilisation d'objets récurrents (comme les chaînes, les entiers, les flottants, les vecteurs ou les maps). Ces objets ne sont pas supprimés mais vidés et rendus disponibles pour une réutilisation ultérieure si nécessaire. Le résultat de cette fonction est un objet `mapsi`.

## Passage d'Arguments à un Programme Tamgu

Lorsqu'un programme Tamgu est appelé avec une liste d'arguments, chacun de ces arguments devient disponible pour le programme Tamgu via trois variables système spécifiques : `_args`, `_current` et `_paths`.

### Exemple :

```bash
tamgu monprogramme c:\test\montexte.txt
```

### `_args` : Vecteur d'Arguments

Tamgu fournit une variable spécifique appelée `_args`, qui est en fait un vecteur de chaînes de caractères qui stocke chaque argument selon sa position dans la liste de déclaration.

### Exemple (à partir de l'appel ci-dessus) :

```cpp
file f;
f.openread(_args[0]);
```

### `_paths` et `_current` : Gestion des Chemins

Tamgu fournit une variable vecteur appelée `_paths`, qui stocke les noms de chemin des différents programmes Tamgu qui ont été chargés. Le premier élément de ce vecteur, `_paths[0]`, stocke le nom de répertoire courant, tandis que `_paths[1]` stocke le chemin du fichier du programme en cours.

Tamgu fournit également une autre variable appelée `_current`, qui stocke le chemin du fichier de programme en cours d'exécution. Le chemin stocké dans `_current` se termine toujours par un séparateur final. En fait, `_current` pointe vers le même chemin que `_paths[1]`.

### `_endl` : Retour à la Ligne

Windows et Unix utilisent des caractères de retour à la ligne différents. `_endl` renvoie le retour à la ligne approprié selon la valeur de la plateforme.

### `_sep` : Séparateur dans les Noms de Chemin

Les systèmes basés sur Unix et Windows utilisent des séparateurs différents dans les noms de chemin entre les noms de répertoire. Unix nécessite un "/" tandis que Windows nécessite un "\". Tamgu fournit une variable spécifique appelée `_sep`, qui renvoie le séparateur approprié selon votre système.

## Console

Tamgu fournit une console par défaut, qui peut être utilisée pour charger et éditer n'importe quel programme. La console peut être utilisée pour tester de petits morceaux de code ou pour vérifier les valeurs à la fin d'une exécution.

Vous pouvez également exécuter un programme en mode débogage, qui affiche le contenu de la pile et des variables à chaque étape de votre programme.

Pour lancer la console, exécutez Tamgu avec l'option `-console`.