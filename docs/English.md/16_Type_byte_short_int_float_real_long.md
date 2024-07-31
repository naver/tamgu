## Tamgu Numerical Types

Tamgu provides different numerical types: byte, short, int, float, real, long, and complex. The types byte, short, int, float, and real are described in the next section. It's worth noting that real and float are aliases of each other. The addition of the real type was necessary because float was often misunderstood.

### C++ Implementation Note

In the C++ implementation of Tamgu, int and float (or real) are implemented as long and double, respectively. The long type is implemented as a 64-bit integer, specifically __int64 on Windows and "long long" on Unix platforms. The byte type is implemented as an unsigned char, and the short type is implemented as short.

## Numerical Methods

Tamgu provides the following numerical methods:

1. `#()`: Return the byte complement.
2. `abs()`: Return the absolute value.
3. `acos()`: Return the arc cosine.
4. `acosh()`: Return the area hyperbolic cosine.
5. `asin()`: Return the arc sine.
6. `asinh()`: Return the area hyperbolic sine.
7. `atan()`: Return the arc tangent.
8. `atanh()`: Return the area hyperbolic tangent.
9. `base(int b)`: Return a string representing a number in base b.
10. `base(vector chrs)`: Set the encoding for each digit in a given base. The default set is 64 characters: 0-9, A-Z, a-z, #, @. You can replace this default set of characters with your own. If you supply an empty vector, then the system resets to the default set of characters.
11. `bit(int ith)`: Return true if the ith bit is 1.
12. `cbrt()`: Return the cubic root.
13. `chr()`: Return the ASCII character corresponding to this number as a code.
14. `cos()`: Return the cosine.
15. `cosh()`: Return the hyperbolic cosine.
16. `emoji()`: Return the textual description (in English) of an emoji based on its Unicode code.
17. `erf()`: Return the error function.
18. `erfc()`: Return the complementary error function.
19. `even()`: Return true if the value is even.
20. `exp()`: Return the exponential function.
21. `exp2()`: Return the binary exponential function.
22. `expm1()`: Return the exponential minus one.
23. `factors()`: Return the prime factor decomposition as an ivector.
24. `floor()`: Return the down value.
25. `format(string form)`: Return a string formatted according to the pattern in form. (This format is the same as the sprintf format in C++).
26. `fraction()`: Return the value as a fraction.
27. `isemoji()`: Return true if the code matches an emoji.
28. `lgamma()`: Return the log-gamma function.
29. `log()`: Return the natural logarithm.
30. `log1p()`: Return the logarithm plus one.
31. `log2()`: Return the binary logarithm.
32. `logb()`: Return the floating-point base logarithm.
33. `nearbyint()`: Round to the nearest integral value.
34. `odd()`: Return true if the value is odd.
35. `prime()`: Return true if the value is a prime number.
36. `rint()`: Round to the nearest integral value.
37. `round()`: Round to the nearest.
38. `sin()`: Return the sine.
39. `sinh()`: Return the hyperbolic sine.
40. `sqrt()`: Return the square root.
41. `tan()`: Return the tangent.
42. `tanh()`: Return the hyperbolic tangent.
43. `tgamma()`: Return the gamma function.
44. `trunc()`: Return the value truncated to an integer.

### Specific to Floating Point Values

For floating-point values (float and decimal), the following methods are available:

45. `bits()`: Return the underlying bit representation of a floating-point value as an integer (int or long).
46. `bits(int v)`: Transform v into a floating-point value. v in this case is not an integer value, but the inner bit representation of a floating-point value, such as the one returned by bits().
47. `exponent()`: Return the exponent of a floating-point value, such as value = mantissa() * 2^exponent().
48. `mantissa()`: Return the mantissa of a floating-point value.

## Complete List of Mathematical Functions

Tamgu provides the following mathematical functions: abs, acos, acosh, asin, asinh, atan, atanh, cbrt, cos, cosh, erf, erfc, exp, exp2, expm1, floor, lgamma, ln, log, log1p, log2, logb, nearbyint, rint, round, sin, sinh, sqrt, tan, tanh, tgamma, trunc.

## Binary

A binary number always starts with "0b". Tamgu considers it a valid number as long as it is a valid binary string, containing only 0s and 1s. For example, 0b1110 is equal to 14.

## Hexadecimal

A hexadecimal number always starts with "0x". Tamgu considers it a valid number as long as it is a valid hexadecimal string. A hexadecimal declaration can mix upper or lower characters for the hexadecimal digits A, B, C, D, E, and F.

## Operators

Tamgu supports various mathematical and bitwise operators:

- `+`, `-`, `*`, `/`: Mathematical operators
- `<<`, `>>`, `&`, `|`, `^`: Bitwise operators
- `%`: Division modulo
- `%%`: Integer Division
- `^^`: Power (2^^2 = 4)
- `+=`, `-=`, etc.: Self operators

## Syntactic Sugar

Tamgu provides some syntactic sugar notations, making operations more readable. These include:

- `×`, `÷` can be used instead of `*` and `/`
- `π`, `τ`, `φ`, `ℯ`, whose values are 3.14159, 6.2831, 1.61803, and 2.71828
- `_pi`, `_tau`, `_phi`, `_e` are alternative names for the values above
- `²`, `³` for square and cubic
- `√(`, `∛(` for square root and cubic root
- Expressions like `2a+b` or `2(12+a)` are supported
- `a b` (with a space in between) is the same as `a*b` or `a×b`

## Example

```cpp
int h = 0xAB45; // Hexadecimal number
int i = 10;
float f = i.log(); // value = 1
f += 10; // value = 11
f = i % 5; // value = 0
f = 2 * i + 10; // 30
f = 2×i + 10; // 30
f = 2π + φ; // 7.90122
f = √(2i); // 4.47214
f = i²; // 100
f = 2(i-1); // 18

complex c(10, -2); // Complex number as a class object
println(c * 2); // 0c20: -4i
```

Complex numbers can be added or multiplied with each other. The complex number exposes two methods:

1. `real()`: Returns the real part of the complex number.
2. `imaginary(int v)`: Returns the imaginary part.

A complex number can be declared in different ways:

1. As a class object: `complex c(10, -2)` - The first part is the real value (a float), and the second part is the imaginary part (also a float).
2. Through a number definition: `0c10:2i` - The "0c" indicates a complex number, followed by the real part, then a ":", and finally the imaginary part ending with "i".