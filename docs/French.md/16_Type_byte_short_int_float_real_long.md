## Types Numériques de Tamgu

Tamgu propose différents types numériques : byte, short, int, float, real, long et complex. Les types byte, short, int, float et real sont décrits dans la section suivante. Il convient de noter que real et float sont des alias l'un de l'autre. L'ajout du type real était nécessaire car float était souvent mal compris.

### Note d'implémentation en C++

Dans l'implémentation C++ de Tamgu, int et float (ou real) sont implémentés comme long et double, respectivement. Le type long est implémenté comme un entier sur 64 bits, spécifiquement __int64 sur Windows et "long long" sur les plates-formes Unix. Le type byte est implémenté comme un unsigned char et le type short est implémenté comme short.

## Méthodes Numériques

Tamgu propose les méthodes numériques suivantes :

1. `#()`: Renvoie le complément byte.
2. `abs()`: Renvoie la valeur absolue.
3. `acos()`: Renvoie le cosinus inverse.
4. `acosh()`: Renvoie le cosinus hyperbolique inverse.
5. `asin()`: Renvoie le sinus inverse.
6. `asinh()`: Renvoie le sinus hyperbolique inverse.
7. `atan()`: Renvoie la tangente inverse.
8. `atanh()`: Renvoie la tangente hyperbolique inverse.
9. `base(int b)`: Renvoie une chaîne représentant un nombre en base b.
10. `base(vector chrs)`: Définit l'encodage pour chaque chiffre dans une base donnée. L'ensemble par défaut est constitué de 64 caractères : 0-9, A-Z, a-z, #, @. Vous pouvez remplacer cet ensemble par défaut par le vôtre. Si vous fournissez un vecteur vide, le système réinitialise l'ensemble par défaut.
11. `bit(int ith)`: Renvoie vrai si le ième bit est égal à 1.
12. `cbrt()`: Renvoie la racine cubique.
13. `chr()`: Renvoie le caractère ASCII correspondant à ce nombre en tant que code.
14. `cos()`: Renvoie le cosinus.
15. `cosh()`: Renvoie le cosinus hyperbolique.
16. `emoji()`: Renvoie la description textuelle (en anglais) d'un emoji basée sur son code Unicode.
17. `erf()`: Renvoie la fonction d'erreur.
18. `erfc()`: Renvoie la fonction d'erreur complémentaire.
19. `even()`: Renvoie vrai si la valeur est paire.
20. `exp()`: Renvoie la fonction exponentielle.
21. `exp2()`: Renvoie la fonction exponentielle binaire.
22. `expm1()`: Renvoie l'exponentielle moins un.
23. `factors()`: Renvoie la décomposition en facteurs premiers sous forme de ivector.
24. `floor()`: Renvoie la valeur inférieure.
25. `format(string form)`: Renvoie une chaîne formatée selon le modèle spécifié dans form. (Ce format est le même que le format sprintf en C++).
26. `fraction()`: Renvoie la valeur sous forme de fraction.
27. `isemoji()`: Renvoie vrai si le code correspond à un emoji.
28. `lgamma()`: Renvoie la fonction log-gamma.
29. `log()`: Renvoie le logarithme naturel.
30. `log1p()`: Renvoie le logarithme plus un.
31. `log2()`: Renvoie le logarithme binaire.
32. `logb()`: Renvoie le logarithme en base flottante.
33. `nearbyint()`: Arrondi à la valeur entière la plus proche.
34. `odd()`: Renvoie vrai si la valeur est impaire.
35. `prime()`: Renvoie vrai si la valeur est un nombre premier.
36. `rint()`: Arrondi à la valeur entière la plus proche.
37. `round()`: Arrondi à la valeur la plus proche.
38. `sin()`: Renvoie le sinus.
39. `sinh()`: Renvoie le sinus hyperbolique.
40. `sqrt()`: Renvoie la racine carrée.
41. `tan()`: Renvoie la tangente.
42. `tanh()`: Renvoie la tangente hyperbolique.
43. `tgamma()`: Renvoie la fonction gamma.
44. `trunc()`: Renvoie la valeur tronquée à un entier.

