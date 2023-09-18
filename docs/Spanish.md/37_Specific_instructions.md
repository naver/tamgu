# Instrucciones Específicas de Tamgu

Tamgu proporciona todas las operaciones necesarias para manejar varios algoritmos, incluyendo las declaraciones if, else, elif, switch, for y while. También soporta operadores booleanos como and (&&), or (||) y xor. Estos operadores booleanos combinan diferentes elementos con un valor booleano. Los operadores and y or también se pueden escribir como && y ||, respectivamente.

- El operador `and` tiene éxito si todos los elementos son verdaderos.
- El operador `or` tiene éxito si al menos un elemento es verdadero.
- El operador `xor` tiene éxito si al menos un elemento es verdadero pero falla si todos son verdaderos.

## Declaraciones if-elif-else

Las declaraciones if-elif-else en Tamgu se utilizan para realizar ramificaciones condicionales. La sintaxis es la siguiente:

```python
if (expresión_booleana) {
    // bloque de código
}
elif (expresión_booleana) {
    // bloque de código
}
...
else {
    // bloque de código
}
```

## Declaración ifnot

La declaración `ifnot` en Tamgu se utiliza para encadenar diferentes instrucciones y devuelve el valor de la primera instrucción que devuelve cualquier cosa que no sea falso, nulo o vacío. Se utiliza comúnmente para manejar secuencias complejas de operaciones.

Ejemplo:

```python
map m1 = { "a": 1, "b": 2, "c": 3 };
map m2 = { "A": 4, "B": 5, "C": 6 };
map m3 = { "aa": 7, "bb": 8, "cc": 9 };
int i = (m1["A"] ifnot m2["B"] ifnot m3["bb"]) + 24;
println(i); // Salida: 29

function calling(string s) {
    return (m1[s] ifnot m2[s] ifnot m3[s]);
}

println(calling('bb')); // Salida: 8
```

## Declaración switch

La declaración `switch` en Tamgu se utiliza para enumerar una serie de pruebas para un solo objeto. La sintaxis es la siguiente:

```python
switch (expresión) (con función) {
    v1: {
        // bloque de código
    }
    v2: {
        // bloque de código
    }
    default: {
        // bloque de código
    }
}
```

Los `v1`, `v2`, etc. pueden ser una cadena, un entero o un flotante. La expresión se evalúa una vez y se compara con `v1`, `v2`, etc. También es posible reemplazar la comparación simple entre los elementos con una llamada a una función, que debe devolver `true` o `false`.

Ejemplo:

```python
// Probamos si un valor es mayor que el otro
function tst(int i, int j) {
    if (j >= i)
        return true;
    return false;
}

int s = 10;

// Probamos a través de tst
switch (s) con tst {
    1: println("1");
    2: println("2");
    20: println("20"); // Esta será la ocurrencia seleccionada
}
```

## Declaraciones for

Hay diferentes tipos de bucles `for` en Tamgu. Aquí se presenta cada uno de ellos.

### `for (expresión; booleano; siguiente)`

Este bucle `for` está compuesto por tres partes: una inicialización, una expresión booleana y una parte de continuación. La sintaxis es la siguiente:

```python
for (expresión; booleano; siguiente) {
    // bloque de código
}
```

Puedes usar `continue` o `break` para pasar al siguiente elemento o romper en medio de un bucle.

Ejemplo:

```python
for (i = 0; i < 10; i += 1)
    print("I=", i, "\n");
```

### `for (var en contenedor)`

Este tipo de bucle `for` se utiliza para iterar sobre un contenedor, como una cadena o un archivo. La sintaxis es la siguiente:

```python
for (var en contenedor) {
    // bloque de código
}
```

Ejemplo:

```python
// Recorriendo un archivo
file f('myfile.txt', "r");
string s;
for (s en f)
    println(s);

// Recorriendo un vector de enteros
vector v = [1, 2, 3, 4, 5, 6];
int i;
for (i en v)
    println(l);
```

### `for (i en <inicio, fin, incremento>)`

Este tipo de bucle `for` es un bucle rápido que es aproximadamente un 30% a un 50% más rápido que su equivalente. Cada valor en el rango se puede instanciar a través de variables. Sin embargo, una vez que el bucle ha comenzado, no se puede modificar ningún elemento, incluida la variable que recibe los diferentes valores. La sintaxis es la siguiente:

```python
for (i en <inicio, fin, incremento>) {
    // bloque de código
}
```

Ejemplo:

```python
int i, j = 1;
int v;
time t1;

// Recorriendo desde 0 hasta 100000, con un incremento de 1.
for (i en <0, 100000, j>)
    v = i;

time t2;
float diff = t2 - t1;
println("Tiempo transcurrido para el bucle rápido 'for':", diff);

time t3;
for (i = 0; i < 100000; i += j)
    v = i;

time t4;
diff = t4 - t3;
println("Tiempo transcurrido para el bucle regular 'for'", diff);
```

## Declaración while

La declaración `while` en Tamgu está compuesta por una única expresión booleana. La sintaxis es la siguiente:

```python
while (booleano) {
    // bloque de código
}
```

Puedes usar `continue` o `break` para pasar al siguiente elemento o romper en medio de un bucle.

Ejemplo:

```python
int i = 10;
while (i > 0) {
    print("I=", i, "\n");
    i -= 1;
}
```

