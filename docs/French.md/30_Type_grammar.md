# Type Grammar

Le type `grammar` est conçu pour fournir aux programmeurs un moyen puissant de décrire des structures de chaînes de caractères complexes. Il est particulièrement utile pour détecter des sous-chaînes spécifiques dans un texte qui impliquent des chiffres, des lettres majuscules ou des ponctuations dans un ordre strict.

## Méthodes

Il existe deux fonctions exposées par ce type :

1. `apply(string|vector)`: Applique une grammaire à un texte, le transformant en un vecteur de caractères ou un vecteur de jetons.
2. `load(rule, int skipblanks)`: Charge des règles en tant que chaîne ou vecteur de règles. Le paramètre `skipblanks` est facultatif et peut avoir les valeurs suivantes :
   - 0 : Tous les caractères sont pris en compte dans la grammaire. C'est la valeur par défaut lorsque `skipblanks` est omis.
   - 1 : Les espaces et les tabulations sont automatiquement ignorés avant d'appliquer une cible aux sous-chaînes. Les caractères de fin à la fin des chaînes sont également ignorés.
   - 2 : Tous les espaces, y compris les sauts de ligne, sont ignorés.

L'opérateur "in" peut également être utilisé avec une grammaire pour détecter si une chaîne est compatible avec la grammaire.

## Règles

Les règles peuvent être implémentées soit sous forme de texte unique, soit sous forme de vecteur de chaînes, où chaque chaîne représente une règle.

### Format des règles

Le format d'une règle est le suivant :

```
tête := (~) élément [,;] élément .
```

où `élément` peut être :
- Une chaîne de caractères : Encadrée de guillemets ou d'apostrophes.
- ? : Tout caractère.
- %a : Tout caractère alphabétique.
- %c : Tout caractère en minuscule.
- %C : Tout caractère en majuscule.
- %d : Un chiffre.
- %e : Un caractère emoji.
- %H : Un caractère Hangul.
- %r : Un saut de ligne.
- %s : Un caractère d'espace.
- %S : Un caractère de séparation (espace ou saut de ligne).
- %p : Une ponctuation.
- %? : Le caractère "?".
- %% : Le caractère "%".
- 0, 1, 2, ..., 9 : Tout chiffre, représenté par son code de caractère.
- $chaîne : Une chaîne de longueur quelconque (identique à "chaîne").
- tête : La tête d'une autre règle.

Négation : Tous les éléments peuvent être négatifs avec "~", sauf les têtes.

Disjonction : Utilisez ";" lorsque vous avez besoin d'une disjonction entre deux éléments, "," sinon.

Étoile de Kleene : Utilisez "+" ou "*" pour boucler sur chacun de ces éléments.

Correspondance la plus longue : Si vous utilisez "++" ou "**", la boucle consommera la chaîne jusqu'à l'élément le plus accessible.

Facultatif : Utilisez "(élément)" pour les caractères ou les têtes facultatifs.

Toutes les règles doivent se terminer par un ".".

Lorsqu'un nom de tête commence par un "_", la chaîne est extraite mais son étiquette n'est pas stockée.

Cas spécifiques :
- ?_ : Tout caractère, mais non stocké.
- %a_ : Tout caractère alphabétique, mais non stocké.
- %c_ : Tout caractère en minuscule, mais non stocké.
- %C_ : Tout caractère en majuscule, mais non stocké.
- %d_ : Un chiffre, mais non stocké.
- %e_ : Un emoji, mais non stocké.
- %H_ : Un caractère Hangul, mais non stocké.
- %r_ : Un saut de ligne, mais non stocké.
- %s_ : Un caractère d'espace, mais non stocké.
- %S_ : Un caractère de séparation, mais non stocké.
- %p_ : Une ponctuation, mais non stockée.
- label_ : Un appel à une règle sans stockage.

L'adjonction d'un "_" à la fin de ces options permet de reconnaître un caractère ou un groupe de caractères qui n'est pas stocké dans le résultat final.

### Exemple

```python
# Cette grammaire reconnaît un mot ou un nombre, seulement pour une seule chaîne...
string r = @"
bloc := mot;nombre.
mot := %a+.
nombre := %d+.
"@;

# Nous chargeons notre grammaire
grammar g(r);

# Nous l'appliquons à la chaîne "the"
map m = g.apply("the")
# Sortie : {'bloc':[{'mot':['the']}]}

m = g.apply("123")
# Sortie : {'bloc':[{'nombre':['123']}]}

# Cependant, si nous appliquons cette grammaire à "Test 123", cela échouera. Nous devons ajouter
# deux choses à la grammaire :
# a) Tenir compte des espaces
# b) Boucler pour reconnaître chaque jeton dans la chaîne
string r = @"
base := bloc+.
bloc := mot;nombre;%s.
mot := %a+.
nombre := %d+.
"@;

# Nous avons ajouté une nouvelle disjonction avec %s pour tenir compte des espaces. Ensuite,
# nous avons ajouté une règle "base" qui boucle sur bloc.
m = g.apply("Test 123")
# Sortie : {'base':[{'bloc':[{'mot':['Test']}]},{'bloc':[' ']},{'bloc':[{'nombre':['123']}]}]}

# Nous pouvons utiliser l'opérateur "_" pour supprimer les informations inutiles de la sortie, telles que "bloc".
string r = @"
base := _bloc+.
_bloc := mot;nombre;%s.
mot := %a+.
nombre := %d+.
"@;

m = g.apply("Test 123")
# Sortie : {'base':[{'mot':['Test']},' ',{'nombre':['123']}]}
```

