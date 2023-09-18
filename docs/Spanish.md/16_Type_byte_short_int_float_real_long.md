## Tipos Numéricos de Tamgu

Tamgu proporciona diferentes tipos numéricos: byte, short, int, float, real, long y complex. Los tipos byte, short, int, float y real se describen en la siguiente sección. Vale la pena destacar que real y float son sinónimos. La adición del tipo real fue necesaria porque float a menudo se entendía mal.

### Nota de Implementación en C++

En la implementación de C++ de Tamgu, int y float (o real) se implementan como long y double, respectivamente. El tipo long se implementa como un entero de 64 bits, específicamente __int64 en Windows y "long long" en plataformas Unix. El tipo byte se implementa como un char sin signo, y el tipo short se implementa como short.

## Métodos Numéricos

Tamgu proporciona los siguientes métodos numéricos:

1. `#()`: Devuelve el complemento byte.
2. `abs()`: Devuelve el valor absoluto.
3. `acos()`: Devuelve el arcocoseno.
4. `acosh()`: Devuelve el arcocoseno hiperbólico.
5. `asin()`: Devuelve el arcoseno.
6. `asinh()`: Devuelve el arcoseno hiperbólico.
7. `atan()`: Devuelve la arcotangente.
8. `atanh()`: Devuelve la arcotangente hiperbólica.
9. `base(int b)`: Devuelve una cadena que representa un número en la base b.
10. `base(vector chrs)`: Establece la codificación para cada dígito en una base dada. El conjunto predeterminado es de 64 caracteres: 0-9, A-Z, a-z, #, @. Puede reemplazar este conjunto predeterminado de caracteres con el suyo propio. Si proporciona un vector vacío, el sistema se restablece al conjunto predeterminado de caracteres.
11. `bit(int ith)`: Devuelve verdadero si el bit i-ésimo es 1.
12. `cbrt()`: Devuelve la raíz cúbica.
13. `chr()`: Devuelve el carácter ASCII correspondiente a este número como un código.
14. `cos()`: Devuelve el coseno.
15. `cosh()`: Devuelve el coseno hiperbólico.
16. `emoji()`: Devuelve la descripción textual (en inglés) de un emoji según su código Unicode.
17. `erf()`: Devuelve la función error.
18. `erfc()`: Devuelve la función error complementaria.
19. `even()`: Devuelve verdadero si el valor es par.
20. `exp()`: Devuelve la función exponencial.
21. `exp2()`: Devuelve la función exponencial binaria.
22. `expm1()`: Devuelve la exponencial menos uno.
23. `factors()`: Devuelve la descomposición en factores primos como un ivector.
24. `floor()`: Devuelve el valor inferior.
25. `format(string form)`: Devuelve una cadena formateada según el patrón en form. (Este formato es el mismo que el formato sprintf en C++).
26. `fraction()`: Devuelve el valor como una fracción.
27. `isemoji()`: Devuelve verdadero si el código coincide con un emoji.
28. `lgamma()`: Devuelve la función log-gamma.
29. `log()`: Devuelve el logaritmo natural.
30. `log1p()`: Devuelve el logaritmo más uno.
31. `log2()`: Devuelve el logaritmo binario.
32. `logb()`: Devuelve el logaritmo en base flotante.
33. `nearbyint()`: Redondea al valor entero más cercano.
34. `odd()`: Devuelve verdadero si el valor es impar.
35. `prime()`: Devuelve verdadero si el valor es un número primo.
36. `rint()`: Redondea al valor entero más cercano.
37. `round()`: Redondea al más cercano.
38. `sin()`: Devuelve el seno.
39. `sinh()`: Devuelve el seno hiperbólico.
40. `sqrt()`: Devuelve la raíz cuadrada.
41. `tan()`: Devuelve la tangente.
42. `tanh()`: Devuelve la tangente hiperbólica.
43. `tgamma()`: Devuelve la función gamma.
44. `trunc()`: Devuelve el valor truncado a un entero.

