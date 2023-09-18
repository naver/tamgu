# Manipulation de chaînes de caractères dans Tamgu

Tamgu propose plusieurs types pour manipuler les chaînes de caractères : `string`, `ustring` et `rawstring`.

- Le type `string` est utilisé pour manipuler tout type de chaîne de caractères. Il offre de nombreuses méthodes différentes pour extraire des sous-chaînes, des caractères ou appliquer une reconnaissance de motifs dessus.
- Le type `ustring` est utilisé pour offrir un accès plus rapide à des chaînes de caractères très grandes, car le système suppose une seule et unique encodage pour toute la chaîne. Il est basé sur l'implémentation `wstring` en C++.
- Le type `rawstring` gère les chaînes de caractères au niveau des octets. Il accepte les chaînes de caractères mais les gère au niveau des octets. Il ne nécessite pas de protection spécifique dans les threads et peut être accédé et modifié librement. Cependant, vous ne pouvez pas redimensionner une `rawstring` si des threads s'exécutent en arrière-plan.

## Méthodes

### Conversion de base

- `base(int b, bool toconvert)`: Retourne la valeur numérique correspondant au contenu numérique de la chaîne en base `b`. Le paramètre `toconvert` est facultatif. Lorsqu'il est `false`, le nombre à convertir est en base 10 et est converti en base `b`.
- `base(vector chrs)`: Définit l'encodage de chaque chiffre dans une base donnée. L'ensemble par défaut est composé de 64 caractères : 0-9, A-Z, a-z, #, @. Vous pouvez remplacer cet ensemble de caractères par le vôtre. Si vous fournissez un vecteur vide, le système réinitialise l'ensemble de caractères par défaut.

### Extraction et manipulation de chaînes

