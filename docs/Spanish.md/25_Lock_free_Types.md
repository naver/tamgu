# Tipos sin bloqueo

Los tipos sin bloqueo son extremadamente útiles en aplicaciones de multithreading, ya que permiten la manipulación de números, cadenas, vectores o mapas sin necesidad de bloqueos. Los bloqueos pueden afectar significativamente el rendimiento de ejecución. Cuando un hilo se pone en espera debido a un bloqueo, el sistema debe almacenar su contexto y reactivarlo una vez que se libere el bloqueo. La programación sin bloqueo tiene como objetivo minimizar el uso de bloqueos para evitar interrupciones costosas en el flujo de ejecución.

## Tabla y Cadena sin formato

En las secciones anteriores, hemos discutido dos objetos sin bloqueo: tabla y cadena sin formato.

Estos objetos tienen la característica única de que su tamaño es fijo y no puede modificarse dentro del contexto de un hilo. Esto garantiza que el acceso concurrente a estos objetos no interfiera con su declaración de búfer interno.

## Valores atómicos: a_bool, a_int, a_float, a_string, a_ustring

Tamgu también proporciona cinco valores atómicos para booleanos, enteros, flotantes y cadenas. Estos objetos se pueden utilizar como valores regulares, pero en caso de acceso concurrente, su atomicidad está garantizada. Atomicidad, en este contexto, significa que solo un hilo puede modificar su valor a la vez.

## Contenedores: a_vector, a_[i|f|s|u]vector, a_map [i|f|s|u][i|f|s|u]

Tamgu también ofrece contenedores que pueden ser modificados por diferentes hilos simultáneamente con un uso mínimo de bloqueos. Estas estructuras se pueden utilizar de la misma manera que sus mapas no atómicos correspondientes. Se implementan como una lista enlazada de fragmentos, donde cada fragmento puede acomodar un número significativo de valores. Los bloqueos solo se utilizan cuando se agrega un nuevo elemento a la lista enlazada. Las operaciones de lectura y la mayoría de las operaciones de almacenamiento se realizan sin bloqueos, excepto cuando el mapa o el vector no tienen suficiente capacidad para acomodar un nuevo elemento.

Nota: los a_map son contenedores de valores, mientras que a_vector es un contenedor de objetos.

Nota: Debes cargar la biblioteca "allmaps" para acceder a estos mapas.

## Anillo

"Anillo" es otro contenedor sin bloqueo que puede contener un máximo de 65535 elementos. Se implementa como una estructura donde los elementos se pueden almacenar o eliminar desde el frente o la parte posterior. A diferencia de otros contenedores, estas operaciones tienen el mismo tiempo de memoria o espacio.

## Importante

Cuando varios hilos intentan modificar la misma variable sin bloqueo, no hay garantía de que ambas modificaciones sean exitosas. El último hilo que acceda a la variable puede ser el que modifique su valor.