### Spécifique aux Valeurs à Virgule Flottante

Pour les valeurs à virgule flottante (float et real), les méthodes suivantes sont disponibles :

45. `bits()`: Renvoie la représentation binaire sous-jacente d'une valeur à virgule flottante sous forme d'un entier (int ou long).
46. `bits(int v)`: Transforme v en une valeur à virgule flottante. v dans ce cas n'est pas une valeur entière, mais la représentation binaire interne d'une valeur à virgule flottante, telle que celle renvoyée par bits().
47. `exponent()`: Renvoie l'exposant d'une valeur à virgule flottante, telle que valeur = mantisse() * 2^exposant().
48. `mantissa()`: Renvoie la mantisse d'une valeur à virgule flottante.

## Liste Complète des Fonctions Mathématiques

Tamgu propose les fonctions mathématiques suivantes : abs, acos, acosh, asin, asinh, atan, atanh, cbrt, cos, cosh, erf, erfc, exp, exp2, expm1, floor, lgamma, ln, log, log1p, log2, logb, nearbyint, rint, round, sin, sinh, sqrt, tan, tanh, tgamma, trunc.

## Binaire

Un nombre binaire commence toujours par "0b". Tamgu le considère comme un nombre valide tant qu'il s'agit d'une chaîne binaire valide, ne contenant que des 0 et des 1. Par exemple, 0b1110 est égal à 14.

## Hexadécimal

Un nombre hexadécimal commence toujours par "0x". Tamgu le considère comme un nombre valide tant qu'il s'agit d'une chaîne hexadécimale valide. Une déclaration hexadécimale peut mélanger des caractères majuscules ou minuscules pour les chiffres hexadécimaux A, B, C, D, E et F.

## Opérateurs

Tamgu prend en charge divers opérateurs mathématiques et logiques :

- `+`, `-`, `*`, `/` : Opérateurs mathématiques
- `<<`, `>>`, `&`, `|`, `^` : Opérateurs logiques
- `%` : Modulo de division
- `^^` : Puissance (2^^2 = 4)
- `+=`, `-=`, etc. : Opérateurs d'affectation

## Sucre Syntaxique

Tamgu propose quelques notations de sucre syntaxique, rendant les opérations plus lisibles. Celles-ci incluent :

- `×`, `÷` peuvent être utilisés à la place de `*` et `/`
- `π`, `τ`, `φ`, `ℯ`, dont les valeurs sont respectivement 3.14159, 6.2831, 1.61803 et 2.71828
- `_pi`, `_tau`, `_phi`, `_e` sont des noms alternatifs pour les valeurs ci-dessus
- `²`, `³` pour le carré et le cube
- `√(`, `∛(` pour la racine carrée et la racine cubique
- Les expressions telles que `2a+b` ou `2(12+a)` sont prises en charge
- `a b` (avec un espace entre les deux) est équivalent à `a*b` ou `a×b`

## Exemple

```cpp
int h = 0xAB45; // Nombre hexadécimal
int i = 10;
float f = i.log(); // valeur = 1
f += 10; // valeur = 11
f = i % 5; // valeur = 0
f = 2 * i + 10; // 30
f = 2×i + 10; // 30
f = 2π + φ; // 7.90122
f = √(2i); // 4.47214
f = i²; // 100
f = 2(i-1); // 18

complex c(10, -2); // Nombre complexe en tant qu'objet de classe
println(c * 2); // 0c20: -4i
```

Les nombres complexes peuvent être ajoutés ou multipliés entre eux. Le nombre complexe expose deux méthodes :

1. `real()`: Renvoie la partie réelle du nombre complexe.
2. `imaginary(int v)`: Renvoie la partie imaginaire.

Un nombre complexe peut être déclaré de différentes manières :

1. En tant qu'objet de classe : `complex c(10, -2)` - La première partie est la valeur réelle (un float), et la deuxième partie est la partie imaginaire (également un float).
2. Par le biais d'une définition de nombre : `0c10:2i` - Le "0c" indique un nombre complexe, suivi de la partie réelle, puis de ":", et enfin de la partie imaginaire se terminant par "i".