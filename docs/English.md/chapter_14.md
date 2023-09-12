# Tamgu Numerical Types

Tamgu provides different numerical types: `byte`, `short`, `int`, `float`, `real`, and `fraction`. The `real` and `float` types are aliases of each other. The `real` type was added to avoid confusion with the meaning of `float`.

## C++ Implementation Notes

In the C++ implementation, `int` and `float` (or `real`) are implemented as `long` and `double`, respectively. `long` is a 64-bit integer, represented as `__int64` on Windows and `long long` on Unix platforms. `byte` is implemented as an unsigned char and `short` is implemented as a short.

## Methods

1. `#()`: Return the byte complement.
2. `abs()`: Return the absolute value.
3. `acos()`: Return the arc cosine.
4. `acosh()`: Return the area hyperbolic cosine.
5. `asin()`: Return the arc sine.
6. `asinh()`: Return the area hyperbolic sine.
7. `atan()`: Return the arc tangent.
8. `atanh()`: Return the area hyperbolic tangent.
9. `base(int b)`: Return a string representing a number in base `b`.
10. `base(vector chrs)`: Set the encoding for each digit in a given base. The default set is 64 characters: 0-9, A-Z, a-z, #, @. You can replace this default set with your own. If you supply an empty vector, the system resets to the default set.
11. `bit(int ith)`: Return true if the ith bit is 1.
12. `cbrt()`: Return the cubic root.
...
36. `rint()`: Round to the nearest integral value.
37. `round()`: Round to the nearest value.
38. `sin()`: Return the sine.
39. `sinh()`: Return the hyperbolic sine.
40. `sqrt()`: Return the square root.
41. `tan()`: Return the tangent.
42. `tanh()`: Return the hyperbolic tangent.
43. `tgamma()`: Return the gamma function.
44. `trunc()`: Return the integral value.

Specific to floating-point values (float and decimal):
45. `bits()`: Return the underlying bit representation of a floating-point value as an integer.
46. `bits(int v)`: Transform `v` into a floating-point value. `v` in this case is not an integer value, but the inner bit representation of a floating-point value.
47. `exponent()`: Return the exponent of a floating-point value.
48. `mantissa()`: Return the mantissa of a floating-point value.

## Complete List of Mathematical Functions

Tamgu provides the following mathematical functions: `abs`, `acos`, `acosh`, `asin`, `asinh`, `atan`, `atanh`, `cbrt`, `cos`, `cosh`, `erf`, `erfc`, `exp`, `exp2`, `expm1`, `floor`, `lgamma`, `ln`, `log`, `log1p`, `log2`, `logb`, `nearbyint`, `rint`, `round`, `sin`, `sinh`, `sqrt`, `tan`, `tanh`, `tgamma`, `trunc`.

## Binary

A binary number always starts with "0b". It is considered valid as long as it contains only 0s and 1s. For example, `0b1110` is 14.

## Hexadecimal

A hexadecimal number always starts with "0x". It is considered valid as long as it is a valid hexadecimal string. A hexadecimal declaration can mix upper or lower characters for the hexadecimal digits A, B, C, D, E, F.

## Operators

- `+`, `-`, `*`, `/`: Mathematical operators.
- `<<`, `>>`, `&`, `|`, `^`: Bitwise operators.
- `%`: Division modulo.
- `^^`: Power (2^^2 = 4).
- `+=`, `-=`, etc.: Self-operators.

## Syntactic Sugar

Tamgu provides some syntactic sugar notations to make operations more readable:
- `×`, `÷` can be used instead of `*` and `/`.
- `π`, `τ`, `φ`, `ℯ` have the values 3.14159, 6.2831, 1.61803, and 2.71828, respectively.
- `_pi`, `_tau`, `_phi`, `_e` are alternative names for the values above.
- `²`, `³` represent square and cubic.
- `√(`, `∛(` represent square root and cubic root.
- Expressions like `2a+b` or `2(12+a)` are allowed.
- `a b` (with a space in between) is the same as `a*b` or `a×b`.

## Example

```cpp
int h = 0xAB45;  // Hexadecimal number
int i = 10;
float f = i.log();  // value = 1
f += 10;  // value = 11
f = i % 5;  // value = 0
f = 2 * i + 10;  // 30
f = 2×i + 10;  // 30
f = 2π + φ;  // 7.90122
f = √(2 * i);  // 4.47214
```
