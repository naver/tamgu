# Gestión de archivos

El tipo `file` se utiliza para gestionar archivos de entrada y salida. El tipo `wfile` se utiliza específicamente para manejar archivos UTF16 (UCS-2).

## Métodos

1. `eof()`: Devuelve verdadero cuando se alcanza el final del archivo.
2. `file f(string filename, string mode_read)`: Abre un archivo según el `mode_read` especificado. Si el archivo está en modo de lectura, `mode_read` es opcional. Los valores posibles para `mode_read` son:
   - "a": añadir
   - "r": leer
   - "w": escribir
   - "w+": añadir
3. `find(string s, bool case_insensitive)`: Devuelve todas las posiciones en el archivo donde se encuentra la cadena `s`.
4. `get()`: Lee un carácter del archivo.
5. `get_signature()`: Devuelve si el archivo contiene una firma.
6. `open_append(string filename)`: Abre un archivo en modo de añadir.
7. `open_read(string filename)`: Abre un archivo en modo de lectura.
8. `open_write(string filename)`: Abre un archivo en modo de escritura.
9. `read()`: Lee todo el archivo en una variable, que puede ser:
   - string: todo el documento se almacena en una cadena
   - svector: el documento se divide en cadenas a lo largo de los saltos de línea, y cada cadena se almacena en el contenedor
   - bvector: el documento se almacena byte a byte en el contenedor
   - ivector: el documento se almacena byte a byte en el contenedor
10. `read(int nb)`: Similar a `read()`, pero extrae solo "nb" caracteres o bytes del archivo.
11. `readln()`: Lee una línea del archivo.
12. `seek(int p)`: Posiciona el cursor del archivo en la posición `p`.
13. `set_signature(bool s)`: Establece la firma UTF8 o UTF16 en consecuencia.
14. `tell()`: Devuelve la posición del cursor del archivo.
15. `unget()`: Devuelve un carácter al flujo.
16. `unget(nb)`: Devuelve `nb` caracteres al flujo.
17. `scan(string grammar, string separator)`: Lee un archivo con una Expresión Regular de Tangu y devuelve un vector de subcadenas. El separador es un carácter que separa un campo de otro (el valor predeterminado es un espacio).
18. `write(string s1, string s2, ...)`: Escribe la cadena `s` en el archivo.
19. `writelen(string s1, string s2, ...)`: Escribe cadenas en el archivo, separando cada cadena con un espacio y agregando un salto de línea al final de la línea.
20. `writebin(int s1, int s2, ...)`: Escribe bytes en el archivo. Si el valor es un contenedor, escribe la lista de bytes desde ese contenedor.

## Firma

Los archivos UTF-8 y UTF-16 pueden tener una firma al comienzo. Un archivo UTF-8 tiene tres octetos para la firma, mientras que un archivo UTF-16 tiene dos octetos.

- Cuando se utiliza el tipo `file`, la firma debe establecerse de antemano para poder leerla. Este tipo solo se puede utilizar para leer archivos UTF-8 o binarios.
- En el caso de `wfile`, la firma se establece automáticamente cuando se encuentra al comienzo del archivo. Este tipo solo se puede utilizar para leer archivos UTF-16 (UCS-2).

## Operador

- `x in file`: Si `x` es una cadena, recibe una línea del archivo. Si `x` es un vector, empuja la línea a la parte superior del vector. Si `x` es un entero o un flotante, recupera solo un carácter del flujo.

## Ejemplo

```cpp
file f;
f.open_read(path);
string s;
svector words;
string w;

for (s in f) { // Usando el operador 'in'
    s = s.trim();
    words = s.split(" ");
    for (w in words)
        print("palabra:", w, endl);
}

f.close();
```

## Entrada estándar: stdin

Tamgu proporciona la variable `stdin` para manejar la entrada estándar. Esta variable es útil para manejar datos que provienen de un archivo enlazado, por ejemplo.

### Ejemplo

```cpp
string s;
int i = 1;

for (s in stdin) {
    println(i, s);
    i++;
}
```

Si almacena estas líneas en un archivo pequeño, digamos `stdin.Tamgu`, entonces el contenido de las cadenas enlazadas se mostrará con un número específico para cada línea:

```bash
echo "La dama está feliz" | Tamgu stdin.Tamgu
```

### Ejemplo (scan)

El texto contiene líneas como:
```
456 -0x1.16bca4f9165dep -3 0x1.0d0e44bfeec9p -3
```

```cpp
file f(_current + "tst.txt");
// Definir una macro para leer una cadena hexadecimal compleja
grammar_macros("X", "({%+ -})0x%x+(%.%x+)(p({%+ -})%d+)");
// Leer los dos primeros campos. El espacio es el separador predeterminado.
uvector v = f.scan("%d+ %X ");
println(v); // ['456', ' -0x1.16bca4f9165dep -3']
// Leer el siguiente campo.
v = f.scan("%X ");
println(v); // ['0x1.0d0e44bfeec9p -3']
f.close();
```