- `bytes()`: Retourne un ivector d'octets correspondant à la chaîne.
- `charposition(int pos)`: Convertit une position d'octet en position de caractère (particulièrement utile dans les chaînes UTF8).
- `deaccentuate()`: Supprime les accents des caractères accentués.
- `doublemetaphone()`: Retourne un svector contenant l'encodage double-metaphone de la chaîne.
- `dos()`: Convertit une chaîne en encodage DOS.
- `dostoutf8()`: Convertit une chaîne DOS en encodage UTF8.
- `emoji()`: Retourne la description textuelle (en anglais) d'un emoji.
- `evaluate()`: Évalue les méta-caractères dans une chaîne et retourne la chaîne évaluée.
- `extract(int pos, string from, string up1, string up2...)`: Retourne un svector contenant toutes les sous-chaînes de la chaîne actuelle, à partir de la position `pos`, qui sont composées de `from` jusqu'à l'une des prochaines chaînes `up1`, `up2`, etc.
- `fill(int nb, string char)`: Crée une chaîne de `nb` caractères.
- `find(string sub, int pos)`: Retourne la position de la sous-chaîne `sub` à partir de la position `pos`.
- `format(p1, p2, p3)`: Crée une nouvelle chaîne à partir de la chaîne actuelle dans laquelle chaque '%x' est associé à l'un des paramètres, 'x' étant la position de ce paramètre dans la liste d'arguments. 'x' commence à 1.
- `geterr()`: Capture la sortie d'erreur actuelle.
- `getstd()`: Capture la sortie standard actuelle.
- `html()`: Retourne la chaîne sous forme d'une chaîne compatible avec HTML ou d'un vecteur de chaînes.
- `insert(i, s)`: Insère la chaîne `s` à la position `i`. Si `i` est -1, alors insère `s` entre chaque caractère de la chaîne d'entrée.
- `isalpha()`: Vérifie si une chaîne ne contient que des caractères alphabétiques.
- `isconsonant()`: Vérifie si une chaîne ne contient que des consonnes.
- `isdigit()`: Vérifie si une chaîne ne contient que des chiffres.
- `isemoji()`: Vérifie si une chaîne ne contient que des emojis.
- `islower()`: Vérifie si une chaîne ne contient que des caractères en minuscules.
- `ispunctuation()`: Vérifie si une chaîne est composée de signes de ponctuation.
- `isupper()`: Vérifie si une chaîne ne contient que des caractères en majuscules.
- `isutf8()`: Vérifie si une chaîne contient des caractères UTF8.
- `isvowel()`: Vérifie si une chaîne ne contient que des voyelles.
- `last()`: Retourne le dernier caractère.
- `latin()`: Convertit une chaîne UTF8 en LATIN.
- `left(int nb)`: Retourne les `nb` premiers caractères d'une chaîne.
- `levenshtein(string s)`: Retourne la distance d'édition avec `s` selon l'algorithme de Levenshtein.
- `parenthetic()`: Convertit une expression parenthétique en un vecteur.
- `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: Convertit une expression récursive en utilisant les caractères d'ouverture et de fermeture comme séparateurs.
- `lower()`: Retourne la chaîne en minuscules.
- `mid(int pos, int nb)`: Retourne les `nb` caractères à partir de la position `pos` d'une chaîne.
- `ngrams(int n)`: Retourne un vecteur de tous les n-grammes de rang `n`.
- `ord()`: Retourne le code d'un caractère de chaîne.
- `parse()`: Analyse une chaîne en tant que morceau de code et retourne l'évaluation dans un vecteur.
- `pop()`: Supprime le dernier caractère.
- `pop(i)`: Supprime le caractère à la position `i`.
- `read(string file)`: Lit un fichier dans la chaîne.
- `removefirst(int nb)`: Supprime les `nb` premiers caractères d'une chaîne.
- `removelast(int nb)`: Supprime les `nb` derniers caractères d'une chaîne.
- `replace(sub, str)`: Remplace les sous-chaînes correspondant à `sub` par `str`.
- `reverse()`: Inverse la chaîne.
- `rfind(string sub, int pos)`: Retourne la position de la sous-chaîne `sub` en partant de la fin à partir de la position `pos`.
- `right(int nb)`: Retourne les `nb` derniers caractères d'une chaîne.
- `scan(rgx, string sep, bool immediatematch, string remaining)`: Retourne toutes les sous-chaînes correspondant à `rgx` selon la formalisme d'expression régulière de Tamgu.
- `size()`: Retourne la longueur d'une chaîne.
- `slice(int n)`: Découpe une chaîne en sous-chaînes de taille `n`.
- `split(string splitter)`: Divise une chaîne le long de `splitter` et stocke les résultats dans un svector.
- `splite(string splitter)`: Divise une chaîne de la même manière que `split` ci-dessus, mais conserve les chaînes vides dans le résultat final.
- `multisplit(string sp1, string sp2...)`: Divise une chaîne le long de plusieurs chaînes séparatrices.
- `stokenize(map keeps)`: Tokenise une chaîne en mots et ponctuations.
- `tags(string o, string c, bool comma, bool separator, bool keepwithdigit, svector rules)`: Analyse une chaîne en tant qu'expression parenthétique, où les chaînes d'ouverture et de fermeture sont fournies.
- `tokenize(bool comma, bool separator, svector rules)`: Tokenise une chaîne en mots et ponctuations.
- `trim()`: Supprime les caractères de fin.
- `trimleft()`: Supprime les caractères de fin à gauche.
- `trimright()`: Supprime les caractères de fin à droite.
- `upper()`: Retourne la chaîne en majuscules.
- `utf8()`: Convertit une chaîne LATIN en UTF8.
- `utf8(int part)`: Convertit une chaîne latine encodée en partie ISO 8859 `part` en UTF8.
- `write(string file)`: Écrit le contenu de la chaîne dans un fichier.

## Manipulation de chaînes

Tamgu propose également des méthodes spécifiques pour les chaînes coréennes et les tables latines. Ces méthodes sont utilisées avec les types `string` et `ustring`.

### Méthodes spécifiques à la langue coréenne

- `ishangul()`: Retourne `true` s'il s'agit d'un caractère coréen.
- `isjamo()`: Retourne `true` s'il s'agit d'un jamo coréen.
- `jamo(bool combine)`: Si `combine` est `false` ou absent, divise un jamo coréen en ses composants principaux de consonne et de voyelle. Si `combine` est `true`, combine le contenu en un jamo.
- `normalizehangul()`: Normalise les différentes encodages UTF8 des caractères coréens.
- `romanization()`: Romanisation des caractères coréens.

### Table latine

Tamgu fournit des noms d'encodage et des constantes pour accéder à ces encodages. Les noms d'encodage sont :

- `e_latin_we` : Europe de l'Ouest
- `e_latin_ce` : Europe centrale
- `e_latin_se` : Europe du Sud
- `e_latin_ne` : Europe du Nord
- `e_cyrillic` : Cyrillique
- `e_arabic` : Arabe
- `e_greek` : Grec
- `e_hebrew` : Hébreu
- `e_turkish` : Turc
- `e_nordic` : Nordique
- `e_thai` : Thaï
- `e_baltic` : Région balte
- `e_celtic` : Celtique
- `e_latin_ffe` : Étendu (français, finnois, estonien)
- `e_latin_see` : Europe du Sud-Est
- `e_windows` : Encodage Windows

## Opérateurs

Tamgu prend en charge plusieurs opérateurs pour la manipulation de chaînes :

- `sub in s` : Vérifie si `sub` est une sous-chaîne de `s`.
- `for (c in s) {…}` : Parcourt tous les caractères de `s`.
- `+` : Concatène deux chaînes.
- `"…"` : Définit une chaîne, où les méta-caractères tels que `\n`, `\t`, `\r`, `\"` sont interprétés.
- `'…'` : Définit une chaîne, où les méta-caractères ne sont pas interprétés.

