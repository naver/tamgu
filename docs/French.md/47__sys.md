# Fonctionnalités du système Tamgu

Tamgu offre des fonctionnalités système telles que la lecture du contenu d'un répertoire dans un vecteur ou l'exécution d'une commande système. Il expose la variable `_sys`, qui doit être utilisée pour accéder aux méthodes suivantes :

## Méthodes

1. `command(string s, string outputfile)`: Exécute la commande système `s`. `outputfile` est facultatif et est utilisé pour rediriger la sortie de la commande (stdout). Si `outputfile` est fourni, `command` retourne également le contenu de ce fichier sous forme de chaîne de caractères.

2. `createdirectory(string path)`: Crée un répertoire pour le `path` donné. Renvoie false si le répertoire existe déjà ou ne peut pas être créé.

3. `env(string var)`: Renvoie la valeur de la variable d'environnement `var`.

4. `env(string var, string value)`: Définit la valeur de la variable d'environnement `var`.

5. `listdirectory(string path)`: Renvoie les fichiers d'un répertoire sous forme de vecteur.

6. `ls(string path)`: Renvoie les fichiers d'un répertoire sous forme de vecteur.

7. `mkdir(string path)`: Crée un répertoire pour le `path` donné. Renvoie false si le répertoire existe déjà ou ne peut pas être créé.

8. `fileinfo(string path)`: Renvoie une map avec les informations suivantes pour un fichier donné :
   - `info["size"]`: taille du fichier
   - `info["date"]`: date du fichier
   - `info["change"]`: date de la dernière modification
   - `info["access"]`: date du dernier accès
   - `info["directory"]`: true si le chemin est un répertoire
   - `info["pathname"]`: le chemin réel

9. `pipe(string cmd)`: Exécute la commande `cmd` et renvoie un vecteur contenant le résultat de cette commande.

10. `realpath(string path)`: Renvoie le chemin réel pour un chemin relatif donné.

11. `getchar()`: Renvoie le dernier caractère saisi. `getchar` n'affiche pas les caractères à l'écran.

12. `colors(int style, int code1, int code2, bool disp)`: Définit la couleur du texte et renvoie la chaîne de couleur.

13. `foregroundcolor(int color)`: Définit la couleur du texte en premier plan.

14. `backgroundcolor(int color)`: Définit la couleur du texte en arrière-plan.

15. `rgbforegroundcolor(int red, int green, int blue)`: Définit la couleur du texte en premier plan avec des valeurs RGB.

16. `rgbbackgroundcolor(int red, int green, int blue)`: Définit la couleur du texte en arrière-plan avec des valeurs RGB.

17. `scrollmargin(int top, int bottom)`: Définit la zone de marge de défilement.

18. `deletechar(int nb)`: Supprime `nb` caractères.

19. `up(int nb)`: Déplace vers le haut `nb` lignes.

20. `down(int nb)`: Déplace vers le bas `nb` lignes.

21. `right(int nb)`: Déplace vers la droite `nb` caractères.

22. `left(int nb)`: Déplace vers la gauche `nb` caractères.

23. `next_line(int nb)`: Passe à la ligne suivante `nb` fois.

24. `previous_line(int nb)`: Passe à la ligne précédente `nb` fois.

25. `column(int nb)`: Passe à la colonne `nb`.

26. `row_column(int row, int column)`: Passe à la ligne/colonne.

27. `home()`: Déplace le curseur à la position d'accueil.

28. `cls()`: Efface l'écran et retourne à la position d'accueil.

29. `hor_vert(int hor, int vert)`: Passe à hor/vert.

30. `clearscreen(int nb)`: Efface l'écran. `nb` peut être 0, 1, 2 ou 3 pour un effacement partiel ou complet de l'écran.

31. `clear()`: Efface l'écran.

32. `eraseline(int nb)`: Efface `nb` lignes.

33. `scroll_up(int nb)`: Fait défiler vers le haut `nb` caractères.

34. `scroll_down(int nb)`: Fait défiler vers le bas `nb` caractères.

35. `screensizes()`: Renvoie la taille de l'écran en nombre de lignes et de colonnes.

36. `hasresized()`: Renvoie 'true' si la taille de l'écran a changé.

37. `cursor()`: Renvoie la position du curseur.

38. `inmouse()`: Active le suivi de la souris.

39. `outmouse()`: Désactive le suivi de la souris.

40. `ismouseaction(string key)`: Renvoie true s'il s'agit d'une action de souris.

41. `mousexy(string key)`: Renvoie la position de la souris.

42. `mousescrollup(string key)`: Renvoie la position de la souris lors du défilement vers le haut.

