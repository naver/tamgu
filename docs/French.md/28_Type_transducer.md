# Type Transducer

Ce type est axé sur le stockage et la manipulation de lexiques de manière très compacte et efficace. Il expose les méthodes suivantes :

## Méthodes

1. `add(container, bool norm, int encoding)`: Transforme un conteneur (vecteur ou map) en un lexique de transducteur. Si le conteneur est un vecteur, il doit avoir un nombre pair de valeurs.
2. `build(string input, string output, bool norm, int encoding)`: Construit un fichier de transducteur à partir d'un fichier texte contenant la forme de surface sur la première ligne et le lemme+caractéristiques sur la ligne suivante.
3. `compilergx(string rgx, svector features, string filename)`: Construit un fichier de transducteur à partir d'expressions régulières. Le nom de fichier est facultatif et l'automate résultant est stocké dans un fichier.
4. `load(string file)`: Charge un fichier de transducteur.
5. `lookdown(string lemFeat, byte lemma)`: Recherche une forme de surface qui correspond à un lemme plus des caractéristiques. Le lemme est facultatif. Lorsqu'il vaut 1 ou 2, la chaîne à comparer peut être réduite à un lemme uniquement. Si le lemme vaut 2, les caractéristiques sont également renvoyées. Remarque : le lemme doit être séparé des caractéristiques par une tabulation.
6. `lookup(string wrd, int threshold, int flags)`: Recherche un mot à l'aide d'un transducteur et d'un ensemble d'actions potentielles combinées à un seuil. Les deux derniers arguments peuvent être omis. Les actions disponibles sont :
   - `a_first`: L'automate peut appliquer des actions au premier caractère.
   - `a_change`: L'automate peut changer un caractère en un autre.
   - `a_delete`: L'automate peut supprimer un caractère.
   - `a_insert`: L'automate peut insérer un caractère.
   - `a_switch`: L'automate transpose deux caractères.
   - `a_nocase`: L'automate vérifie si les deux caractères peuvent correspondre indépendamment de leur casse.
   - `a_repetition`: L'automate accepte qu'un caractère soit répété plusieurs fois.
   - `a_vowel`: L'automate compare les voyelles sans accentuation ensemble. Par exemple, "e" correspondra à "é" ou "è" mais pas à "a".
   - `a_surface`: L'automate ne renvoie que la forme de surface.
7. `parse(string sentence, int option, int threshold, int flags)`: Analyse une séquence de mots à l'aide d'un transducteur. L'option peut prendre les valeurs suivantes :
   - `0`: Ne renvoie que les formes de surface qui ont été reconnues dans la chaîne initiale.
   - `1`: Identique à `0`, mais avec leurs décalages.
   - `2`: Renvoie les formes de surface et les lemmes avec leurs caractéristiques qui ont été reconnus dans l'entrée initiale.
   - `3`: Identique à `2`, mais avec leurs décalages. Le seuil et les indicateurs sont facultatifs et suivent la même convention que pour `lookup`.
8. `store(string output, bool norm, int encoding)`: Stocke un transducteur dans un fichier. Les deux derniers paramètres sont facultatifs.

## Format

Le format des fichiers compilés en lexiques, que ce soit par le biais de `build` ou de `add`, a une structure similaire. Dans le cas d'un fichier, la première ligne doit être une forme de surface, tandis que la ligne suivante doit être un lemme avec quelques caractéristiques, séparés par une tabulation.

Exemple :
```
classes
class +Plural+Noun
class
class +Singular+Noun
```

La fonction `build` prend un tel fichier en entrée et génère un fichier qui contient le transducteur correspondant basé sur ces lignes. Les deux autres paramètres sont utilisés lors du traitement d'un mot ou d'un texte.

- La normalisation signifie que le lexique peut correspondre à des mots sans tenir compte de la casse. Ainsi, ce lexique reconnaîtra "CLASS" comme un mot.
- Le système a été implémenté pour reconnaître les mots en encodage UTF8 (en fait, les transducteurs sont stockés en Unicode). Cependant, il est possible d'indiquer au système comment prendre en compte les encodages latins. Par exemple, vous pouvez fournir au système la valeur 5 en tant qu'encodage, qui dans ce cas fait référence au Latin 5, utilisé pour encoder les caractères cyrilliques. La valeur par défaut est Latin 1.

## Vecteur

