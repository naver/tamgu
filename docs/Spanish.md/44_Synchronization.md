# Sincronización

Tamgu ofrece una forma sencilla de poner los hilos en un estado de espera. El proceso es muy simple de implementar. Tamgu proporciona diferentes funciones con este propósito:

## Métodos

1. `cast(string)`: Esta instrucción libera la ejecución de todos los hilos que esperan en la cadena especificada.
2. `cast()`: Esta instrucción libera todos los hilos, independientemente de su estado de cadena.
3. `lock(string s)`: Esta instrucción pone un candado en una porción de código para evitar que dos hilos accedan a las mismas líneas al mismo tiempo.
4. `unlock(string s)`: Esta instrucción elimina un candado para permitir que otros hilos accedan al contenido de una función.
5. `waitonfalse(var)`: Esta función pone un hilo en un estado de espera hasta que el valor de la variable `var` se establezca en falso (o cero, o cualquier cosa que devuelva falso).
6. `waitonjoined()`: Esta función espera a que los hilos lanzados dentro del hilo actual terminen. Estos hilos deben declararse con la marca `join`.
7. `wait(string)`: Esta función pone un hilo en un estado de espera utilizando una cadena como disparador. El modo de espera se libera cuando se realiza un `cast` en esa cadena.

### Ejemplo 1

```java
// Utilizamos la cadena "test" como disparador
hilo unido esperando() {
    wait("test");
    println("Liberado");
}

// Hacemos algo de trabajo y luego liberamos nuestro hilo en espera
hilo unido contando() {
    int nb = 0;
    while (nb < 10000)
        nb++;
    cast("test");
    println("Fin del conteo");
}

esperando();
contando();
waitonjoined();
println("Fuera");
```

Ejecución:

Si ejecutamos el programa anterior, Tamgu mostrará en el siguiente orden:

```
Fin del conteo
Liberado
Fuera
```

### Ejemplo 2

```java
int nb = 1000;

hilo unido decompte() {
    while (nb > 1) {
        nb--;
    }
    printlnerr("Fin del conteo", nb);
    nb--;
}

hilo unido attend() {
    waitonfalse(nb);
    printlnerr("Ok");
}

attend();
decompte();
waitonjoined();
printlnerr("Fin");
```

### Mutex: lock y unlock

Hay casos en los que es necesario evitar que ciertos hilos accedan a las mismas líneas al mismo tiempo, por ejemplo, para asegurarse de que dos llamadas a funciones se apliquen completamente antes de que otro hilo tome el control. Cuando se establece un candado en una función determinada, las líneas siguientes de esta función ya no son accesibles para otros hilos hasta que se llame a un desbloqueo.

Ejemplo:

```java
// Implementamos nuestro hilo
hilo lanzamiento(string n, int m) {
    int i;
    println(n);
    // Mostramos todos nuestros valores
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

funcion principal() {
    // Lanzamos nuestro hilo
    lanzamiento("Primero", 2);
    lanzamiento("Segundo", 4);
}
```

Si lo ejecutamos, obtendremos una visualización bastante aleatoria, ya que los hilos se ejecutan en un orden indeterminado, solo conocido por el kernel.

```
Primero
Segundo
0 1
0 1 2 3
```

Este orden se puede imponer con candados, que evitarán que el kernel ejecute el mismo conjunto de líneas al mismo tiempo.

Debemos agregar candados al código para evitar que el sistema fusionen líneas en una salida terrible:

```java
// Reimplementamos nuestro hilo con un candado
hilo lanzamiento(string n, int m) {
    lock("lanzamiento"); // Aquí ponemos el candado, nadie puede pasar más
    int i;
    println(n);
    // Mostramos todos nuestros valores
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
    unlock("lanzamiento"); // Desbloqueamos con la misma cadena, para permitir el paso.
}
```

Luego, cuando volvamos a ejecutar este fragmento de código, obtendremos una salida completamente diferente, que se ajusta más a lo que esperamos:

```
Primero
0 1
Segundo
0 1 2 3
```

Esta vez, las líneas se mostrarán según su orden en el código.

Importante: Las cadenas de candado son globales para todo el código, lo que significa que un candado en algún lugar puede desbloquearse en otro lugar. También significa que un candado en una cadena determinada puede bloquear otra parte del código que usaría la misma cadena para bloquear sus propias líneas. Por lo tanto, se recomienda usar cadenas muy específicas para diferenciar un candado de otro.

