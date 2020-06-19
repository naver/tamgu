# Un tableur en mode caractère avec formules en Lisp: TAMGUCALC

Ce tableur, extrêmement léger, se lance instantanément dans votre terminal. Il vous permet de saisir des nombres, de définir des labels mais surtout de rentrer des formules numériques en Lisp. Vous pouvez ensuite sauvegarder vos feuilles de calcul avec leurs formules dans un fichier CSV. Vous pouvez aussi exporter vos feuilles en ne conservant que les données brutes.

Le code fait moins de 700 lignes et est entièrement modifiable à votre convenance, si vous désirez rajouter vos propres options.

## Initialisation

Par défaut, la feuille utilise les dimensions de votre terminal pour définir le nombre de lignes et de colonnes.

### Taille des colonnes

Les colonnes ont par défaut une taille de 10 caractères. Pour changer cette taille, il vous faudra modifier la variable "columnsize" dans le code. Notez que vos feuilles de calcul sauvegardent cette taille ainsi que le nombre de lignes et de colonnes.


## Entrer une valeur

Pour rentrer une valeur, il suffit de se positionner sur une cellule avec les flèches et d'entrer votre valeur. Vous pouvez alors passer à la cellule suivante avec les flèches ou appuyer sur la touche "entrée". Une cellule peut contenir des chiffres, des formules ou des labels.

## Entrer une formule Lisp

Pour entrer une formule, il suffit de se positionner sur une cellule et de taper: '('. Lorsque votre formule est terminée, le système vérifie que les parenthèses s'équilibrent et il vous affiche le message: "Ready to compute". 

*Il suffit de presser: "entrée" pour qu'elle soit alors calculée.*

### Sélection d'une cellule

Lorsque vous tapez une formule, vous pouvez utiliser les flèches pour choisir les cases qui entrent dans votre formule. Lorsque vous avez sélectionné une cellule, vous pressez "entrée" pour l'enregistrer dans votre formule. 

### Sélection d'un domaine de cellules

Pour choisir un domaine de cellules, il vous faut d'abord choisir une première cellule, puis taper ".." (deux points) puis choisir une seconde cellule. tamgucalc remplace alors votre sélection automatiquement avec les règles suivantes:

Le choix d'une cellule apparaît sous la forme suivant:

* mat[i:j]: une seule cellule aux positions i,j.
* mat[:c][a:b]: 'c' est le numéro de colonne, 'a,b' sont les lignes de 'a' à 'b' sur la colonne 'c'.
* mat[l:][a:b]: 'l' est le numéro de ligne, 'a,b' sont les colonnes de 'a' à 'b' sur la ligne 'l'.

Notez, l'utilisation de "l:" et ":c" pour identifier lignes et colonnes.

**Important**: Les choix des colonnes et des lignes se traduisent sous la forme de vecteurs Tamgu, ce qui explique que dans les formules qui traduisent vos sélections, 'b' sera toujours "sélection + 1".

### Plusieurs lignes et colonnes

Notez que lorsque vous définissez un domaine qui comprend plusieurs lignes et colonnes, tamgucalc introduit un opérateur "&&&" afin de fusionner l'ensemble des lignes choisies en un seul vecteur de données. De cette façon, votre sélection devient un seul élément.

#### Exemple:

```Lisp
(sum (&&& mat[1:][1:3] mat[2:][1:3]))
```

## Formules

Les formules dans tamgucalc doivent être écrite dans le Lisp défini pour Tamgu ([voir la documentation](https://github.com/naver/tamgu/blob/master/docs/tamgu.pdf)). Vous pouvez définir des lambdas fonctions, des fonctions (defun) ou simplement utiliser les opérateurs de base.

**Important** Par défaut, toutes les valeurs numériques sont des "double" au sens C (type 'float' pour tamgu).

### Opérateurs

Les opérateurs traditionnels tels que : +,-,/, * sont disponibles.

Vous pouvez aussi utiliser: "sum" et "product". Notez que "+" est utilisé pour des valeurs atomiques et que "sum" s'applique à la fois sur les listes et les valeurs atomiques.

L'ensemble des méthodes Lisp définie dans Tamgu (voir la documentation) est d'ailleurs disponible, ainsi que les méthodes disponibles pour les listes, telle que "size" par exemple.

```Lisp
# addition
(+ mat[6:1] mat[7:1] mat[10:1] ) 

# Reste d'une division
(% mat[6:1] mat[7:1])

# logarithme Népérien
(ln mat[9:1] )

# Vous pouvez encapsuler vos formules
(* 2 (+ mat[6:1] mat[7:1] mat[10:1]))

# Appliquer une modification sur une liste et faire la somme
(sum (_map '(* 2) mat[:1][6:10])) 

```

### Lambdas

Vous pouvez aussi définir des lambdas fonctions dans vos cellules et effectuer tous les calculs voulus y compris des récursions. 

```Lisp

# Une fonction simple pour calculer la moyenne
((lambda (x) (/ (sum x) (size x))) mat[1:][1:10])

# On peut aussi récurser

((lambda (x) 
     (if (eq x ()) 
         0 
         (+ 
            (* 2 (car x)) 
            (self (cdr x))))) 
  mat[:1][6:10])

```

### Fonctions

Quand vous définissez une fonction avec "defun" dans une cellule telle que:

```Lisp
(defun avg(x) (/ (sum x) (size x)))
```

La cellule prend alors le nom de cette fonction. Vous pouvez désormais l'utiliser dans votre feuille comme nouvel opérateur:

```Lisp
(avg mat[:1][1:8])) 
 ```

**Attention**: Une fonction ne peut être définie qu'une fois. Lorsqu'une fonction a été enregistrée, elle ne pourra plus être modifiée. Pour prendre en compte la modification d'une fonction, il faut sauvegarder votre feuille de calcul, sortir de l'interpréteur et le relancer. Il vaut mieux dans un premier temps écrire cette fonction sous la forme d'une lambda avant de la rendre définitive sous la forme d'une fonction.

## Commandes de base

tamgucalc offre quelques options:

* Controle+s: sauvegarde
* Controle+w: sauvegarde sous 
* Controle+d: sauvegarde les données brutes
* Controle+r: permet de modifier le nombre de lignes et de colonnes
* Controle+q: permet de sortir



# Installer tamgucalc

Vous pouvez trouver "tamgucalc" à: [tamgucalc.tmg](https://github.com/naver/tamgu/blob/master/examples/miscelleaneous/tamgucalc.tmg)

Pour exécuter "tamgucalc" vous devez aussi vous procurer l'interpréteur "tamgu" dont des versions précompilées sont disponibles à: 

* **Mac OS**: [Version for macos](https://github.com/naver/tamgu/releases/tag/tamgu.macos)
* **Linux**: [Versions for linux:fedora, ubuntu, centos](https://github.com/naver/tamgu/releases/tag/tamgu.linux)


Pour lancer tamgucalc: *tamgu tamgucalc.tmg (fichier)*

Notez que fichier est optionnel...
 
