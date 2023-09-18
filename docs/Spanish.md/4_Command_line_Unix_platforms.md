## Línea de Comandos (Plataformas Unix)

Puedes utilizar Tamgu como una herramienta de línea de comandos en plataformas Linux o Mac OS. Tamgu ofrece los siguientes comandos:

- **-i**: Este comando te permite pre-declarar variables. Por ejemplo, `bool a,b,c; int i,j,k; float f,g,h; string s,t,u; map m; vector v; self x,y,z;`
- **-c**: Permite ejecutar un fragmento de código Tamgu proporcionado después del comando. Por ejemplo, `tamgu -c 'println(10+20);'`
- **-a**: Intercepta los envíos en stdio y los registra en el vector _args. Es posible proporcionar un fragmento de código.
- **-p**: Intercepta los envíos línea por línea en stdio y permite la ejecución de un fragmento de código donde la variable pre-declarada l contiene la línea actual de stdio. Por ejemplo, `ls -l | tamgu -p 'println(l);'`
- También se puede llamar a Tamgu con un nombre de programa y una lista de argumentos. Esta lista de argumentos estará disponible en _args. Por ejemplo, `tamgu mycode.tmg a1 a2 a3 a3`. _args contiene: ["a1", "a2", "a3"]
- También es posible combinar las líneas de stdio y la llamada de un programa con sus argumentos. La opción "-a" debe colocarse después del nombre del programa. Por ejemplo, `ls -1 | tamgu mycode.tmg -a a1 a2 a3`

## IDE Integrado

Tamgu proporciona un entorno de desarrollo integrado en modo texto que puedes utilizar para crear, modificar e incluso depurar tus programas. Puedes llamar a este entorno de dos formas diferentes:

- Si ejecutas "tamgu" sin argumentos, puedes ingresar el comando "edit" que te llevará al modo de edición.
- También puedes lanzar: "tamgu -e myfile.tmg" que abrirá tu archivo y automáticamente te llevará al modo de edición. Si "myfile.tmg" no existe, puedes crearlo la primera vez que lo guardes.

### Modo de Edición/Ejecución

Puedes utilizar Tamgu tanto para editar tus archivos como para ejecutarlos. Puedes lanzar la ejecución de tu archivo desde el editor con "Ctrl-Xr". Una vez que tu código ha sido ejecutado, pasarás al "modo de ejecución" donde puedes probar tus variables. También puedes cambiar al "modo de ejecución" con "Ctrl-c". Para volver al "modo de edición" escribe: edit.

### Ayuda

El IDE ofrece muchos comandos, incluyendo la siguiente ayuda en línea, que se puede llamar ya sea con el comando "help" en el modo de ejecución o con "Ctrl-Xh" en el modo de edición.

#### Comandos:

1. **Programas**:
   - create filename: crea un espacio de archivo con un nombre de archivo específico
   - open filename: carga un programa (usa run para ejecutarlo)
   - open: vuelve a cargar el programa (usa run para ejecutarlo)
   - save filename: guarda el contenido del búfer en un archivo
   - save: guarda el contenido del búfer con el nombre de archivo actual
   - run filename: carga y ejecuta un programa filename
   - run: ejecuta el contenido del búfer
   - debug: modo de depuración
   - n!: siguiente línea
   - l!: muestra las variables locales
   - a!: muestra todas las variables activas
   - s!: muestra la pila
   - g!: ir al siguiente punto de interrupción
   - e!: ejecutar hasta el final
   - S!: detener la ejecución
   - nombre: muestra el contenido de una variable (solo escribe su nombre)
   - h!: muestra ayuda de depuración
   - args v1 v2...vn: establece una lista de valores para la lista de argumentos
   - filename: muestra el nombre de archivo actual