Dans le cas d'un vecteur en tant qu'entrée pour `add`, la structure sera un peu différente. Les positions paires dans le vecteur seront la forme de surface, tandis que les positions impaires seront les lemmes plus leurs caractéristiques, à nouveau séparés par une tabulation.

## Map

Pour une map, la clé sera la forme de surface et la valeur sera les lemmes avec leurs caractéristiques. Une map peut en fait poser un problème pour stocker des mots ambigus.

## Traitement des chaînes de caractères

Il existe différentes façons de traiter les chaînes de caractères avec un transducteur.

### `lookup`

`lookup` est utilisé pour détecter si un mot appartient au transducteur et, dans ce cas, il renvoie son lemme et ses caractéristiques. Le transducteur peut renvoyer plus d'une solution. La variable destinataire doit être un vecteur si vous souhaitez récupérer toutes les solutions possibles.

Exemple :
```cpp
t.lookup("class") renvoie : class +Singular+Noun
```

Vous pouvez restreindre le traitement d'une chaîne de caractères avec une distance d'édition et des actions.

```cpp
t.lookup("cliss", 1, a_change) renvoie : class +Singular+Noun
```

### `lookdown`

`lookdown` est utilisé pour récupérer la forme de surface correcte d'un mot en utilisant son lemme et ses caractéristiques.

Exemple :
```cpp
t.lookdown("class +Plural+Noun") renvoie : classes
```

### `parse`

`parse` divise une chaîne de caractères en jetons et renvoie, pour chaque jeton, son lemme+caractéristiques sous la forme d'un vecteur de toutes les possibilités.

Exemple :
```cpp
transducer t(_current + "english.tra");
string sentence = "The lady drinks a glass of milk.";
vector v = t.parse(sentence);
printjln(v);
```

Sortie :
```
['The', 'The +0+3+0+3+Prop+WordParticle+Sg+NOUN', 'the +0+3+0+3+Det+Def+SP+DET']
['lady', 'lady +4+8+4+8+Noun+Sg+NOUN']
['drinks', 'drink +9+15+9+15+Verb+Trans+Pres+3sg+VERB', 'drink +9+15+9+15+Noun+Pl+NOUN']
['a', 'a +16+17+16+17+Det+Indef+Sg+DET']
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Verb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

Note : `parse` renvoie également la position de chaque mot dans la phrase initiale.

## Expressions régulières

Les expressions régulières traitées par le transducteur sont très limitées :

1. `%c` : Définit un caractère, où `c` est un caractère UTF8.
2. `$..` : Définit une chaîne de caractères.
3. `u-u` : Définit un intervalle entre deux caractères Unicode.
4. `[..]` : Définit une séquence de caractères.
5. `{...}` : Définit une disjonction de chaînes de caractères.
6. `.+` : La structure doit apparaître au moins une fois.
7. `(..)` : Définit une structure facultative.
8. `!n` : Insère une structure de caractéristiques avec son numéro dans le vecteur de caractéristiques (n>=1).

Exemples :
```cpp
transducer t;
// Cette expression reconnaît les chiffres romains
t.compilergx("{DMCLXVI}+!1", ["\t+Rom"]);
// Cette expression reconnaît tous les types de nombres, y compris le séparateur décimal et les expressions exponentielles.
t.compilergx("({-+}){0-9}+!1(%.{0-9}+!2({eE}({-+}){0-9}+!3))", ["+Card", "+Dec", "+Exp+Dec"]);
// Pour reconnaître les nombres ordinaux
t.compilergx("{[1st][2nd][3rd]}!1", ["+Ord"]);
t.compilergx("[3-9]([0-9]+)$th!1", ["+Ord"]);
// Nous voulons reconnaître toutes les chaînes composées de l'alphabet grec
t.compilergx("{α-ω0-9}+!1", ["+Greek"]);

int i;
string s;
for (i in <945,970,1>) s += i.chr();
println(t.lookup("MMMDDD")); // MMMDDD +Rom
println(t.lookup("1234")); // 1234 +Card
println(t.lookup("1.234")); // 1.234 +Dec
println(t.lookup("1.234e-8")); // 1.234e-8 +Exp+Dec
println(t.lookup("1st")); // 1st +Ord
println(t.lookup("2nd")); // 2nd +Ord
println(t.lookup("3rd")); // 3rd +Ord
println(t.lookup("4th")); // 4th +Ord
println(t.lookup(s)); // αβγδεζηθικλμνξοπρςστυφχψ +Greek
```