## Index

Tamgu permet l'indexation et la modification des chaînes en utilisant des index :

- `str[i]` : Retourne le `i`-ème caractère d'une chaîne.
- `str[i:j]` : Retourne la sous-chaîne entre `i` et `j`.
- `str[s..]` : Retourne la sous-chaîne à partir de la chaîne `s`.
- `str[-s..]` : Retourne la sous-chaîne à partir de la chaîne `s` depuis la fin de la chaîne.

Si un index est hors limites, une exception est levée à moins que le drapeau `erroronkey` n'ait été défini sur `false`.

## Exemples

Voici quelques exemples de manipulations de chaînes dans Tamgu :

```tamgu
string s;
string x;
vector v;

// Quelques manipulations de base de chaînes
s = "12345678a";
x = s[0];  // valeur=1
x = s[2:3];   // valeur=3
x = s[2:-2];  // valeur=34567
x = s[3:];  // valeur=45678a
x = s[:"56"];  // valeur=123456
x = s["2":"a"];  // valeur=2345678a
s[2] = "ef";  // valeur=vide

// Les 3 derniers caractères
x = s.right(3);  // valeur=78a

// Une division le long d'un espace
s = "a b c";
v = s.split(" ");  // v=["a", "b", "c"]

// Regex, x est une chaîne, nous recherchons la première correspondance de l'expression régulière
x = s.scan("%d%d%c");  // valeur=78a

// Nous avons un motif, nous divisons notre chaîne le long de ce motif
s = "12a23s45e";
v = s.scan("%d%d%c");  // valeur=['12a', '23s', '45e']

x = s.replace(r"%d%ds", "X"); // valeur=12aX45e

// Replace accepte également les variables %x comme dans les expressions régulières de Tamgu
x = s.replace(r"%d%1s", "%1"); // valeur=12a2345e

// Expressions régulières : Non disponibles sur toutes les plates-formes
preg rgx(p"\w+day");
string str = "Yooo Wesdenesday Saturday";
vector vrgx = rgx in str; // ['Wesdenesday', 'Saturday']
string s = rgx in str; // Wesdenesday
int i = rgx in str; // la position est 5

// Nous utilisons (…) pour isoler des jetons spécifiques qui seront stockés dans le vecteur
rgx = p"(\d{1,3}):( \d{1,3}):( \d{1,3}):( \d{1,3})";
str = '1:22:33:444';
vrgx = str.split(rgx);  // [1, 22, 33, 444], rgx est une expression de division

str = '1:22:33:4444';
vrgx = str.split(rgx);  // [] (4444 contient 4 chiffres)

str = "A_bcde";
// Correspondance complète requise
if (p'[a-zA-Z]_.+' == str)
    println("Yooo"); // Yooo

// C'est également équivalent à :
rgx = p'[a-zA-Z]_.+';
if (rgx.match(str))
    println("Yooo bis");

str = "ab(Tamgu 12,Tamgu 14,Tamgu 15,Tamgu 16)";
v = str.extract(0, "Tamgu ", ",", ")");
// Résultat : ['12', '14', '15', '16']

string frm = "this %1 is a %2 of %1 with %3";
str = frm.format("tst", 12, 14);
println(str); // Résultat : this tst is a 12 of tst with 14
```