### Hilos protegidos

El ejemplo anterior podría haberse reescrito con exactamente el mismo comportamiento utilizando una función protegida.

```java
// Reimplementamos nuestro hilo como una función protegida
protegido hilo lanzamiento(string n, int m) {
    int i;
    println(n);
    // Mostramos todos nuestros valores
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}
```

Esta función dará exactamente la misma salida que la anterior. Los hilos protegidos implementan un candado al comienzo de la ejecución y lo liberan una vez que la función ha terminado. Sin embargo, la ventaja de usar candados sobre una función protegida es la posibilidad de ser mucho más preciso en qué líneas deben protegerse.

### Semáforos: waitonfalse

Si las funciones anteriores son útiles en un contexto multinúcleo, no son suficientes en algunos casos. Tamgu proporciona funciones que se utilizan para sincronizar hilos en valores de variables. Estas funciones solo se pueden asociar con tipos simples como booleano, entero, flotante o cadena.

El papel de estas dos funciones es que un hilo espere a que una variable específica alcance un valor falso. Falso se devuelve automáticamente cuando una variable numérica tiene el valor 0, cuando una cadena está vacía o cuando una variable booleana se establece en falso.

`waitonfalse(var)`: Esta función pondrá un hilo en un estado de espera hasta que la variable `var` alcance el valor falso.

Ejemplo:

```java
// Primero, declaramos una variable stopby
// Importante: su valor inicial debe ser diferente de 0
int stopby = 1;

// Implementamos nuestro hilo
hilo lanzamiento(int m) {
    // Restablecemos stopby con el número de bucles
    stopby = m;
    int i;
    // Mostramos todos nuestros valores
    for (i = 0; i < m; i++) {
        print(i, " ");
        // Decrementamos nuestra variable stopby
        stopby--;
    }
}

funcion principal() {
    // Lanzamos nuestro hilo
    lanzamiento(10);
    // Esperamos a que stopby llegue a 0...
    waitonfalse(stopby);
    println("Fin");
}

principal();
```

La ejecución retrasará la visualización de "Fin" hasta que se haya mostrado cada `i` en la pantalla.

```
0 1 2 3 4 5 6 7 8 9 Fin
```

Si eliminamos el `waitonfalse`, la salida será bastante diferente:

```
Fin 0 1 2 3 4 5 6 7 8 9
```

Como podemos ver en este ejemplo, Tamgu mostrará primero el mensaje "Fin" antes de mostrar cualquier otro valor. El `waitonfalse` sincroniza `principal` y `lanzamiento` juntos.

Nota: El ejemplo anterior podría haberse implementado con `wait` y `cast` de la siguiente manera:

```java
// Implementamos nuestro hilo
hilo lanzamiento(int m) {
    int i;
    // Mostramos todos nuestros valores
    for (i = 0; i < m; i++)
        print(i, " ");
    cast("fin");
}

funcion principal() {
    // Lanzamos nuestro hilo
    lanzamiento(10);
    wait("fin");
    println("Fin");
}

principal();
```

Sin embargo, se debe recordar que solo se puede realizar un `cast` a la vez para liberar hilos. Con una variable sincrónica, el `waitonfalse` puede ser desencadenado por diferentes hilos, no solo por el que realizaría un `cast`.

### `waitonjoined()` con la marca `join`

Cuando un hilo debe esperar a que otros hilos terminen antes de continuar, la solución más sencilla es declarar cada uno de estos hilos como `join` y luego usar el método `waitonjoined()`.

Diferentes hilos pueden esperar en un conjunto diferente de hilos unidos al mismo tiempo.

Ejemplo:

```java
// Un primer hilo con un join
hilo unido jdisplay(string s) {
    print(s + "\r");
}

// Que se lanza desde este hilo también "join"
hilo unido lanzamiento(int x) {
    int i;
    for (i = 0; i < 5000; i++) {
        string s = "Hilo:" + x + "=" + i;
        jdisplay(s);
    }
    // Esperamos a que nuestros hilos locales terminen
    waitonjoined();
    println("Fin:" + x);
}

// Lanzamos dos de ellos
lanzamiento(0);
lanzamiento(1);
// Y esperamos a que terminen...
waitonjoined();
println("Terminación");
```