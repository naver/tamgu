# Expression régulière Tamgu

Tamgu propose sa propre formalisation des expressions régulières appelée `treg`. `treg` est à la fois un type et une façon spécifique d'écrire des expressions régulières. En tant que type, il prend une chaîne de caractères en argument, qui suit la description ci-dessous. Mais il peut également être fourni directement sous forme de chaîne de caractères, auquel cas la chaîne est de la forme : `r"..."`, où le 'r' signifie expressions régulières.

## Méthodes

Le type `treg` expose deux méthodes :

1. `compile(string rgx)` : compile une chaîne de caractères en un objet `treg`.
2. `match(string s)` : vérifie si la chaîne `s` correspond à l'expression régulière Tamgu.

Exemple :

```cpp
treg tt("%C+");
if (tt.match("ABCDE"))
    println("Oui");
if (tt == "aABCDE")
    println("Oui");
else
    println("NON");
```

## Expression r

Vous pouvez également utiliser des chaînes `r` pour effectuer le même type d'opération :

```cpp
if (r"%C+" == "ABCDE")
    println("Oui");
```

Une expression `r` peut être écrite avec `""` ou `''` (guillemets doubles ou simples).

## Description de la langue

Une expression régulière Tamgu est une chaîne de caractères où des méta-caractères sont utilisés pour introduire une certaine liberté dans la description d'un mot. Ces méta-caractères sont :

- `%d` : représente n'importe quel chiffre
- `%x` : représente un chiffre hexadécimal (abcdef0123456789ABCDEF)
- `%p` : représente toute ponctuation appartenant à l'ensemble suivant : `< > { } [ ] ) , ; : . & | ! / \ = ~ # @ ^ ? + - * $ % ' _ ¬ £ €` “
- `%c` : représente toute lettre minuscule
- `%C` : représente toute lettre majuscule
- `%a` : représente toute lettre
- `?` : représente n'importe quel caractère
- `%?` : représente le caractère "?" lui-même
- `%%` : représente le caractère "%" lui-même
- `%s` : représente n'importe quel caractère d'espacement, y compris l'espace insécable
- `%r` : représente un retour chariot
- `%n` : représente un espace insécable
- `~` : négation
- `\x` : caractère d'échappement
- `\ddd` : code de caractère sur 3 chiffres exactement
- `\xFFFF` : code de caractère sur 4 hexadécimaux exactement
- `{a-z}` : entre a et z (inclus)
- `[a-z]` : séquence de caractères
- `^` : l'expression doit commencer au début de la chaîne
- `$` : l'expression doit correspondre jusqu'à la fin de la chaîne

Exemple :

- `dog%c` correspond à `dogs` ou `dogg`
- `m%d` correspond à `m0`, `m1`, ..., `m9`

### Opérateurs : `*`, `+`, `()`, `([])`

Une expression régulière peut utiliser la convention de l'étoile de Kleene pour définir des caractères qui se produisent plus d'une fois.

- `x*` : le caractère peut être répété 0 ou plusieurs fois
- `x+` : le caractère doit être présent au moins une fois
- `(x)` : le caractère est facultatif
- `[xyz](+*)` : une séquence de caractères
- `{xyz}(+*)` : une disjonction de caractères

Ici, `x` peut être un caractère ou un méta-caractère. Il y a un cas particulier avec `*` et `+`. Si le caractère qui doit être répété peut être n'importe quel caractère, alors on doit utiliser `%+` ou `%*`.

Important : Ces deux règles sont également équivalentes à `?*` ou `?+`.

Exemple :

1. `a*ed` correspond à `aed`, `aaed`, `aaaed`, etc. (le `a` peut être présent 0 ou plusieurs fois)
2. `a%*ed` correspond à `aed`, `aued`, `auaed`, `aubased`, etc. (n'importe quel caractère peut se trouver entre `a` et `ed`)
3. `a%d*` correspond à `a`, `a1`, `a23`, `a45`, `a765735`, etc.
4. `a{%d %p}` correspond à `a1`, `a/`, `a`, etc.
5. `a{bef}` correspond à `ab`, `ae` ou `af`
6. `a{%dbef}` correspond à `a1`, `ab`, `ae`, `af`, `a0`, `a9`, etc.
7. `a{be}+` correspond à `ab`, `ae`, `abb`, `abe`, `abbbe`, `aeeeb`, etc.
8. `a[be]+` correspond à `abe`, `abebe`, etc.

## Macros : `grammar_macros(key, pattern)`

Certaines expressions peuvent être un peu complexes à écrire. Tamgu fournit la procédure `grammar_macros`, qui crée un nouveau méta-caractère qui peut être utilisé dans les expressions. Le premier argument est un caractère qui sera utilisé comme index, tandis que le second est un motif d'expression régulière qui sera associé à cette clé.

Cette fonction fournit des motifs pour toutes les appels aux expressions régulières de grammaire.

Exemple :

```cpp
grammar_macros("X", "({%+ -})0x%x+(.%x+)(p({%+ -})%d+)");
string s = "ceci est : 0x1.0d0e44bfeec9p -3 dans la chaîne.";
// On utilise la macro
string res = s.scan("%X");
println(res); // 0x1.0d0e44bfeec9p -3
```

IMPORTANT : `grammar_macros` est une fonction système, ce qui signifie qu'elle est exécutée lors de l'analyse du code. Elle ne s'exécutera pas avec le reste du code une fois la compilation terminée.

## Utilisation de `treg` dans des chaînes de caractères

Il est également possible d'utiliser `treg` directement dans des chaînes de caractères pour extraire ou modifier leur contenu.

```cpp
string uu = "C'était : 0x1.0d0e44bfeec9p -3, 0x3.456aebp -1 ici.";
print("Tst : <", uu[-r"%d":" "], ">\n"); // Tst : <1>
println("Tst2 :", uu["x":r"%d %c"]); // Tst2 : x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

