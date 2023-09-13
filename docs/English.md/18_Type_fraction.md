# Fraction Type

The `Fraction` type in Tamgu enables users to handle numbers as fractions, which can be used anywhere in any calculations. All the mathematical methods for integers and floats are still valid for fractions, but this type offers a few additional specific methods.

## Methods

1. `d()`: Returns the denominator of the fraction.
2. `d(int v)`: Sets the denominator of the fraction.
3. `fraction f(int n, int d)`: Creates a fraction with a numerator and a denominator. By default, the numerator is 0 and the denominator is 1.
4. `invert()`: Switches the denominator with the numerator of a fraction.
5. `n()`: Returns the numerator of the fraction.
6. `n(int v)`: Sets the numerator of the fraction.
7. `nd(int n, int d)`: Sets the numerator and denominator of a fraction.

## Conversion

When a fraction is used as a string, integer, or float, Tamgu automatically creates the appropriate float or integer through a simple computation of the fraction. However, this translation may result in a loss of information. Additionally, Tamgu simplifies the fraction at each step in order to keep it as small as possible.

When used as a string, Tamgu returns the fraction in the format "NUM/DEN".

## Examples

```cpp
// We create two fractions
fraction f(10, 3);
fraction g(18, 10);

// We add g to f...
f += g;

println(f); // Display: 77/15
```

Note: The code example assumes the usage of the Tamgu programming language.