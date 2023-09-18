# Funcionalidades del Sistema Tamgu

Tamgu proporciona funcionalidades del sistema, como leer el contenido de un directorio en un vector o ejecutar un comando del sistema. Expone la variable `_sys`, que se debe usar para acceder a los siguientes métodos:

## Métodos

1. `command(string s, string outputfile)`: Ejecuta el comando del sistema `s`. `outputfile` es opcional y se utiliza para redirigir la salida del comando (stdout). Si se proporciona `outputfile`, `command` también devuelve el contenido de este archivo como una cadena.

2. `createdirectory(string path)`: Crea un directorio para la ruta `path` dada. Devuelve false si el directorio ya existe o no se puede crear.

3. `env(string var)`: Devuelve el valor de la variable de entorno `var`.

4. `env(string var, string value)`: Establece el valor de la variable de entorno `var`.

5. `listdirectory(string path)`: Devuelve los archivos de un directorio como un vector.

6. `ls(string path)`: Devuelve los archivos de un directorio como un vector.

7. `mkdir(string path)`: Crea un directorio para la ruta `path` dada. Devuelve false si el directorio ya existe o no se puede crear.

8. `fileinfo(string path)`: Devuelve un mapa con la siguiente información para un archivo dado:
   - `info["size"]`: tamaño del archivo
   - `info["date"]`: fecha del archivo
   - `info["change"]`: fecha del último cambio
   - `info["access"]`: fecha del último acceso
   - `info["directory"]`: true si la ruta es un directorio
   - `info["pathname"]`: la ruta real

9. `pipe(string cmd)`: Ejecuta el comando `cmd` y devuelve un vector como resultado que contiene la salida de ese comando.

10. `realpath(string path)`: Devuelve la ruta real para una ruta relativa dada.

11. `getchar()`: Devuelve el último carácter ingresado. `getchar` no muestra los caracteres en la pantalla.

12. `colors(int style, int code1, int code2, bool disp)`: Establece el color del texto y devuelve la cadena de color.

13. `foregroundcolor(int color)`: Establece el color del texto de primer plano.

14. `backgroundcolor(int color)`: Establece el color del texto de fondo.

15. `rgbforegroundcolor(int red, int green, int blue)`: Establece el color del texto de primer plano en RGB.

16. `rgbbackgroundcolor(int red, int green, int blue)`: Establece el color del texto de fondo en RGB.

17. `scrollmargin(int top, int bottom)`: Define el área de margen de desplazamiento.

18. `deletechar(int nb)`: Elimina `nb` caracteres.

19. `up(int nb)`: Mueve hacia arriba `nb` líneas.

20. `down(int nb)`: Mueve hacia abajo `nb` líneas.

21. `right(int nb)`: Mueve hacia la derecha `nb` caracteres.

22. `left(int nb)`: Mueve hacia la izquierda `nb` caracteres.

23. `next_line(int nb)`: Mueve a la siguiente línea hacia abajo `nb` veces.

24. `previous_line(int nb)`: Mueve a la línea anterior hacia arriba `nb` veces.

25. `column(int nb)`: Mueve a la columna `nb`.

26. `row_column(int row, int column)`: Mueve a la fila/columna.

27. `home()`: Mueve el cursor al inicio.

28. `cls()`: Borra la pantalla y vuelve a la posición de inicio.

29. `hor_vert(int hor, int vert)`: Mueve a hor/vert.

30. `clearscreen(int nb)`: Borra la pantalla. `nb` puede ser 0, 1, 2 o 3 para borrar parcial o completamente la pantalla.

31. `clear()`: Borra la pantalla.

32. `eraseline(int nb)`: Borra `nb` líneas.

33. `scroll_up(int nb)`: Desplaza hacia arriba `nb` caracteres.

34. `scroll_down(int nb)`: Desplaza hacia abajo `nb` caracteres.

35. `screensizes()`: Devuelve el tamaño de la pantalla en filas y columnas.

36. `hasresized()`: Devuelve 'true' si el tamaño de la pantalla ha cambiado.

37. `cursor()`: Devuelve la posición del cursor.

38. `inmouse()`: Habilita el seguimiento del mouse.

39. `outmouse()`: Deshabilita el seguimiento del mouse.

40. `ismouseaction(string key)`: Devuelve true si es una acción del mouse.

41. `mousexy(string key)`: Devuelve la posición del mouse.

42. `mousescrollup(string key)`: Devuelve la posición del mouse al desplazarse hacia arriba.

