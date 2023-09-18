# Types prédéfinis

Tamgu propose de nombreux objets différents, chacun avec un ensemble spécifique de méthodes. Chacun de ces objets est livré avec une liste de méthodes prédéfinies.

## Méthodes de base

Tous les types ci-dessous partagent les mêmes méthodes de base :

a) `isa(typename)`: Vérifie si une variable a le type `typename` (en tant que chaîne de caractères).

b) `type()`: Renvoie le type d'une variable en tant que chaîne de caractères.

c) `methods()`: Renvoie la liste des méthodes disponibles pour une variable en fonction de son type.

d) `info(string name)`: Renvoie l'aide concernant une méthode spécifique.

e) `json()`: Renvoie la représentation JSON de l'objet, si disponible.

## Objet transparent : `self` (ou `let`)

`self` est un objet transparent, similaire à une sorte de pointeur, qui ne nécessite aucune transformation spécifique du paramètre lorsqu'il est utilisé dans une fonction.

Remarque : Le mot-clé `let` se comporte comme `self`, avec une grande différence. La première variable stockée dans une variable `let` définit le type de cette variable. En d'autres termes, si vous stockez une chaîne de caractères dans une variable `let`, alors cette variable se comportera toujours comme une chaîne de caractères. Vous pouvez modifier ce comportement avec l'opérateur `:=`, qui dans ce cas force la variable `let` à un nouveau type.

### Exemple

```javascript
function compare(self x, self y) {
    if (x.type() == y.type())
        print("Il s'agit du même type d'objets");
}

// Par exemple, dans ce cas, la fonction compare reçoit deux paramètres, dont les types peuvent varier. Une déclaration self supprime la nécessité d'appliquer une conversion spécifique aux objets qui sont passés à cette fonction.

string s1, s2;
compare(s1, s2); // Nous comparons deux chaînes de caractères

myframe i1;
myframe i2;
compare(i1, i2); // Nous comparons deux frames
```

### Exemple avec `let`

```javascript
let l = "x=";
l += 12;
println(l); // Cela affiche : x=12

// Nous forçons 'l' à être un entier
l := 1;
l += 12;
println(l); // Cela affiche : 13
```