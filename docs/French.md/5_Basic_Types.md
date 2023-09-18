## 5 Types de base

Tamgu nécessite que tous les éléments soient déclarés avec un type spécifique. Les types peuvent être prédéfinis ou définis par l'utilisateur en tant que frame.

### 5.1 Types prédéfinis

Tamgu propose quelques types très basiques :

#### Objets de base :
- self
- string
- int
- decimal
- float
- long
- bit
- short
- fraction
- bool
- date
- time
- call

#### Conteneurs :
- vector
- map
- imatrix
- fmatrix
- file
- iterator

#### Alias
Un alias est une fonction dans laquelle les arguments sont déclarés sans types. Un alias peut être utilisé pour accéder à une structure ou pour exécuter du code.

Note : Toutes les variables sont sujettes à modification dans un alias.

#### Fonction
Une fonction est déclarée n'importe où dans le code en utilisant le mot-clé `function`.

#### Frame
Un frame est un type défini par l'utilisateur qui est très similaire à une classe dans d'autres langages. Un frame contient autant de variables ou de définitions de fonctions que nécessaire.

### Déclaration de variable
Une variable est déclarée de manière similaire à de nombreux autres langages. Tout d'abord, le type de la variable est donné, suivi d'une liste de noms de variables séparés par des virgules et se terminant par un point-virgule.

Exemple :
```cpp
// Chaque variable peut être instantiée individuellement dans la liste
int i, j, k = 10;
string s1 = "s1", s2 = "s2";
private type name;
```

Une variable peut être déclarée comme privée, ce qui interdit son accès depuis des sources externes :
```cpp
private test toto;
```