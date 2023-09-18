# Función, Autorun, Hilo

En Tamgu, una función se declara usando la palabra clave `function`, seguida de su nombre y parámetros. Un valor puede ser devuelto usando la palabra clave `return`. Los parámetros siempre se envían como valores, excepto si el tipo es `self`. Cabe destacar que una función no proporciona ninguna información de tipo para su valor de retorno.

## 7.1 Imponiendo el tipo de retorno

Una función puede especificar un tipo de retorno usando el símbolo `::` después de la lista de argumentos. Por ejemplo:

```
function toto(int i) :: int {
    i += 10;
    return i;
}
```

En este ejemplo, la función `toto` se declara para devolver un `int`.

## 7.2 Autorun

Una función `autorun` se ejecuta automáticamente después de su declaración. Las funciones autorun solo se ejecutan en el archivo principal. Si tienes funciones autorun en un archivo que es llamado desde otro archivo, esas funciones no se ejecutarán.

Ejemplo:

```
autorun waitonred() {
    println("Cargando:", _paths[1]);
}
```

## 7.3 Hilo

Cuando se lanza una función `thread`, se ejecuta en un hilo de sistema independiente.

Ejemplo:

```
thread toto(int i) {
    i += 10;
    return i;
}

int j = toto(10);
print("J=" + j + "\n");
```

Salida:

```
J=20
```

### Hilo protegido

Un hilo `protected` evita que dos hilos accedan a las mismas líneas de código al mismo tiempo. Un hilo protegido establece un bloqueo al comienzo de su ejecución, que se libera una vez que la función se ejecuta. Esto garantiza que las diferentes llamadas a una función protegida se realicen en secuencia y no al mismo tiempo. Se debe utilizar `protected` para el código que no es reentrante.

Ejemplo:

```
// Implementamos un hilo sincronizado
protected thread launch(string n, int m) {
    int i;
    println(n);
    // Mostramos todos nuestros valores
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // Lanzamos nuestro hilo
    launch("Primero", 2);
    launch("Segundo", 4);
    println("Fin");
}

principal();
```

Salida:

```
Fin
Primero
0 1
Segundo
0 1 2
```

### Hilo exclusivo

Un hilo `exclusive` es similar a un hilo protegido, pero con una diferencia. En el caso de un hilo protegido, la protección está a nivel de método, mientras que con un hilo exclusivo, está a nivel de marco. Esto significa que los hilos exclusivos solo permiten que un hilo tenga acceso al objeto de marco a la vez.

Ejemplo:

```
// Este marco expone dos métodos
frame disp {
    // Exclusivo
    exclusive thread edisplay(string s) {
        println("Exclusivo:", s);
    }
    // Protegido
    protected thread pdisplay(string s) {
        println("Protegido:", s);
    }
}

// También implementamos un marco de tarea
frame task {
    string s;
    // Con una instancia específica de "disp"
    disp cx;

    function _initial(string x) {
        s = x;
    }

    // Llamamos a nuestra instancia local con protegido
    function pdisplay() {
        cx.pdisplay(s);
    }

    // Llamamos a nuestra instancia local con exclusivo
    function edisplay() {
        cx.edisplay(s);
    }

    // Llamamos a la instancia global con exclusivo
    function display(disp c) {
        c.edisplay(s);
    }
}

// La instancia común
disp c;
vector v;
int i;
string s = "T";

for (i = 0; i < 100; i++) {
    s = "T" + i;
    task t(s);
    v.push(t);
}

// En este caso, la visualización se ordenará ya que el protegido no es reentrante
// Solo se puede ejecutar un pdisplay a la vez
for (i = 0; i < 100; i++)
    v[i].pdisplay();

// En este caso, la visualización será una mezcla de todos los edisplay trabajando en paralelo
// Dado que el exclusivo solo protege los métodos dentro de una instancia, y tenemos diferentes
// instancias en este caso...
for (i = 0; i < 100; i++)
    v[i].edisplay();

// En este último caso, tenemos un solo objeto común "disp" compartido por todas las "task"
// La visualización se ordenará nuevamente como con protegido, ya que esta vez nos encontramos con la misma
// instancia de "c disp".
for (i = 0; i < 100; i++)
    v[i].display(c);
```

### Unido y Waitonjoined

Un hilo puede declararse como unido, lo que significa que el hilo principal esperará a que se completen todos los hilos que se lanzaron antes de completar su propio código. Puedes usar `waitonjoined()` para esperar a que estos hilos terminen. Puedes usar tantos `waitonjoined()` como sea necesario en diferentes hilos. `waitonjoined()` solo espera a los hilos unidos que se lanzaron dentro de un hilo determinado.