43. `mousescrolldown(string key)`: Renvoie la position de la souris lors du défilement vers le bas.

44. `mousedown1(string key)`: Renvoie la position de la souris lors du clic primaire.

45. `mousedown2(string key)`: Renvoie la position de la souris lors du clic secondaire.

46. `mouseup(string key)`: Renvoie la position de la souris lors du relâchement du bouton.

47. `mousetrack(string key)`: Renvoie la position de la souris lors du déplacement de la souris tout en maintenant le bouton enfoncé.

48. `reset()`: Réinitialise le mode souris et revient aux valeurs initiales de termios. À utiliser sur les machines Unix lorsque `getchar` a été utilisé.

49. `isescapesequence(string key)`: Renvoie true si `key` est une séquence d'échappement.

50. `showcursor(bool show)`: Affiche/masque le curseur.

51. `resizecallback(function f)`: Définit la fonction de rappel qui est appelée lorsque la fenêtre du terminal est redimensionnée. `f` est une fonction avec deux paramètres, qui sont les nouvelles tailles de lignes et de colonnes.

## Exemple

```javascript
// Cette fonction copie tous les fichiers d'un répertoire donné vers un autre, s'ils sont plus récents qu'une date donnée
function cp(string thepath, string topath) {
    // Nous lisons le contenu du répertoire source
    vector v = _sys.listdirectory(thepath);
    iterator it;
    string path;
    string cmd;
    map m;
    date t;
    // Nous définissons la date d'aujourd'hui à partir de 9 heures du matin
    t.setdate(t.year(), t.month(), t.day(), 9, 0, 0);
    it = v;
    for (it.begin(); it.nend(); it.next()) {
        path = thepath + '/' + it.value();
        // Si le fichier est du bon type
        if (".cxx" in path || ".h" in path || ".c" in path) {
            m = _sys.fileinfo(path);
            // Si la date est plus récente que notre date actuelle
            if (m["date"] > t) {
                // Nous le copions
                cmd = "copy " + path + ' ' + topath;
                println(cmd);
                // Nous exécutons notre commande
                _sys.command(cmd);
            }
        }
    }
}

// Nous appelons cette fonction pour copier d'un répertoire vers un autre
cp('C:\src', 'W:\src');
```

## Variables

Certaines variables sont également exposées par cette bibliothèque :

- `_sys_keyright` : flèche droite
- `_sys_keyleft` : flèche gauche
- `_sys_keydown` : flèche bas
- `_sys_keyup` : flèche haut
- `_sys_keydel` : touche Suppr
- `_sys_keyhomekey` : touche Home
- `_sys_keyendkey` : touche End
- `_sys_keyc_up` : Ctrl+flèche haut
- `_sys_keyc_down` : Ctrl+flèche bas
- `_sys_keyc_right` : Ctrl+flèche droite
- `_sys_keyc_left` : Ctrl+flèche gauche
- `_sys_keybackspace` : touche Retour arrière
- `_sys_keyescape` : touche Échap

Pour Windows, les variables suivantes ont été ajoutées :

- `_sys_keyc_homekey` : Ctrl+touche Home
- `_sys_keyc_endkey` : Ctrl+touche End

Ces variables peuvent être comparées à `getchar`, qui renvoie ces valeurs lorsqu'une touche est pressée.

## Exemple

```javascript
// Nous vérifions si un caractère a été saisi
string c = _sys.getchar();
println(c.bytes()); // Nous affichons son contenu

// Si l'utilisateur appuie sur la flèche haut, nous affichons toutes les couleurs possibles
if (c == _sys_keyup) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 99; j++) {
            for (int k = 0; k < 99; k++) {
                _sys.colors(i, j, k);
                println("test:", i, j, k);
            }
        }
    }
}
```

## Suivi de la souris

Vous pouvez également suivre la souris dans votre terminal. Voici un exemple :

```javascript
// Nous initialisons le suivi de la souris
_sys.inmouse();
string u;
u = _sys.getchar();
ivector iv;
while (u.ord() != 17) {
    if (_sys.ismouseaction(u)) {
        iv = _sys.mousedown1(u);
        if (iv != [])
            println("Clic gauche enfoncé à la position :", iv);
        iv = _sys.mousexy(u);
        if (iv != [])
            println("La souris est à la position :", iv);
        iv = _sys.mousescrollup(u);
        if (iv != [])
            println("La souris fait défiler vers le haut à la position :", iv);
        iv = _sys.mousescrolldown(u);
        if (iv != [])
            println("La souris fait défiler vers le bas à la position :", iv);
    }
    u = _sys.getchar();
}
// Nous désactivons le suivi de la souris et réinitialisons le terminal
_sys.reset();
```