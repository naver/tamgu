# Manipulación de cadenas en Tamgu

Tamgu ofrece varios tipos para manejar cadenas: `string`, `ustring` y `rawstring`.

- El tipo `string` se utiliza para manejar cualquier tipo de cadena. Proporciona muchos métodos diferentes para extraer subcadenas, caracteres o aplicar reconocimiento de patrones sobre ellas.
- El tipo `ustring` se utiliza para ofrecer un acceso más rápido a cadenas muy grandes, ya que el sistema asume una sola codificación para toda la cadena. Está basado en la implementación de `wstring` en C++.
- El tipo `rawstring` maneja cadenas a nivel de byte. Acepta cadenas pero las maneja a nivel de byte. No requiere protección específica en los hilos y se puede acceder y modificar libremente. Sin embargo, no se puede redimensionar una `rawstring` si los hilos se están ejecutando en segundo plano.

## Métodos

### Conversión de base

- `base(int b, bool toconvert)`: Devuelve el valor numérico correspondiente al contenido numérico de la cadena en base `b`. El parámetro `toconvert` es opcional. Cuando es `false`, el número a convertir está en base 10 y se convierte a base `b`.
- `base(vector chrs)`: Establece la codificación para cada dígito en una base dada. El conjunto predeterminado es de 64 caracteres: 0-9, A-Z, a-z, #, @. Puede reemplazar este conjunto de caracteres predeterminado con el suyo propio. Si proporciona un vector vacío, el sistema se restablece al conjunto predeterminado de caracteres.

### Extracción y manipulación de cadenas