### Específicos para Valores de Punto Flotante

Para valores de punto flotante (float y decimal), están disponibles los siguientes métodos:

45. `bits()`: Devuelve la representación de bits subyacente de un valor de punto flotante como un entero (int o long).
46. `bits(int v)`: Transforma v en un valor de punto flotante. En este caso, v no es un valor entero, sino la representación de bits interna de un valor de punto flotante, como la que devuelve bits().
47. `exponent()`: Devuelve el exponente de un valor de punto flotante, como valor = mantissa() * 2^exponent().
48. `mantissa()`: Devuelve la mantisa de un valor de punto flotante.

## Lista Completa de Funciones Matemáticas

Tamgu proporciona las siguientes funciones matemáticas: abs, acos, acosh, asin, asinh, atan, atanh, cbrt, cos, cosh, erf, erfc, exp, exp2, expm1, floor, lgamma, ln, log, log1p, log2, logb, nearbyint, rint, round, sin, sinh, sqrt, tan, tanh, tgamma, trunc.

## Binario

Un número binario siempre comienza con "0b". Tamgu lo considera un número válido siempre que sea una cadena binaria válida, que contenga solo 0s y 1s. Por ejemplo, 0b1110 es igual a 14.

## Hexadecimal

Un número hexadecimal siempre comienza con "0x". Tamgu lo considera un número válido siempre que sea una cadena hexadecimal válida. Una declaración hexadecimal puede mezclar caracteres en mayúsculas o minúsculas para los dígitos hexadecimales A, B, C, D, E y F.

## Operadores

Tamgu admite varios operadores matemáticos y bit a bit:

- `+`, `-`, `*`, `/`: Operadores matemáticos
- `<<`, `>>`, `&`, `|`, `^`: Operadores bit a bit
- `%`: Módulo de la división
- `^^`: Potencia (2^^2 = 4)
- `+=`, `-=`, etc.: Operadores de asignación

## Azúcar Sintáctico

Tamgu proporciona algunas notaciones de azúcar sintáctico, lo que hace que las operaciones sean más legibles. Estos incluyen:

- `×`, `÷` se pueden usar en lugar de `*` y `/`
- `π`, `τ`, `φ`, `ℯ`, cuyos valores son 3.14159, 6.2831, 1.61803 y 2.71828
- `_pi`, `_tau`, `_phi`, `_e` son nombres alternativos para los valores anteriores
- `²`, `³` para cuadrado y cúbico
- `√(`, `∛(` para raíz cuadrada y raíz cúbica
- Se admiten expresiones como `2a+b` o `2(12+a)`
- `a b` (con un espacio en medio) es lo mismo que `a*b` o `a×b`

## Ejemplo

```cpp
int h = 0xAB45; // Número hexadecimal
int i = 10;
float f = i.log(); // valor = 1
f += 10; // valor = 11
f = i % 5; // valor = 0
f = 2 * i + 10; // 30
f = 2×i + 10; // 30
f = 2π + φ; // 7.90122
f = √(2i); // 4.47214
f = i²; // 100
f = 2(i-1); // 18

complex c(10, -2); // Número complejo como objeto de clase
println(c * 2); // 0c20: -4i
```

Los números complejos se pueden sumar o multiplicar entre sí. El número complejo expone dos métodos:

1. `real()`: Devuelve la parte real del número complejo.
2. `imaginary(int v)`: Devuelve la parte imaginaria.

Un número complejo se puede declarar de diferentes formas:

1. Como un objeto de clase: `complex c(10, -2)` - La primera parte es el valor real (un float), y la segunda parte es la parte imaginaria (también un float).
2. A través de una definición de número: `0c10:2i` - El "0c" indica un número complejo, seguido de la parte real, luego ":", y finalmente la parte imaginaria que termina con "i".