# Tamgu: Una Introducción

El lenguaje Tamgu toma muchos conceptos de otros lenguajes, principalmente C++ y Python. Por lo tanto, es bastante sencillo de aprender para alguien con conocimientos básicos de estos lenguajes.

## Algunos Elementos del Lenguaje

Un programa Tamgu contiene declaraciones de variables, declaraciones de funciones y declaraciones de marcos (o clases). Las variables y funciones se pueden declarar en cualquier lugar, excepto dentro de bucles.

### Comentarios

Los comentarios de una sola línea se introducen con `//`. Por ejemplo:

```cpp
// Esto es un comentario
```

Los comentarios de un bloque de líneas se insertan entre `/@` y `@/`. Por ejemplo:

```cpp
/@
Esto es
un bloque de comentarios.
@/
```

### Funciones

Una función se declara con la palabra clave `function`, seguida de un nombre y algunos parámetros.

### Marcos

Un marco se declara con la palabra clave `frame`, seguida de un nombre. Un sub-marco simplemente se declara como un marco dentro de un marco.

### Pre-declaraciones de Funciones y Marcos

La pre-declaración de funciones y marcos no es necesaria en Tamgu, ya que el intérprete primero recorre el código para detectar todas las funciones y marcos y los declara de antemano. Por lo tanto, el siguiente código es válido:

```cpp
// Llamamos a call2 desde dentro de call1
function call1(int x, int y) {
    call2(x, y);
}

// call2 se declara después de call1
function call2(int x, int y) {
    println(x, y);
}
```

## Funciones del Sistema

### Salida: `_exit()`

Esta función se utiliza para salir del programa.

### Error en Clave: `_erroronkey(bool)`

De forma predeterminada, cualquier intento de acceder a un valor en un mapa con una clave desconocida no genera una excepción. La función `_erroronkey(bool)`, que debe colocarse al principio de su código, modifica este comportamiento.

### Tamaño de la Pila: `_setstacksize(size)`

El tamaño de la pila se establece inicialmente en 1000 llamadas a funciones. Puede modificar este valor con esta función. Sin embargo, si el tamaño de la pila es demasiado grande, es posible que su programa se bloquee, ya que podría ser mayor que el tamaño de la pila real de su sistema.

### Número de Hilos: `_setthreadlimit(nb)`

El número de hilos reales que pueden ejecutarse en paralelo se establece inicialmente en 1000 en Windows y 500 en otros sistemas. Puede modificar este valor para aumentar el número de hilos que pueden ejecutarse en paralelo.

### Características Válidas: `_setvalidfeatures(mapss features)`

Este método se utiliza para poner restricciones en las características válidas que se pueden usar para synodes y dependencias. Consulte la sección de synode, dependencia para obtener más detalles.

### Número de Hilos para Unirse en Conjunto: `_setjoinedlimit(nb)`

De forma predeterminada, hasta 256 hilos se pueden "unir" juntos. Puede modificar este número con esta función.

### Variables de Entorno Iniciales: `_setenv(varname, value)`

Es posible establecer variables de entorno en el momento del lanzamiento con esta función.

### Versión de Tamgu: `_version()`

Devuelve una cadena con información de versión sobre Tamgu.

### Visualización de Espejo: `_mirrordisplay(bool)`

Esta función se utiliza para establecer la visualización de espejo desde una GUI. Cuando está activado, "print" muestra valores tanto en la salida de la GUI como en la ventana de comandos.

### Compatibilidad de Argumentos de Función: `_loosecompatibility(bool v)`

De forma predeterminada, una llamada a una función que requiere parámetros enteros o de cadena con argumentos que no lo son falla en tiempo de compilación. Sin embargo, es posible relajar esta restricción con `_loosecompatibility(true)`. Entonces, la llamada anterior será aceptada y los argumentos se convertirán automáticamente en sus valores correctos. Los números y las cadenas se convertirán automáticamente entre sí.

### Gestión de Memoria: `_poolstats()`

Esta función se utiliza para devolver el estado actual de diferentes grupos de objetos. Por razones de eficiencia, algunos objetos se administran en grupos, lo que permite la reutilización de objetos recurrentes (como cadenas, enteros, flotantes, vectores o mapas). Estos objetos no se eliminan, sino que se borran y se devuelven para su reutilización cuando sea necesario. El resultado de esta función es un objeto `mapsi`.

## Pasar Argumentos a un Programa Tamgu

Cuando se llama a un programa Tamgu con una lista de argumentos, cada uno de estos argumentos está disponible para el programa Tamgu a través de tres variables de sistema específicas: `_args`, `_current` y `_paths`.

### Ejemplo:

```bash
tamgu myprogram c:\test\mytext.txt
```

### `_args`: Vector de Argumentos

Tamgu proporciona una variable específica llamada `_args`, que en realidad es un vector de cadenas que almacena cada argumento según su posición en la lista de declaraciones.

### Ejemplo (a partir de la llamada anterior):

```cpp
file f;
f.openread(_args[0]);
```

### `_paths` y `_current`: Gestión de Rutas

Tamgu proporciona una variable de vector llamada `_paths`, que almacena los nombres de ruta de los diferentes programas Tamgu que se han cargado. El primer elemento de este vector, `_paths[0]`, almacena la ruta del directorio actual, mientras que `_paths[1]` almacena la ruta del archivo del programa actual.

Tamgu también proporciona otra variable llamada `_current`, que almacena la ruta del archivo del programa que se está ejecutando actualmente. La ruta almacenada en `_current` siempre termina con un separador final. En realidad, `_current` apunta a la misma ruta que `_paths[1]`.

### `_endl`: Retorno de Carro

Windows y Unix utilizan caracteres de retorno de carro diferentes. `_endl` devuelve el retorno de carro adecuado según el valor de la plataforma.

### `_sep`: Separador en Nombres de Ruta

Los sistemas basados en Unix y Windows utilizan separadores diferentes en los nombres de ruta entre los nombres de directorio. Unix requiere "/" mientras que Windows requiere "\". Tamgu proporciona una variable específica llamada `_sep`, que devuelve el separador correcto según su sistema.

## Consola

Tamgu proporciona una consola predeterminada, que se puede utilizar para cargar y editar cualquier programa. La consola se puede utilizar para probar pequeños fragmentos de código o para verificar los valores al final de una ejecución.

También se puede ejecutar un programa en modo de depuración, que muestra el contenido de la pila y las variables en cada paso del programa.

Para iniciar la consola, ejecute Tamgu con la opción `-console`.