## Sous-grammaires

Les sous-grammaires sont introduites entre "[...]". Dans ces crochets, il est possible de définir une disjonction de chaînes de caractères d'expressions régulières. Ces expressions sont particulièrement utiles lors de l'application d'une grammaire à un vecteur de chaînes de caractères, où les chaînes de caractères peuvent être mises en correspondance au niveau des caractères avec l'expression elle-même. Chaque expression doit être séparée de la suivante par un "|". Vous ne pouvez pas appeler une règle à partir de crochets, donc une chaîne telle que "dog" sera équivalente à "$dog".

### Exemple

```python
string dico = @"
test := %a, wrd,%a.
wrd := [%C,(" -"),%c+|test|be|dog|cat].
"@;

grammar g(dico);
ustring s = "The C -at drinks";
uvector v = s.tokenize();
vector res = g.apply(v);
println(res);
```

## Vecteur vs Map

Si la variable destinataire est remplacée par un vecteur, la structure de sortie est différente. Le nom de la règle de tête est inséré dans la structure finale en tant que premier élément. Par exemple, si nous appliquons la grammaire à la même chaîne mais avec un vecteur en sortie, nous obtenons : `['base',['mot','Test'],' ',['nombre','123'],' ',['code','T234e']]`.

## Entrée sous forme de chaîne ou de vecteur

Si l'entrée est une chaîne, chaque caractère détecté est ajouté à la chaîne de sortie. Cependant, si l'entrée est un vecteur de caractères, le résultat de sortie est conservé sous forme de vecteur de caractères.

### Exemple

```python
# Cette grammaire reconnaît un mot ou un nombre
string r = @"
base := _bloc+.
_bloc := code;mot;nombre;%s.
mot := %a+.
nombre := %d+.
code := %C,%d+,%c.
"@;

# Nous chargeons notre grammaire
grammar g(r);

# Nous divisons une chaîne en un vecteur de caractères
string s = "Test 123 T234e";
svector vs = s.split("");

# Nous appliquons la grammaire au vecteur de caractères
vector v = g.apply(vs);
println(v);
```

## Fonction

Il est possible d'associer une fonction à une grammaire. La signature de la fonction est la suivante : `function grammarcall(string head, self structure, int pos)`. Cette fonction est appelée pour chaque nouvelle structure calculée pour une tête donnée. Si cette fonction renvoie false, alors l'analyse de cette règle échoue. `pos` est la dernière position dans la chaîne jusqu'à laquelle l'analyse syntaxique a eu lieu.

### Exemple

```python
# Cette grammaire reconnaît un mot ou un nombre
string r = @"
base := _bloc+.
_bloc := code;mot;nombre;%s.
mot := %a+.
nombre := %d+.
code := %C,%d+,%c.
"@;

# Cette fonction est appelée pour chaque nouvelle règle qui réussit
function callgrm(string head, self v, int ps) {
    println(head,v,ps);
    return true;
}

# Nous chargeons notre grammaire
grammar g(r) with callgrm;

# Nous divisons une chaîne en un vecteur de caractères
string s = "Test 123 T234e";

# Nous appliquons la grammaire au vecteur de caractères
map m = g.apply(s);
println(m);
```

## Modification de la structure

La structure peut également être modifiée dans la fonction associée à la grammaire, mais il convient d'être prudent lors des modifications.

### Exemple

```python
function callgrm(string head, self v, int ps) {
    # Si la tête est un mot, nous modifions la chaîne interne
    if (head == "mot") {
        println(head,v);
        v[0] += "_aword";
    }
    return true;
}
```

## Depuis une règle

Une fonction peut également être appelée depuis une règle. La signature est la suivante : `function rulecall(self structure, int pos)`.

### Exemple

```python
# Cette grammaire reconnaît un mot ou un nombre
string r = @"
base := _bloc+.
_bloc := code;mot;nombre;%s.
mot := %a+.
nombre := %d+.
code := %C,%d+,%c,callcode.
"@;

# Nous chargeons notre grammaire
grammar g(r);

# Nous divisons une chaîne en un vecteur de caractères
string s = "Test 123 T234e";

# Nous appliquons la grammaire au vecteur de caractères
map m = g.apply(s);
println(m);
```

## Analyse HTML

```python
# evaluate est une méthode de base pour remplacer chaque entité HTML par sa contrepartie UTF8.
function evalue(self s, int p) {
    s[1] = s[1].evaluate();
    return true;
}

# C'est notre grammaire HTML
# Nous ne conservons pas les caractères d'espace entre les balises, d'où : %s_ dans object
string htmlgrm = @"
html := _object+.
_object := tag;%s_;text.
tag := "<",?+,">".
text := _characters,evalue.
_characters := ~"<"+.
"@;

# Nous compilons notre grammaire
grammar ghtml(htmlgrm);

# Nous appliquons la grammaire à un texte HTML
vector rgram = ghtml.apply(html_text);
```