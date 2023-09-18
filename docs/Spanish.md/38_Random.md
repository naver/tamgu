# Aleatorio

## Número aleatorio: random(), a_random()

Tamgu proporciona una función para devolver un valor aleatorio entre 0 y 99. La función `random()` devuelve un valor largo. También puedes proporcionar un valor límite máximo como argumento. `a_random()` es una versión sin bloqueo de `random()`.

Ejemplo:
```cpp
float rd = random(); // valor entre 0 y 99
rd = random(999); // valor entre 0 y 999
```

## Distribuciones

### Elección aleatoria

La función `random_choice()` en Tamgu devuelve una lista de valores basados en los valores de un vector. La aleatorización se basa en una distribución discreta, donde cada valor tiene la misma probabilidad de ocurrir.

Ejemplo:
```cpp
random_choice(10, ["a", "b", "c"])
// devuelve, por ejemplo: ['c', 'a', 'b', 'a', 'b', 'c', 'b', 'c', 'a', 'a']
```

### Distribuciones uniformes

#### Entero uniforme

La función `uniform_int()` produce valores enteros aleatorios `i`, distribuidos uniformemente en el intervalo cerrado `[a, b]`, es decir, distribuidos según la función de probabilidad discreta.

Parámetros: `a`, `b`
- `a`: Límite inferior del rango de valores posibles que la distribución puede generar.
- `b`: Límite superior del rango de valores posibles que la distribución puede generar.

Ten en cuenta que `b` debe ser mayor o igual que `a` (`a < b`).

Devuelve un `int` o un `ivector` de tamaño `nb`.

#### Real uniforme

La función `uniform_real()` construye un objeto `uniform_real_distribution`, adoptando los parámetros de distribución especificados por `a` y `b`.

Parámetros: `a`, `b`
- `a`: Límite inferior del rango de valores posibles que la distribución puede generar.
- `b`: Límite superior del rango de valores posibles que la distribución puede generar.

Ten en cuenta que el rango incluye `a` pero no `b`. Además, `b` debe ser mayor o igual que `a` (`a <= b`).

Devuelve un `float` o un `fvector` de tamaño `nb`.

### Distribuciones de Bernoulli

#### Distribución de Bernoulli

La función `bernoulli_distribution()` construye un objeto `bernoulli_distribution`, adoptando los parámetros de distribución especificados por `t` y `p`.

Parámetros: `t`, `p`
- `t`: El límite superior del rango `[0, t]` de valores posibles que la distribución puede generar. Esto representa el número de experimentos independientes distribuidos de forma bernoulli que cada valor generado se dice que simula.
- `p`: Probabilidad de éxito. Esto representa la probabilidad de éxito en cada uno de los experimentos independientes distribuidos de forma bernoulli que cada valor generado se dice que simula. Esto debe ser un valor entre 0.0 y 1.0 (ambos incluidos).

Devuelve un `ivector` de valores booleanos (0 o 1) de tamaño `nb`.

#### Distribución binomial

La función `binomial_distribution()` construye un objeto `binomial_distribution`, adoptando los parámetros de distribución especificados por `t` y `p`.

Parámetros: `t`, `p`
- `t`: El límite superior del rango `[0, t]` de valores posibles que la distribución puede generar. Esto representa el número de experimentos independientes distribuidos de forma bernoulli que cada valor generado se dice que simula.
- `p`: Probabilidad de éxito. Esto representa la probabilidad de éxito en cada uno de los experimentos independientes distribuidos de forma bernoulli que cada valor generado se dice que simula. Esto debe ser un valor entre 0.0 y 1.0 (ambos incluidos).

Devuelve un `int` o un `ivector` de tamaño `nb`.

#### Distribución binomial negativa

La función `negative_binomial_distribution()` construye un objeto `negative_binomial_distribution`, adoptando los parámetros de distribución especificados por `k` y `p`.

Parámetros: `k`, `p`
- `k`: Parámetro `k` de la distribución binomial negativa. Esto representa el número de ensayos fallidos que detiene el recuento de experimentos exitosos distribuidos de forma bernoulli que cada valor generado se dice que simula.
- `p`: Probabilidad de éxito. Esto representa la probabilidad de éxito en cada uno de los experimentos independientes distribuidos de forma bernoulli que cada valor generado se dice que simula. Esto debe ser un valor entre 0.0 y 1.0 (ambos incluidos).

Devuelve un `int` o un `ivector` de tamaño `nb`.

#### Distribución geométrica

La función `geometric_distribution()` construye un objeto `geometric_distribution`, adoptando el parámetro de distribución especificado por `p`.

Parámetro: `p`
- `p`: Probabilidad de éxito. Esto representa la probabilidad de éxito en cada uno de los experimentos independientes distribuidos de forma bernoulli que cada valor generado se dice que simula. Esto debe ser un valor entre 0.0 y 1.0 (ambos incluidos).

Devuelve un `int` o un `ivector` de tamaño `nb`.

### Distribuciones de Poisson

#### Distribución de Poisson

La función `poisson_distribution()` construye un objeto `poisson_distribution`, adoptando el parámetro de distribución especificado por `mean`.

Parámetro: `mean`
- `mean`: Número esperado de eventos en el intervalo (μ). Esto representa la tasa a la que se observan los eventos que se cuentan, en promedio. Su valor debe ser positivo (μ > 0).

Devuelve un `int` o un `ivector` de tamaño `nb`.

#### Distribución exponencial