- `bytes()`: Devuelve un ivector de bytes que coinciden con la cadena.
- `charposition(int pos)`: Convierte una posición de byte en una posición de carácter (especialmente útil en cadenas UTF8).
- `deaccentuate()`: Elimina los acentos de los caracteres acentuados.
- `doublemetaphone()`: Devuelve un svector que contiene la codificación de doble-metaphone de la cadena.
- `dos()`: Convierte una cadena a la codificación DOS.
- `dostoutf8()`: Convierte una cadena DOS en la codificación UTF8.
- `emoji()`: Devuelve la descripción textual (en inglés) de un emoji.
- `evaluate()`: Evalúa los meta-caracteres dentro de una cadena y devuelve la cadena evaluada.
- `extract(int pos, string from, string up1, string up2...)`: Devuelve un svector que contiene todas las subcadenas de la cadena actual, comenzando en la posición `pos`, que están compuestas por `from` hasta una de las siguientes cadenas `up1`, `up2`, etc.
- `fill(int nb, string char)`: Crea una cadena de `nb` caracteres.
- `find(string sub, int pos)`: Devuelve la posición de la subcadena `sub` que comienza en la posición `pos`.
- `format(p1, p2, p3)`: Crea una nueva cadena a partir de la cadena actual en la que cada '%x' está asociado a uno de los parámetros, siendo 'x' la posición de ese parámetro en la lista de argumentos. 'x' comienza en 1.
- `geterr()`: Captura la salida de error actual.
- `getstd()`: Captura la salida estándar actual.
- `html()`: Devuelve la cadena en una cadena compatible con HTML o como un vector de cadenas.
- `insert(i, s)`: Inserta la cadena `s` en la posición `i`. Si `i` es -1, entonces inserta `s` entre cada carácter en la cadena de entrada.
- `isalpha()`: Comprueba si una cadena contiene solo caracteres alfabéticos.
- `isconsonant()`: Comprueba si una cadena contiene solo consonantes.
- `isdigit()`: Comprueba si una cadena contiene solo dígitos.
- `isemoji()`: Comprueba si una cadena contiene solo emojis.
- `islower()`: Comprueba si una cadena contiene solo caracteres en minúscula.
- `ispunctuation()`: Comprueba si la cadena está compuesta por signos de puntuación.
- `isupper()`: Comprueba si una cadena contiene solo caracteres en mayúscula.
- `isutf8()`: Comprueba si una cadena contiene caracteres UTF8.
- `isvowel()`: Comprueba si una cadena contiene solo vocales.
- `last()`: Devuelve el último carácter.
- `latin()`: Convierte una cadena UTF8 a LATINA.
- `left(int nb)`: Devuelve los primeros `nb` caracteres de una cadena.
- `levenshtein(string s)`: Devuelve la distancia de edición con `s` según el algoritmo de Levenshtein.
- `parenthetic()`: Convierte una expresión entre paréntesis en un vector.
- `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: Convierte una expresión recursiva utilizando caracteres de apertura y cierre como separadores.
- `lower()`: Devuelve la cadena en caracteres en minúscula.
- `mid(int pos, int nb)`: Devuelve los `nb` caracteres a partir de la posición `pos` de una cadena.
- `ngrams(int n)`: Devuelve un vector de todos los ngramas de rango `n`.
- `ord()`: Devuelve el código de un carácter de cadena.
- `parse()`: Analiza una cadena como un fragmento de código y devuelve la evaluación en un vector.
- `pop()`: Elimina el último carácter.
- `pop(i)`: Elimina el carácter en la posición `i`.
- `read(string file)`: Lee un archivo en la cadena.
- `removefirst(int nb)`: Elimina los primeros `nb` caracteres de una cadena.
- `removelast(int nb)`: Elimina los últimos `nb` caracteres de una cadena.
- `replace(sub, str)`: Reemplaza las subcadenas que coinciden con `sub` por `str`.
- `reverse()`: Invierte la cadena.
- `rfind(string sub, int pos)`: Devuelve la posición de la subcadena `sub` hacia atrás comenzando en la posición `pos`.
- `right(int nb)`: Devuelve los últimos `nb` caracteres de una cadena.
- `scan(rgx, string sep, bool immediatematch, string remaining)`: Devuelve todas las subcadenas que coinciden con `rgx` según el formalismo de expresiones regulares de Tamgu.
- `size()`: Devuelve la longitud de una cadena.
- `slice(int n)`: Divide una cadena en subcadenas de tamaño `n`.
- `split(string splitter)`: Divide una cadena a lo largo de `splitter` y almacena los resultados en un svector.
- `splite(string splitter)`: Divide una cadena de la misma manera que `split` anteriormente, pero mantiene las cadenas vacías en el resultado final.
- `multisplit(string sp1, string sp2...)`: Divide una cadena a lo largo de varias cadenas separadoras.
- `stokenize(map keeps)`: Tokeniza una cadena en palabras y puntuaciones.
- `tags(string o, string c, bool comma, bool separator, bool keepwithdigit, svector rules)`: Analiza una cadena como una expresión entre paréntesis, donde se proporcionan las cadenas de apertura y cierre.
- `tokenize(bool comma, bool separator, svector rules)`: Tokeniza una cadena en palabras y puntuaciones.
- `trim()`: Elimina los caracteres finales.
- `trimleft()`: Elimina los caracteres finales a la izquierda.
- `trimright()`: Elimina los caracteres finales a la derecha.
- `upper()`: Devuelve la cadena en caracteres en mayúscula.
- `utf8()`: Convierte una cadena LATINA en UTF8.
- `utf8(int part)`: Convierte una cadena en Latín codificada en la parte ISO 8859 `part` en UTF8.
- `write(string file)`: Escribe el contenido de la cadena en un archivo.

## Manipulación de cadenas

Tamgu también proporciona métodos específicos para cadenas coreanas y tablas latinas. Estos métodos se utilizan con los tipos `string` y `ustring`.

### Métodos específicos para el coreano

- `ishangul()`: Devuelve `true` si es un carácter Hangul.
- `isjamo()`: Devuelve `true` si es un jamo Hangul.
- `jamo(bool combine)`: Si `combine` es `false` o está ausente, divide un jamo coreano en sus componentes principales de consonante y vocal. Si `combine` es `true`, combina el contenido en un jamo.
- `normalizehangul()`: Normaliza diferentes codificaciones UTF8 de caracteres Hangul.
- `romanization()`: Romanización de caracteres Hangul.

### Tabla latina

Tamgu proporciona nombres de codificación y constantes para acceder a estas codificaciones. Los nombres de codificación son:

- `e_latin_we`: Europa Occidental
- `e_latin_ce`: Europa Central
- `e_latin_se`: Europa del Sur
- `e_latin_ne`: Europa del Norte
- `e_cyrillic`: Cirílico
- `e_arabic`: Árabe
- `e_greek`: Griego
- `e_hebrew`: Hebreo
- `e_turkish`: Turco
- `e_nordic`: Nórdico
- `e_thai`: Tailandés
- `e_baltic`: Borde del Báltico
- `e_celtic`: Celta
- `e_latin_ffe`: Extendido (Francés, Finlandés, Estonio)
- `e_latin_see`: Europa del Sureste
- `e_windows`: Codificación de Windows

## Operadores

Tamgu admite varios operadores para la manipulación de cadenas:

- `sub in s`: Comprueba si `sub` es una subcadena de `s`.
- `for (c in s) {…}`: Recorre todos los caracteres en `s`.
- `+`: Concatena dos cadenas.
- `"…"`: Define una cadena, donde los meta-caracteres como `\n`, `\t`, `\r`, `\"` se interpretan.
- `'…'`: Define una cadena, donde los meta-caracteres no se interpretan.

