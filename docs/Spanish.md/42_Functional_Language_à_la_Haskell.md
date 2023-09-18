# Taskell

Taskell es un lenguaje funcional inspirado en Haskell. Proporciona capacidades similares a Haskell, pero con un enfoque específico en un rango seleccionado de funciones. Si bien Taskell no se comporta como un compilador completo de Haskell, incorpora algunos aspectos interesantes del lenguaje Haskell.

## Antes de empezar: Algunos operadores nuevos

Antes de sumergirnos en el lenguaje, vamos a introducir algunos operadores específicos que enriquecen el mundo de Taskell.

### Declaraciones de rango: [a..b]

Para cumplir con el lenguaje Haskell, Taskell ha agregado una nueva forma de declarar un rango de elementos usando el operador "..". Por ejemplo, `[1..10]` define el vector `[1,2,3,4,5,6,7,8,9,10]`.

### Paso

Por defecto, el paso es 1, pero es posible establecer un paso diferente. Puedes definir el paso directamente con ":" al final de la expresión. Por ejemplo, `[1..10:2]` define el vector `[1,3,5,7,9]`. También puedes definir el paso proporcionando el siguiente elemento en la definición. Por ejemplo, `[1,3..10]` define el vector `[1,3,5,7,9]`. También funciona con caracteres. Por ejemplo, `[‘a’,’c’..’g’]` define el vector `['a','c','e','g']`. El mismo vector también se puede definir con `[‘a’..’g’:2]`.

### Rangos infinitos

Taskell también proporciona una noción de rango infinito de elementos. Hay dos casos: puedes ignorar el primer elemento del conjunto o el último elemento. `[1..]` define un vector infinito que comienza en 1, hacia adelante: `[1,2,3,4…`. `[..1]` define un vector infinito que comienza en 1, hacia atrás: `[1,0,-1,-2,-3…`. También puedes usar diferentes pasos. `[1..:2]` define un vector infinito que comienza en 1, hacia adelante: `[1,3,5…`. `[..1:2]` define un vector infinito que comienza en 1, hacia atrás: `[1,-1,-3…`. `[1,3..]` define un vector infinito que comienza en 1, hacia adelante: `[1,3,5…`. `[..-1,1]` define un vector infinito que comienza en 1, hacia atrás: `[1,-1,-3…`.

### Tres nuevos operadores: &&&, ||| y ::

Estos tres operadores se utilizan para concatenar una lista de elementos o para agregar un elemento a un vector.

#### Combinar: &&&

Este operador se utiliza para combinar diferentes elementos en un vector. Si uno de los elementos no es una lista, simplemente se combina en la lista actual. Por ejemplo, `vector v= 7 &&& 8 &&& [1,2]; println (v);` daría como resultado `[7,8,1,2]`. Este operador es similar a "++" en Haskell. Dado que este operador ya estaba definido en Tamgu, se ha modificado a "&&&".

#### Combinar: ||| (c1 ||| c2)

Este operador combina los valores de un contenedor con otro contenedor, o de una cadena con una cadena. Cuando los contenedores son contenedores de valores, se utiliza el operador "+" para agregar o concatenar los valores de c1 con c2. Por ejemplo, `vector v= [1,2,3]; vector vv = [4,5,6]; println (v ||| vv); //[1,4],[2,5],[3,6]` sería el resultado. De manera similar, `ivector iv= [1,2,3]; ivector ivv = [4,5,6]; println (iv ||| ivv); //[5,7,9]` sería el resultado. La misma operación se puede hacer con cadenas, mapas o mapas con valores enteros.

#### Agregar: ::

Este operador fusiona el elemento en el vector actual. Por ejemplo, `1::v` daría como resultado `[1,7,8,1,2]`, y `v::12` daría como resultado `[1,7,8,1,2,12]`.

## Conceptos básicos

### Declarar una instrucción similar a Taskell

Todas las instrucciones de Taskell en Tamgu deben declararse entre "<..>", que el compilador interno de Tamgu utiliza para detectar una fórmula de Taskell. Por ejemplo, `vector v=<map (+1) [1..10]>;` suma 1 a cada elemento del vector.

### La estructura más simple

La estructura más simple para un programa de Taskell es devolver un valor como `<1>`.

También puedes devolver un cálculo como `<3+1>;`, en cuyo caso el sistema devolverá un único valor atómico.

### Utilización de >, <, |, << y >>

Estos operadores pueden causar problemas cuando se usan dentro de una fórmula de Taskell, ya que pueden confundir al compilador con la apertura o cierre de corchetes de Taskell. Para evitar este problema, debes insertar estas expresiones entre paréntesis.

Por ejemplo, `<x | x < - [-5..5], (x > 0)>` produce `[1,2,3,4,5]`, y `< (x << 1) | x <- [0..5]>` produce `[0,2,4,6,8,10]`.

### Iteración

El lenguaje Taskell proporciona una forma conveniente y eficiente de representar listas. En Tamgu, estas listas se implementan como "vectores", que se pueden intercambiar entre diferentes estructuras.

La instrucción de Taskell más básica tiene la siguiente forma: `<x | x < - v, Boolean>`. Devuelve una lista como resultado, con los siguientes pasos:

1. Agregamos x a nuestra lista de resultados actual.
2. Obtenemos x iterando a través de v, denotado por `x <- v`.
3. Ponemos una restricción booleana, que se puede omitir.

La razón por la que devuelve una lista se debe a la iteración en la expresión. Por ejemplo, `<x | x < - [-5..5], x!=0>` produce `[-5,-4,-3,-2,-1,1,2,3,4,5]`.