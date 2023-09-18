# Gestion de fichiers

Le type `file` est utilisé pour gérer les fichiers d'entrée et de sortie. Le type `wfile` est spécifiquement utilisé pour gérer les fichiers UTF16 (UCS-2).

## Méthodes

1. `eof()`: Renvoie true lorsque la fin du fichier est atteinte.
2. `file f(string filename, string mode_read)`: Ouvre un fichier selon le mode de lecture spécifié `mode_read`. Si le fichier est en mode lecture, `mode_read` est facultatif. Les valeurs possibles pour `mode_read` sont:
   - "a": ajouter
   - "r": lire
   - "w": écrire
   - "w+": ajouter
3. `find(string s, bool case_insensitive)`: Renvoie toutes les positions dans le fichier où la chaîne `s` est trouvée.
4. `get()`: Lit un caractère du fichier.
5. `get_signature()`: Renvoie si le fichier contient une signature.
6. `open_append(string filename)`: Ouvre un fichier en mode ajout.
7. `open_read(string filename)`: Ouvre un fichier en mode lecture.
8. `open_write(string filename)`: Ouvre un fichier en mode écriture.
9. `read()`: Lit tout le fichier dans une variable, qui peut être:
   - string: le document entier est stocké dans une seule chaîne
   - svector: le document est divisé en chaînes le long des retours chariot, chaque chaîne étant stockée dans le conteneur
   - bvector: le document est stocké octet par octet dans le conteneur
   - ivector: le document est stocké octet par octet dans le conteneur
10. `read(int nb)`: Similaire à `read()`, mais extrait seulement "nb" caractères ou octets du fichier.
11. `readln()`: Lit une ligne du fichier.
12. `seek(int p)`: Positionne le curseur du fichier à la position `p`.
13. `set_signature(bool s)`: Définit la signature UTF8 ou UTF16 en conséquence.
14. `tell()`: Renvoie la position du curseur du fichier.
15. `unget()`: Renvoie un caractère au flux.
16. `unget(nb)`: Renvoie `nb` caractères au flux.
17. `scan(string grammar, string separator)`: Lit un fichier avec une expression régulière Tangu et renvoie un vecteur de sous-chaînes. Le séparateur est un caractère qui sépare un champ d'un autre (par défaut, un espace).
18. `write(string s1, string s2, ...)`: Écrit la chaîne `s` dans le fichier.
19. `writelen(string s1, string s2, ...)`: Écrit des chaînes dans le fichier, séparant chaque chaîne par un espace et ajoutant un retour chariot à la fin de la ligne.
20. `writebin(int s1, int s2, ...)`: Écrit des octets dans le fichier. Si la valeur est un conteneur, il écrit la liste d'octets de ce conteneur.

## Signature

Les fichiers UTF-8 et UTF-16 peuvent avoir une signature au début. Un fichier UTF-8 a trois octets pour la signature, tandis qu'un fichier UTF-16 a deux octets.

- Lors de l'utilisation du type `file`, la signature doit être définie au préalable afin de la lire. Ce type ne peut être utilisé que pour lire des fichiers UTF-8 ou binaires.
- Dans le cas de `wfile`, la signature est automatiquement définie lorsqu'elle est trouvée au début du fichier. Ce type ne peut être utilisé que pour lire des fichiers UTF-16 (UCS-2).

## Opérateur

- `x in file`: Si `x` est une chaîne, elle reçoit une ligne du fichier. Si `x` est un vecteur, il ajoute la ligne en haut du vecteur. Si `x` est un entier ou un flottant, il récupère seulement un caractère du flux.

## Exemple

```cpp
file f;
f.open_read(path);
string s;
svector words;
string w;

for (s in f) { // Utilisation de l'opérateur in
    s = s.trim();
    words = s.split(" ");
    for (w in words)
        print("mot:", w, endl);
}

f.close();
```

## Entrée standard: stdin

Tamgu fournit la variable `stdin` pour gérer l'entrée standard. Cette variable est utile pour traiter les données provenant d'un fichier mis en pipe, par exemple.

### Exemple

```cpp
string s;
int i = 1;

for (s in stdin) {
    println(i, s);
    i++;
}
```

Si vous stockez ces lignes dans un petit fichier, par exemple `stdin.Tamgu`, alors le contenu des chaînes pipées sera affiché avec un numéro spécifique pour chaque ligne:

```bash
echo "La dame est heureuse" | Tamgu stdin.Tamgu
```

### Exemple (scan)

Le texte contient des lignes telles que:
```
456 -0x1.16bca4f9165dep -3 0x1.0d0e44bfeec9p -3
```

```cpp
file f(_current + "tst.txt");
// Définir une macro pour lire une chaîne hexadécimale complexe
grammar_macros("X", "({%+ -})0x%x+(%.%x+)(p({%+ -})%d+)");
// Lire les deux premiers champs. L'espace est le séparateur par défaut.
uvector v = f.scan("%d+ %X ");
println(v); // ['456', ' -0x1.16bca4f9165dep -3']
// Lire le champ suivant.
v = f.scan("%X ");
println(v); // ['0x1.0d0e44bfeec9p -3']
f.close();
```