## Índices

Tamgu permite indexar y modificar cadenas utilizando índices:

- `str[i]`: Devuelve el `i`-ésimo carácter de una cadena.
- `str[i:j]`: Devuelve la subcadena entre `i` y `j`.
- `str[s..]`: Devuelve la subcadena a partir de la cadena `s`.
- `str[-s..]`: Devuelve la subcadena a partir de la cadena `s` desde el final de la cadena.

Si un índice está fuera de rango, se genera una excepción a menos que el indicador `erroronkey` se haya establecido en `false`.

## Ejemplos

Aquí hay algunos ejemplos de manipulación de cadenas en Tamgu:

```tamgu
string s;
string x;
vector v;

// Algunas manipulaciones básicas de cadenas
s = "12345678a";
x = s[0];  // valor=1
x = s[2:3];   // valor=3
x = s[2:-2];  // valor=34567
x = s[3:];  // valor=45678a
x = s[:"56"];  // valor=123456
x = s["2":"a"];  // valor=2345678a
s[2] = "ef";  // valor=vacío

// Los últimos 3 caracteres
x = s.right(3);  // valor=78a

// Una división a lo largo de un espacio
s = "a b c";
v = s.split(" ");  // v=["a", "b", "c"]

// Regex, x es una cadena, buscamos la primera coincidencia de la expresión regular
x = s.scan("%d%d%c");  // valor=78a

// Tenemos un patrón, dividimos nuestra cadena a lo largo de ese patrón
s = "12a23s45e";
v = s.scan("%d%d%c");  // valor=['12a', '23s', '45e']

x = s.replace(r"%d%ds", "X"); // valor=12aX45e

// Replace también acepta variables %x como en las expresiones regulares de Tamgu
x = s.replace(r"%d%1s", "%1"); // valor=12a2345e

// Expresiones regulares: No disponibles en todas las plataformas
preg rgx(p"\w+day");
string str = "Yooo Wesdenesday Saturday";
vector vrgx = rgx in str; // ['Wesdenesday', 'Saturday']
string s = rgx in str; // Wesdenesday
int i = rgx in str; // la posición es 5

// Usamos (…) para aislar tokens específicos que se almacenarán en el vector
rgx = p"(\d{1,3}):( \d{1,3}):( \d{1,3}):( \d{1,3})";
str = '1:22:33:444';
vrgx = str.split(rgx);  // [1, 22, 33, 444], rgx es una expresión de división

str = '1:22:33:4444';
vrgx = str.split(rgx);  // [] (4444 contiene 4 dígitos)

str = "A_bcde";
// Se requiere una coincidencia completa
if (p'[a-zA-Z]_.+' == str)
    println("Yooo"); // Yooo

// Esto también es equivalente a:
rgx = p'[a-zA-Z]_.+';
if (rgx.match(str))
    println("Yooo bis");

str = "ab(Tamgu 12,Tamgu 14,Tamgu 15,Tamgu 16)";
v = str.extract(0, "Tamgu ", ",", ")");
// Resultado: ['12', '14', '15', '16']

string frm = "this %1 is a %2 of %1 with %3";
str = frm.format("tst", 12, 14);
println(str); // Resultado: this tst is a 12 of tst with 14
```