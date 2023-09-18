# Tipo de Fracción

El tipo `Fraction` en Tamgu permite a los usuarios manejar números como fracciones, las cuales pueden ser utilizadas en cualquier cálculo. Todos los métodos matemáticos para enteros y flotantes son válidos para las fracciones, pero este tipo ofrece algunos métodos adicionales específicos.

## Métodos

1. `d()`: Devuelve el denominador de la fracción.
2. `d(int v)`: Establece el denominador de la fracción.
3. `fraction f(int n, int d)`: Crea una fracción con un numerador y un denominador. Por defecto, el numerador es 0 y el denominador es 1.
4. `invert()`: Intercambia el denominador con el numerador de una fracción.
5. `n()`: Devuelve el numerador de la fracción.
6. `n(int v)`: Establece el numerador de la fracción.
7. `nd(int n, int d)`: Establece el numerador y el denominador de una fracción.

## Conversión

Cuando una fracción se utiliza como una cadena de texto, entero o flotante, Tamgu automáticamente crea el flotante o entero apropiado a través de un cálculo simple de la fracción. Sin embargo, esta traducción puede resultar en una pérdida de información. Además, Tamgu simplifica la fracción en cada paso para mantenerla lo más pequeña posible.

Cuando se utiliza como una cadena de texto, Tamgu devuelve la fracción en el formato "NUM/DEN".

## Ejemplos

```cpp
// Creamos dos fracciones
fraction f(10, 3);
fraction g(18, 10);

// Sumamos g a f...
f += g;

println(f); // Muestra: 77/15
```

Nota: El ejemplo de código asume el uso del lenguaje de programación Tamgu.