La función `exponential_distribution()` construye un objeto `exponential_distribution`, adoptando el parámetro de distribución especificado por `lambda`.

Parámetro: `lambda`
- `lambda`: Tasa promedio de ocurrencia (λ). Esto representa el número de veces que se observan los eventos aleatorios por intervalo, en promedio. Su valor debe ser positivo (λ > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución gamma

La función `gamma_distribution()` construye un objeto `gamma_distribution`, adoptando los parámetros de distribución especificados por `alpha` y `beta`.

Parámetros: `alpha`, `beta`
- `alpha`: Parámetro `alpha` (α), que define la forma de la distribución. Esto debe ser un valor positivo (α > 0).
- `beta`: Parámetro `beta` (β), que define la escala de la distribución. Esto debe ser un valor positivo (β > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución de Weibull

La función `weibull_distribution()` construye un objeto `weibull_distribution`, adoptando los parámetros de distribución especificados por `a` y `b`.

Parámetros: `a`, `b`
- `a`: Parámetro de distribución `a`, que define la forma de la distribución. Esto debe ser un valor positivo (a > 0).
- `b`: Parámetro de distribución `b`, que define la escala de la distribución. Esto debe ser un valor positivo (b > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución de valor extremo

La función `extreme_value_distribution()` construye un objeto `extreme_value_distribution`, adoptando los parámetros de distribución especificados por `a` y `b`.

Parámetros: `a`, `b`
- `a`: Parámetro de distribución `a`, que define la ubicación (desplazamiento) de la distribución.
- `b`: Parámetro de distribución `b`, que define la escala de la distribución. Esto debe ser un valor positivo (b > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

### Distribuciones normales

#### Distribución normal

La función `normal_distribution()` construye un objeto `normal_distribution`, adoptando los parámetros de distribución especificados por `mean` y `stddev`.

Parámetros: `mean`, `stddev`
- `mean`: Media de la distribución (su valor esperado, μ), que coincide con la ubicación de su pico.
- `stddev`: Desviación estándar (σ), la raíz cuadrada de la varianza, que representa la dispersión de los valores con respecto a la media de la distribución. Esto debe ser un valor positivo (σ > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución lognormal

La función `lognormal_distribution()` construye un objeto `lognormal_distribution`, adoptando los parámetros de distribución especificados por `m` y `s`.

Parámetros: `m`, `s`
- `m`: Media de la distribución normal subyacente formada por las transformaciones logarítmicas de los posibles valores en esta distribución.
- `s`: Desviación estándar de la distribución normal subyacente formada por las transformaciones logarítmicas de los posibles valores en esta distribución. Esto debe ser un valor positivo (s > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución chi-cuadrado

La función `chi_squared_distribution()` construye un objeto `chi_squared_distribution`, adoptando el parámetro de distribución especificado por `n`.

Parámetro: `n`
- `n`: Número de grados de libertad, que especifica el número de variables independientes simuladas por la distribución. Esto debe ser un valor positivo (n > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución de Cauchy

La función `cauchy_distribution()` construye un objeto `cauchy_distribution`, adoptando los parámetros de distribución especificados por `a` y `b`.

Parámetros: `a`, `b`
- `a`: Parámetro de distribución `a`, que especifica la ubicación del pico (su modo).
- `b`: Parámetro de distribución `b`, que define la escala de la distribución. Esto debe ser un valor positivo (b > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución F de Fisher

La función `fisher_distribution()` construye un objeto `fisher_f_distribution`, adoptando los parámetros de distribución especificados por `m` y `n`.

Parámetros: `m`, `n`
- `m`: Parámetro de distribución `m`, que especifica los grados de libertad del numerador. Esto debe ser un valor positivo (m > 0).
- `n`: Parámetro de distribución `n`, que especifica los grados de libertad del denominador. Esto debe ser un valor positivo (n > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

#### Distribución t de Student

La función `student_distribution()` construye un objeto `student_t_distribution`, adoptando el parámetro de distribución especificado por `n`.

Parámetro: `n`
- `n`: Grados de libertad. Su valor debe ser positivo (n > 0).

Devuelve un `float` o un `fvector` de tamaño `nb`.

### Distribuciones de muestreo

#### Distribución discreta

La función `discrete_distribution()` utiliza la secuencia en la lista `il` como los pesos para cada valor entero de 0 a `(n - 1)`, donde `n` es el tamaño de la lista inicializadora.

Devuelve un `int` o un `ivector` de tamaño `nb`.

Ejemplo:
```cpp
ivector iv = discrete_distribution(10, [40, 10, 10, 40]);
```

#### Distribución constante por partes

La función `piecewise_constant_distribution()` utiliza los valores en el rango definido dentro de `firstB` como los límites para los subintervalos (`bi`), y la secuencia que comienza en `firstW` como los pesos (`wi`) para cada subintervalo.

Devuelve un `float` o un `fvector` de tamaño `nb`.

Ejemplo:
```cpp
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res = piecewise_constant_distribution(100, intervals, weights);
```

#### Distribución lineal por partes

La función `piecewise_linear_distribution()` utiliza los valores en el rango dentro de `firstB` como los límites para los subintervalos (`bi`), y la secuencia que comienza en `firstW` como los pesos (`wi`) para cada límite de subintervalo.

Devuelve un `float` o un `fvector` de tamaño `nb`.

Ejemplo:
```cpp
fvector intervals = [0.0, 4.5, 9.0];
fvector weights = [10.0, 0.0, 10.0];
fvector res = piecewise_linear_distribution(100, intervals, weights);
```