## Declaración do-while

La declaración `do-while` en Tamgu es similar a `while`, pero la primera iteración se realiza antes de la prueba booleana. La sintaxis es la siguiente:

```python
do {
    // bloque de código
}
while (booleano);
```

Ejemplo:

```python
int i = 10;
do {
    print("I=", i, "\n");
    i -= 1;
}
while (i > 0);
```

## Identificador Único Universal: `uuid()`

La función `uuid()` en Tamgu devuelve un UUID (Identificador Único Universal) de acuerdo con el RFC 4122 de UUID, versión 4 (totalmente aleatorio). Devuelve un valor de cadena que contiene valores hexadecimales.

Ejemplo:

```python
string s = uuid(); // Ejemplo de salida: 80c67c4d-4c4d-14cd-e58d-eb3364cd
```

## Evaluación: `eval(cadena de código)`

La función `eval()` en Tamgu puede evaluar y ejecutar código Tamgu sobre la marcha. El resultado de la evaluación se devuelve según lo que se evaluó.

## Evaluación JSON: `evaljson(cadena de código)`

La función `evaljson()` en Tamgu toma una cadena JSON como entrada y la compila en un vector o un mapa según la estructura interna.

## Instrucciones de Impresión

Tamgu proporciona varias instrucciones de impresión para mostrar resultados en el puerto de visualización actual. Las instrucciones `print` y `println` se utilizan para mostrar resultados, mientras que las instrucciones `printerr` y `printlnerr` muestran los resultados en la salida de error estándar. Las versiones `println` y `printlnerr` agregan dos características a la salida: se agrega un espacio adicional para los valores separados con una coma, y se agrega un retorno de carro al final de la línea.

## Instrucciones de Impresión Conjunta

Las instrucciones `printj`, `printjln`, `printjerr` y `printjlnerr` se utilizan para mostrar valores de contenedores que se "unen" previamente. Aceptan dos, tres o cuatro argumentos. El primer parámetro debe ser un contenedor y el segundo debe ser una cadena separadora. Si el contenedor es un mapa, también se puede proporcionar un tercer parámetro como separador de claves. Se puede proporcionar un cuarto parámetro numérico para agregar un retorno de carro cada n valores.

Ejemplo:

```python
ivector v = [1..10];
printj(v, "-"); // Salida: 1-2-3-4-5-6-7-8-9-10

map m = {1: 2, 2: 3, 4: 5, 6: 7};
printjln(m, ",", "-"); // Salida: 1-2,2-3,4-5,6-7
```

## Funciones de Redirección de E/S

Las funciones `ioredirect` y `iorestate` se utilizan para capturar la salida de stderr o stdout en un archivo. La función `ioredirect` redirige stderr (si `err` es verdadero) o stdout (si `err` es falso) a un archivo. Devuelve un ID, que se utiliza para restablecer la salida a la normalidad mediante la función `iorestate`.

Ejemplo:

```python
int o = ioredirect('C:\Tamgu\test\test.txt', true);
printlnerr("Esta cadena ahora se almacena en el archivo: test.txt");
iorestate(o, true); // De vuelta a la normalidad
```

## Funciones de Pausa y Sueño

Las funciones `pause` y `sleep` se utilizan para poner un hilo en pausa o en modo de suspensión. La función `pause` no suspende la ejecución de un hilo, mientras que la función `sleep` sí lo hace. La función `pause` toma un número decimal como entrada, que representa la duración de la pausa en segundos. También puede tomar un segundo parámetro booleano para mostrar una pequeña animación. La función `sleep` se basa en la instrucción de suspensión del sistema operativo y su comportamiento depende de la implementación local. Toma un número entero como entrada, que representa la duración del sueño en segundos.

Ejemplo:

```python
pause(0.1); // El hilo se pausará durante 10 ms
pause(2, true); // El hilo se pausará durante 2 segundos, con una pequeña animación
sleep(1); // El hilo dormirá durante 1 segundo (dependiendo de la plataforma)
```

## Función de Emojis

La función `emojis()` devuelve una lista de todos los caracteres de emoji según la norma v5.0 beta.

## Función de Distancia GPS

La función `GPSdistance` en Tamgu se utiliza para calcular la distancia entre dos puntos GPS dados su longitud y latitud. El quinto parámetro es opcional y representa el radio de la Tierra, con un valor predeterminado de 6371 km. Este valor también define la unidad utilizada para calcular la distancia final.

Ejemplo:

```python
float d = GPSdistance(46.9001, 7.1201, 47.01, 6.1);
// Salida: d es 78.382 km
```

## Leer desde el Teclado: `kget()`

La función `kget()` en Tamgu se utiliza para leer una cadena desde el teclado.

Ejemplo:

```python
string message = kget("Mensaje:");
```

## Variables Persistentes

Puedes crear variables persistentes en el contexto de una GUI para realizar un seguimiento de ciertos valores al ejecutar tus programas repetidamente. Los siguientes tipos persistentes están disponibles: `ithrough`, `fthrough`, `uthrough`, `sthrough`, `vthrough` y `mthrough`. Estas variables nunca se reinicializan entre ejecuciones.

Ejemplo:

```python
// Esta variable llevará la cuenta de cuántas veces se ejecutó este programa
ithrough icount;
icount += 1;
println(icount);
```