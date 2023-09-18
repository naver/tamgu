## Ligne de commande (plateformes Unix)

Vous pouvez utiliser Tamgu en tant qu'outil en ligne de commande sur les plateformes Linux ou Mac OS. Tamgu propose les commandes suivantes :

- **-i** : Cette commande vous permet de pré-déclarer des variables. Par exemple, `bool a,b,c; int i,j,k; float f,g,h; string s,t,u; map m; vector v; self x,y,z;`
- **-c** : Vous permet d'exécuter un morceau de code Tamgu fourni après la commande. Par exemple, `tamgu -c 'println(10+20);'`
- **-a** : Intercepte les envois sur stdio et les enregistre dans le vecteur _args. Il est possible de fournir un morceau de code.
- **-p** : Intercepte l'envoi ligne par ligne sur stdio et permet l'exécution d'un morceau de code où la variable pré-déclarée l contient la ligne stdio actuelle. Par exemple, `ls -l | tamgu -p 'println(l);'`
- Tamgu peut également être appelé avec un nom de programme et une liste d'arguments. Cette liste d'arguments est ensuite disponible dans _args. Par exemple, `tamgu mycode.tmg a1 a2 a3 a3`. _args contient : ["a1", "a2", "a3"]
- Il est également possible de combiner les lignes stdio et l'appel d'un programme avec ses arguments. L'option "-a" doit alors être placée après le nom du programme. Par exemple, `ls -1 | tamgu mycode.tmg -a a1 a2 a3`

## IDE intégré

Tamgu propose un environnement de développement intégré en mode texte que vous pouvez utiliser pour créer, modifier ou même déboguer vos programmes. Vous pouvez appeler cet environnement de deux manières différentes :

- Si vous exécutez "tamgu" sans arguments, vous pouvez ensuite taper la commande "edit" qui vous mettra en mode édition.
- Vous pouvez également lancer : "tamgu -e myfile.tmg" qui ouvrira votre fichier et vous mettra automatiquement en mode édition. Si "myfile.tmg" n'existe pas, vous pouvez le créer la première fois que vous l'enregistrez.

### Mode Édition/Exécution

Vous pouvez utiliser Tamgu soit pour éditer vos fichiers, soit pour les exécuter. Vous pouvez lancer l'exécution de votre fichier depuis l'éditeur avec "Ctrl-Xr". Une fois votre code exécuté, vous passerez en "mode exécution" où vous pourrez tester vos variables. Vous pouvez également passer en "mode exécution" avec "Ctrl-c". Pour revenir en "mode édition", tapez : edit.

### Aide

L'IDE propose de nombreuses commandes, dont l'aide en ligne suivante, qui peut être appelée soit avec la commande "help" en mode exécution, soit avec "Ctrl-Xh" en mode édition.

#### Commandes :