43. `mousescrolldown(string key)`: Devuelve la posición del mouse al desplazarse hacia abajo.

44. `mousedown1(string key)`: Devuelve la posición del mouse al hacer clic primario.

45. `mousedown2(string key)`: Devuelve la posición del mouse al hacer clic secundario.

46. `mouseup(string key)`: Devuelve la posición del mouse al soltar el botón.

47. `mousetrack(string key)`: Devuelve la posición del mouse cuando el mouse se mueve mientras está presionado.

48. `reset()`: Restablece el modo del mouse y vuelve a los valores iniciales de termios. Se utiliza en máquinas Unix cuando se ha utilizado `getchar`.

49. `isescapesequence(string key)`: Devuelve true si `key` es una secuencia de escape.

50. `showcursor(bool show)`: Muestra/oculta el cursor.

51. `resizecallback(function f)`: Establece la función de devolución de llamada que se llama cuando se cambia el tamaño de la ventana del terminal. `f` es una función con dos parámetros, que son los nuevos tamaños de fila y columna.

## Ejemplo

```javascript
// Esta función copia todos los archivos de un directorio dado a otro, si son más recientes que una fecha dada
function cp(string thepath, string topath) {
    // Leemos el contenido del directorio de origen
    vector v = _sys.listdirectory(thepath);
    iterator it;
    string path;
    string cmd;
    map m;
    date t;
    // Establecemos la fecha de hoy a partir de las 9 A.M.
    t.setdate(t.year(), t.month(), t.day(), 9, 0, 0);
    it = v;
    for (it.begin(); it.nend(); it.next()) {
        path = thepath + '/' + it.value();
        // Si el archivo es del tipo correcto
        if (".cxx" in path || ".h" in path || ".c" in path) {
            m = _sys.fileinfo(path);
            // Si la fecha es más reciente que nuestra fecha actual
            if (m["date"] > t) {
                // Lo copiamos
                cmd = "copy " + path + ' ' + topath;
                println(cmd);
                // Ejecutamos nuestro comando
                _sys.command(cmd);
            }
        }
    }
}

// Llamamos a esta función para copiar de un directorio a otro
cp('C:\src', 'W:\src');
```

## Variables

Esta biblioteca también expone algunas variables:

- `_sys_keyright`: flecha derecha
- `_sys_keyleft`: flecha izquierda
- `_sys_keydown`: flecha abajo
- `_sys_keyup`: flecha arriba
- `_sys_keydel`: tecla suprimir
- `_sys_keyhomekey`: tecla de inicio
- `_sys_keyendkey`: tecla de fin
- `_sys_keyc_up`: control+flecha arriba
- `_sys_keyc_down`: control+flecha abajo
- `_sys_keyc_right`: control+flecha derecha
- `_sys_keyc_left`: control+flecha izquierda
- `_sys_keybackspace`: retroceso
- `_sys_keyescape`: tecla de escape

Para Windows, se han agregado las siguientes variables:

- `_sys_keyc_homekey`: control+tecla de inicio
- `_sys_keyc_endkey`: control+tecla de fin

Estas variables se pueden comparar con `getchar`, que devuelve estos valores cuando se presiona una tecla.

## Ejemplo

```javascript
// Comprobamos si se ha presionado un carácter
string c = _sys.getchar();
println(c.bytes()); // Mostramos su contenido

// Si el usuario presiona la flecha arriba, mostramos todos los colores posibles
if (c == _sys_keyup) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 99; j++) {
            for (int k = 0; k < 99; k++) {
                _sys.colors(i, j, k);
                println("test:", i, j, k);
            }
        }
    }
}
```

## Seguimiento del Mouse

También puedes seguir el mouse dentro de tu terminal. Aquí tienes un ejemplo:

```javascript
// Inicializamos el seguimiento del mouse
_sys.inmouse();
string u;
u = _sys.getchar();
ivector iv;
while (u.ord() != 17) {
    if (_sys.ismouseaction(u)) {
        iv = _sys.mousedown1(u);
        if (iv != [])
            println("Mouse button 1 down at position:", iv);
        iv = _sys.mousexy(u);
        if (iv != [])
            println("Mouse is at position:", iv);
        iv = _sys.mousescrollup(u);
        if (iv != [])
            println("Mouse is scrolling up at position:", iv);
        iv = _sys.mousescrolldown(u);
        if (iv != [])
            println("Mouse is scrolling down at position:", iv);
    }
    u = _sys.getchar();
}
// Desactivamos el seguimiento del mouse y restablecemos el terminal
_sys.reset();
```