### Valores atómicos

La mayoría de las estructuras de datos (mapas, vectores, cadenas, etc.) están protegidas en hilos contra el acceso concurrente mediante un bloqueo. Sin embargo, estos bloqueos pueden ser costosos en términos de tiempo y espacio. Para solucionar esto, Tamgu proporciona estructuras de datos sin bloqueo (tipos atómicos) como `a_int`, `a_string` o `a_mapii`. Estas estructuras de datos permiten un uso más eficiente de la máquina, ya que los hilos que acceden a estos tipos no se ponen en espera. Sin embargo, su implementación los hace más lentos que otros tipos en un contexto sin hilos. Solo son útiles cuando se comparten entre hilos.

### Operador de flujo '<<<'

Cuando lanzas un hilo, el resultado de ese hilo no se puede almacenar directamente en una variable usando el operador `=` ya que el hilo vive su propia vida sin ningún vínculo con el código que lo llama. Tamgu proporciona un operador específico para esta tarea: `<<<`, también conocido como operador de flujo. Un flujo es una variable que está conectada al hilo de tal manera que los valores devueltos por el hilo se pueden almacenar dentro de esa variable. Por supuesto, la variable debe existir independientemente del hilo.

Ejemplo:

```cpp
// Creamos un hilo como un hilo "join", para poder usar waitonjoined.
// Este hilo simplemente devuelve 2 * x
joined thread Test(int i) {
    return (i * 2);
}

// Nuestra función de lanzamiento, que lanzará 10 hilos
function launch() {
    // Primero declaramos nuestra variable de almacenamiento de mapa dentro de esta función
    treemapi m;
    int i = 0;

    // Luego lanzamos 10 hilos y almacenamos el resultado de cada uno en m en una posición específica
    for (i in <0,10,1>)
        m[i] <<< Test(i);

    // Esperamos a que todos los hilos terminen
    waitonjoined();

    // Mostramos nuestro valor final
    println(m); // {0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```

## 7.4 Múltiples definiciones

Tamgu permite múltiples definiciones de funciones que comparten el mismo nombre pero difieren en su definición de parámetros. Por ejemplo, puedes implementar un `display(string s)` y un `display(int s)`. En este caso, el sistema elegirá la función más adecuada según la lista de argumentos de la llamada a la función.

Ejemplo:

```cpp
function testmultipledeclaration(string s, string v) {
    println("String:", s, v);
}

function testmultipledeclaration(int s, int v) {
    println("Int:", s, v);
}

// Declaramos nuestras variables
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// En este caso, el sistema elegirá la función correcta según su lista de argumentos
testmultipledeclaration(s1, s2); // La implementación de cadena
testmultipledeclaration(i, j); // La implementación de entero
```

## 7.5 Argumentos predeterminados

Tamgu proporciona un mecanismo para declarar argumentos predeterminados en una función. Puedes definir un valor para un parámetro que puede omitirse en la llamada.

Ejemplo:

```cpp
function acall(int x, int y = 12, int z = 30, int u = 43) {
    println(x + y + z + u);
}

acall(10, 5); // El resultado es: 88 = 10 + 5 + 30 + 43
```

Nota: Solo los últimos parámetros en una lista de declaración pueden ser opcionales.

## 7.6 Banderas específicas: Private y Strict

Las funciones también se pueden declarar con dos banderas específicas: `private` y `strict`.

### Private

Cuando una función se declara como `private`, no se puede acceder a ella desde fuera del archivo Tamgu actual. Si se carga un programa Tamgu desde otro programa Tamgu, las funciones privadas no son accesibles desde el cargador.

Ejemplo:

```cpp
// Esta función es invisible para los cargadores externos
private function toto(int i) {
    i += 10;
    return i;
}
```

### Strict

Por defecto, cuando se declara una función en Tamgu, el lenguaje intenta convertir cada argumento de la función que llama en los parámetros esperados por la implementación de la función. Sin embargo, en algunos casos, se requiere una comprobación de parámetros más estricta. La bandera `strict` ayuda a resolver este problema al exigir un control estricto de parámetros para una función.

Ejemplo:

```cpp
strict function teststrictdeclaration(int s, int v) {
    println("Int:", s, v);
}

// Declaramos nuestras variables
string s1 = "s1";
string s2 = "s2";

// En este caso, el sistema no podrá encontrar la función correcta para estos parámetros
// y devolverá un mensaje de error
teststrictdeclaration(s1, s2); // Sin implementación de cadena
```