# Tamgu Fraction Type

The Tamgu library enables users to handle numbers as fractions, which can be used anywhere in any calculations. All the mathematical methods for integers and floats can still be applied to fractions. However, this type offers a few additional specific methods.

## Methods

1. `d()`: Returns the denominator of the fraction.
2. `d(int v)`: Sets the denominator of the fraction.
3. `fraction f(int n,int d)`: Creates a fraction with a numerator and a denominator. By default, the numerator is 0 and the denominator is 1.
4. `invert()`: Switches the denominator with the numerator of a fraction.
5. `n()`: Returns the numerator of the fraction.
6. `n(int v)`: Sets the numerator of the fraction.
7. `nd(int n,int d)`: Sets the numerator and denominator of a fraction.

Fractions can also be represented as strings, integers, or floats. Tamgu automatically converts the fraction into an appropriate float or integer through a simple computation. However, this translation may result in a loss of information. Additionally, at each step, Tamgu simplifies the fraction to keep it as small as possible.

When represented as a string, Tamgu returns the fraction in the format "NUM/DEN".

## Examples

```cpp
// Create two fractions
fraction f(10,3);
fraction g(18,10);

// Add g to f
f += g;

println(f); // Display: 77/15
```

Note: The above code snippet is written in C++.