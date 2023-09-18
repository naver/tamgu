# Tamgu Contextuel

## 12.1 Tamgu est un langage de programmation contextuel.

Dans Tamgu, la manière dont une variable est traitée dépend de son contexte d'utilisation. Lorsque deux variables sont utilisées ensemble à travers un opérateur, le résultat de l'opération dépend du type de la variable à gauche, qui introduit l'opération. Dans le cas d'une affectation, le type de la variable réceptrice détermine le type de l'ensemble du groupe.

### Exemple

Si nous déclarons deux variables, une chaîne de caractères et un entier, alors l'opérateur "+" peut agir comme une concaténation ou une opération arithmétique.

a) Dans ce cas, "i" est la variable réceptrice, ce qui rend l'instruction entière une opération arithmétique.

```
int i=10;
string s="12";
i=s+i;       // le "s" est automatiquement converti en nombre.
print("I="+i+"\n");
```

Sortie:
```
I=22
```

b) Dans cet autre cas, "s" est la variable réceptrice. L'opération est maintenant une concaténation.

```
int i=10;
string s="12";
s=s+i;       // le "i" est automatiquement converti en chaîne de caractères.
print("S="+s+"\n");
```

Sortie:
```
S=1210
```

## 12.2 Conversion implicite

La notion de contexte est très importante dans Tamgu car elle définit comment chaque variable doit être interprétée. Les conversions implicites sont effectuées automatiquement pour certains types. Par exemple, un entier est automatiquement transformé en une chaîne de caractères, avec ses propres chiffres comme valeur. Dans le cas d'une chaîne de caractères, le contenu est transformé en un nombre si la chaîne ne contient que des chiffres, sinon il est 0.

Pour des cas plus spécifiques, tels qu'un vecteur ou une map, les conversions implicites peuvent être un peu plus complexes. Par exemple, un vecteur en tant qu'entier renvoie sa taille, et en tant que chaîne de caractères, une représentation du vecteur. Un fichier en tant que chaîne de caractères renvoie son nom de fichier, et en tant qu'entier, sa taille en octets.