Notez qu'il est également possible de remplacer une expression `treg` par une variable de type `treg` :

```cpp
treg subt(r"%d %c");
println("Tst3 :", uu["x":subt]); // Tst3 : x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

## Taskell

Vous pouvez également utiliser ces expressions `treg` avec les fonctions de Taskell :

```cpp
<mytreg(r"%C+") = "majuscules">
<mytreg([r"%d+":v]) = "ok">
<mytreg(v) = "manqué">
println(mytreg("ABC")); // majuscules
println(mytreg(["3a4", 1, 4, 5])); // ok
```

## Prolog

Vous pouvez également les utiliser en Prolog :

```cpp
tst(r"%d+"). // l'expression `treg` est ici...
tst("machin").
truc("Ok", ?P) :- tst(?P).
vector vpred;
vpred = truc(?P, "machin");
println(vpred); // [truc("Ok",machin)]
vpred = truc(?P, "12");
println(vpred); // [truc("Ok",12)]
vpred = truc(?P, "non");
println(vpred); // []
```

## Expressions régulières POSIX : `preg`

Les expressions régulières POSIX sont également disponibles dans Tamgu, de la même manière que les expressions régulières Tamgu. Cependant, il y a deux différences mineures. Premièrement, pour utiliser ces expressions, vous devez préfixer votre expression avec `p` au lieu de `r`. Deuxièmement, le type d'objet associé est `preg`.

Ces expressions régulières sont basées sur le schéma d'expression régulière POSIX. Elles peuvent être utilisées à la place de `treg` partout, sauf pour la méthode `scan`, dans les chaînes de caractères et les fichiers.

Exemple :

```cpp
string str = "ce sujet a un sous-marin comme sous-séquence";
svector vs = p'\b(sub)([^ ]*)' in str;
println(vs); // ['sujet', 'sous-marin', 'sous-séquence']
ivector iv = p'\b(sub)([^ ]*)' in str;
println(iv); // [5, 12, 19, 28, 34, 45]
string s = p'\b(sub)([^ ]*)' in str;
println(s); // sujet
s = str[p"\b(sub)([^ ]*)"];
println("S=" + s); // S=sujet
s = str[-p"\b(sub)([^ ]*)"];
println("S=" + s); // S=sous-séquence
```