1. **Programmes** :
   - create filename : crée un espace de fichier avec un nom de fichier spécifique
   - open filename : charge un programme (utilisez run pour l'exécuter)
   - open : recharge le programme (utilisez run pour l'exécuter)
   - save filename : enregistre le contenu du tampon dans un fichier
   - save : enregistre le contenu du tampon avec le nom de fichier actuel
   - run filename : charge et exécute un programme filename
   - run : exécute le contenu du tampon
   - debug : mode de débogage
   - n! : ligne suivante
   - l! : affiche les variables locales
   - a! : affiche toutes les variables actives
   - s! : affiche la pile
   - g! : passe au point d'arrêt suivant
   - e! : exécute jusqu'à la fin
   - S! : arrête l'exécution
   - name : affiche le contenu d'une variable (il suffit de taper son nom)
   - h! : affiche l'aide de débogage
   - args v1 v2...vn : définit une liste de valeurs pour la liste d'arguments
   - filename : affiche le nom de fichier actuel

2. **Mode Ligne de Commande** :
   - help : affiche l'aide
   - help n : affiche l'une des sections d'aide (de 1 à 5)
   - history : affiche l'historique des commandes
   - load filename : charge un fichier d'historique de commandes
   - store filename : enregistre l'historique des commandes dans un fichier
   - filename : affiche le nom de fichier actuel
   - filespace : affiche tous les fichiers stockés en mémoire avec leur identifiant d'espace de fichier
   - select idx : sélectionne un espace de fichier
   - metas : affiche les méta-caractères
   - rm : efface le contenu du tampon
   - rm b:e : supprime les lignes de b à e (b: ou :e est également possible)
   - list : liste le contenu du tampon
   - list b:e : affiche le contenu du tampon de la ligne b à la ligne e (b: ou :e est également possible)
   - Ctrl-t : remonte dans la liste de code
   - Ctrl-g : descend dans la liste de code
   - Ctrl-f : force la ligne actuelle à être ajoutée à la fin du code
   - ?method : renvoie une description de 'method'
   - ?o.method : renvoie une description de 'method' pour la variable 'o'

3. **Édition (idx)** : mode d'édition. Vous pouvez éventuellement sélectionner également un espace de fichier
   - Ctrl-b : bascule le point d'arrêt
   - Ctrl-k : supprime du curseur jusqu'à la fin de la ligne
   - Ctrl-d : supprime une ligne entière
   - Ctrl-u : annule la dernière modification
   - Ctrl-r : rétablit la dernière modification
   - Ctrl-f : recherche une chaîne
   - Ctrl-n : recherche suivante
   - Ctrl-g : déplace vers une ligne spécifique, '$' est la fin du code
   - Ctrl-l : bascule entre le haut et le bas de l'écran
   - Ctrl-t : ré-indente le code
   - Ctrl-h : aide locale
   - Ctrl-w : écrit le fichier sur le disque
   - Ctrl-c : quitte l'éditeur
   - Ctrl-x : Commandes combinées
   - C : compte un motif
   - H : convertit les entités HTML en caractères Unicode
   - D : supprime un bloc de lignes
   - c : copie un bloc de lignes
   - x : coupe un bloc de lignes
   - v : colle un bloc de lignes
   - d : exécute en mode de débogage
   - r : exécute le code
   - w : écrire et quitter
   - l : charger un fichier
   - m : afficher les méta-caractères
   - h : aide complète
   - q : quitter

4. **Environnement** :
   - cls : efface l'écran
   - color : affiche les couleurs du terminal
   - color att fg bg : affiche une couleur à partir de l'attribut, du premier plan et du fond
   - colors : affiche les couleurs pour la coloration du code
   - colors name att fg bg : modifie la couleur pour l'une des dénominations suivantes = chaînes, méthodes, mots-clés, fonctions, commentaires

5. **Système** :
   - !unix : ce qui suit le '!' sera exécuté en tant que commande Unix (ex : !ls)
   - clear (idx) : efface l'environnement actuel ou un espace de fichier spécifique
   - reinit : efface le contenu du tampon et initialise les variables pré-déclarées
   - Ctrl-d : termine la session et quitte Tamgu
   - exit : termine la session et quitte Tamgu

6. **Expressions régulières pour 'find'** :
   - %d représente n'importe quel chiffre
   - %x représente un chiffre hexadécimal (abcdef0123456789ABCDEF)
   - %p représente toute ponctuation
   - %c représente une lettre minuscule
   - %C représente une lettre majuscule
   - %a représente n'importe quelle lettre
   - ? représente n'importe quel caractère
   - %? représente le caractère "?" lui-même
   - %% représente le caractère "%" lui-même
   - %s représente n'importe quel caractère d'espace, y compris l'espace insécable
   - %r représente un retour chariot
   - %n représente un espace insécable
   - ~ négation
   - \x caractère d'échappement
   - \ddd code de caractère sur 3 chiffres exactement
   - \xFFFF code de caractère sur 4 hexadécimaux exactement
   - {a-z} entre a et z inclus
   - [a-z] séquence de caractères
   - ^ l'expression doit commencer au début de la chaîne
   - $ l'expression doit correspondre jusqu'à la fin de la chaîne

#### Exemples :

- `dog%c` correspond à dogs ou dogg
- `m%d` correspond à m0, m1,...,m9
- `{%dab}` correspond à 1, a, 2, b
- `{%dab}+` correspond à 1111a, a22a90ab