2. **Modo de Línea de Comandos**:
   - help: muestra la ayuda
   - help n: muestra una de las secciones de ayuda (de 1 a 5)
   - history: muestra el historial de comandos
   - load filename: carga un archivo de historial de comandos
   - store filename: guarda el historial de comandos en un archivo
   - filename: muestra el nombre de archivo actual
   - filespace: muestra todos los archivos almacenados en memoria con su identificación de espacio de archivo
   - select idx: selecciona un espacio de archivo
   - metas: muestra los meta-caracteres
   - rm: borra el contenido del búfer
   - rm b:e: elimina las líneas desde b hasta e (también es posible b: o :e)
   - list: muestra el contenido del búfer
   - list b:e: muestra el contenido del búfer desde la línea b hasta e (también es posible b: o :e)
   - Ctrl-t: sube en la lista de código
   - Ctrl-g: baja en la lista de código
   - Ctrl-f: fuerza que la línea actual se agregue al final del código
   - ?method: devuelve una descripción para 'method'
   - ?o.method: devuelve una descripción para 'method' de la variable 'o'

3. **Editar (idx)**: modo de edición. Opcionalmente, también puedes seleccionar un espacio de archivo
   - Ctrl-b: alternar punto de interrupción
   - Ctrl-k: eliminar desde el cursor hasta el final de la línea
   - Ctrl-d: eliminar una línea completa
   - Ctrl-u: deshacer la última modificación
   - Ctrl-r: rehacer la última modificación
   - Ctrl-f: buscar una cadena
   - Ctrl-n: buscar siguiente
   - Ctrl-g: moverse a una línea específica, '$' es el final del código
   - Ctrl-l: alternar entre la parte superior e inferior de la pantalla
   - Ctrl-t: reindentar el código
   - Ctrl-h: ayuda local
   - Ctrl-w: escribir archivo en disco
   - Ctrl-c: salir del editor
   - Ctrl-x: Comandos combinados
   - C: contar un patrón
   - H: convertir entidades HTML en caracteres Unicode
   - D: eliminar un bloque de líneas
   - c: copiar un bloque de líneas
   - x: cortar un bloque de líneas
   - v: pegar un bloque de líneas
   - d: ejecutar en modo de depuración
   - r: ejecutar el código
   - w: escribir y salir
   - l: cargar un archivo
   - m: mostrar meta-caracteres
   - h: ayuda completa
   - q: salir

4. **Entorno**:
   - cls: limpiar pantalla
   - color: mostrar colores de terminal
   - color att fg bg: mostrar un color a partir de atributo, primer plano, fondo
   - colors: mostrar los colores para el coloreado de código
   - colors name att fg bg: modificar el color para una de las siguientes denominaciones = cadenas, métodos, palabras clave, funciones, comentarios

5. **Sistema**:
   - !unix: lo que sigue a '!' se ejecutará como un comando Unix (por ejemplo, !ls)
   - clear (idx): borrar el entorno actual o un espacio de archivo específico
   - reinit: borrar el contenido del búfer e inicializar variables pre-declaradas
   - Ctrl-d: finalizar la sesión y salir de Tamgu
   - exit: finalizar la sesión y salir de Tamgu

6. **Expresiones regulares para 'find'**:
   - %d representa cualquier dígito
   - %x representa un dígito hexadecimal (abcdef0123456789ABCDEF)
   - %p representa cualquier signo de puntuación
   - %c representa cualquier letra minúscula
   - %C representa cualquier letra mayúscula
   - %a representa cualquier letra
   - ? representa cualquier carácter
   - %? representa el carácter "?" en sí mismo
   - %% representa el carácter "%" en sí mismo
   - %s representa cualquier carácter de espacio, incluido el espacio sin separación
   - %r representa un retorno de carro
   - %n representa un espacio sin separación
   - ~ negación
   - \x carácter de escape
   - \ddd código de carácter en 3 dígitos exactamente
   - \xFFFF código de carácter en 4 hexadecimales exactamente
   - {a-z} entre a y z inclusive
   - [a-z] secuencia de caracteres
   - ^ la expresión debe comenzar al principio de la cadena
   - $ la expresión debe coincidir hasta el final de la cadena

#### Ejemplos:

- `dog%c` coincide con dogs o dogg
- `m%d` coincide con m0, m1,...,m9
- `{%dab}` coincide con 1, a, 2, b
- `{%dab}+` coincide con 1111a, a22a90ab