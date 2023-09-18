# Type Fraction

Le type `Fraction` dans Tamgu permet aux utilisateurs de manipuler des nombres sous forme de fractions, qui peuvent être utilisées n'importe où dans n'importe quel calcul. Toutes les méthodes mathématiques pour les entiers et les flottants restent valables pour les fractions, mais ce type offre quelques méthodes spécifiques supplémentaires.

## Méthodes

1. `d()`: Renvoie le dénominateur de la fraction.
2. `d(int v)`: Définit le dénominateur de la fraction.
3. `fraction f(int n, int d)`: Crée une fraction avec un numérateur et un dénominateur. Par défaut, le numérateur est 0 et le dénominateur est 1.
4. `invert()`: Inverse le dénominateur avec le numérateur d'une fraction.
5. `n()`: Renvoie le numérateur de la fraction.
6. `n(int v)`: Définit le numérateur de la fraction.
7. `nd(int n, int d)`: Définit le numérateur et le dénominateur d'une fraction.

## Conversion

Lorsqu'une fraction est utilisée comme une chaîne de caractères, un entier ou un flottant, Tamgu crée automatiquement le flottant ou l'entier approprié grâce à un calcul simple de la fraction. Cependant, cette traduction peut entraîner une perte d'information. De plus, Tamgu simplifie la fraction à chaque étape afin de la garder aussi petite que possible.

Lorsqu'elle est utilisée comme une chaîne de caractères, Tamgu renvoie la fraction au format "NUM/DEN".

## Exemples

```cpp
// Nous créons deux fractions
fraction f(10, 3);
fraction g(18, 10);

// Nous ajoutons g à f...
f += g;

println(f); // Affiche : 77/15
```

Note : L'exemple de code suppose l'utilisation du langage de programmation Tamgu.