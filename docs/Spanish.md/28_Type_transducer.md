# Transductor de tipo

Este tipo se centra en almacenar y manejar léxicos de una manera muy compacta y eficiente. Expone los siguientes métodos:

## Métodos

1. `add(container, bool norm, int encoding)`: Transforma un contenedor (vector o mapa) en un léxico transductor. Si el contenedor es un vector, debe tener un número par de valores.
2. `build(string input, string output, bool norm, int encoding)`: Construye un archivo transductor a partir de un archivo de texto que contiene la forma superficial en la primera línea y el lema+características en la siguiente línea.
3. `compilergx(string rgx, svector features, string filename)`: Construye un archivo transductor a partir de expresiones regulares. El nombre de archivo es opcional y el autómata resultante se almacena en un archivo.
4. `load(string file)`: Carga un archivo transductor.
5. `lookdown(string lemFeat, byte lemma)`: Busca una forma superficial que coincida con un lema más características. El lema es opcional. Cuando es 1 o 2, la cadena con la que se compara puede reducirse solo a un lema. Si el lema es 2, también se devuelven las características. Nota: el lema debe estar separado de las características con una tabulación.
6. `lookup(string wrd, int threshold, int flags)`: Busca una palabra utilizando un transductor y un conjunto de acciones potenciales combinadas con un umbral. Los dos últimos argumentos pueden omitirse. Las acciones disponibles son:
   - `a_first`: El autómata puede aplicar acciones al primer carácter.
   - `a_change`: El autómata puede cambiar un carácter por otro.
   - `a_delete`: El autómata puede eliminar un carácter.
   - `a_insert`: El autómata puede insertar un carácter.
   - `a_switch`: El autómata transpone dos caracteres.
   - `a_nocase`: El autómata verifica si los dos caracteres pueden coincidir independientemente de su caso.
   - `a_repetition`: El autómata acepta que un carácter se repita varias veces.
   - `a_vowel`: El autómata compara las vocales sin acento juntas. Por ejemplo, "e" coincidirá con "é" o "è" pero no con "a".
   - `a_surface`: El autómata solo devuelve la forma superficial.
7. `parse(string sentence, int option, int threshold, int flags)`: Analiza una secuencia de palabras utilizando un transductor. La opción puede tener los siguientes valores:
   - `0`: Devuelve solo las formas superficiales que se reconocieron dentro de la cadena inicial.
   - `1`: Lo mismo que `0`, pero con sus desplazamientos.
   - `2`: Devuelve las formas superficiales y los lemas con sus características que se reconocieron en la entrada inicial.
   - `3`: Lo mismo que `2`, pero con sus desplazamientos. El umbral y las banderas son opcionales y siguen la misma convención que para `lookup`.
8. `store(string output, bool norm, int encoding)`: Almacena un transductor en un archivo. Los dos últimos parámetros son opcionales.

## Formato

El formato de los archivos compilados en léxicos, ya sea a través de `build` o `add`, tiene una estructura similar. En el caso de un archivo, la primera línea debe ser una forma superficial, mientras que la siguiente línea debe ser un lema con algunas características, separadas por una tabulación.

Ejemplo:
```
classes
class +Plural+Noun
class
class +Singular+Noun
```

La función `build` toma dicho archivo como entrada y genera un archivo que contiene el transductor correspondiente basado en estas líneas. Los otros dos parámetros se utilizan al procesar una palabra o texto.

- La normalización significa que el léxico puede coincidir con palabras sin tener en cuenta las mayúsculas y minúsculas. Por lo tanto, este léxico reconocerá "CLASS" como una palabra.
- El sistema se ha implementado para reconocer palabras en codificación UTF8 (en realidad, los transductores se almacenan en Unicode). Sin embargo, es posible indicar al sistema cómo tener en cuenta las codificaciones latinas. Por ejemplo, se puede proporcionar al sistema un valor de 5 como codificación, que en este caso se refiere a Latin 5, utilizado para codificar caracteres cirílicos. El valor predeterminado es Latin 1.

## Vector

En el caso de un vector como entrada a `add`, la estructura será un poco diferente. Las posiciones pares en el vector serán la forma superficial, mientras que las posiciones impares serán los lemas más sus características, nuevamente separados por una tabulación.

## Mapa

Para un mapa, la clave será la forma superficial y el valor será los lemas con sus características. Un mapa puede plantear problemas para almacenar palabras ambiguas.

## Procesamiento de cadenas

Hay diferentes formas de procesar cadenas con un transductor.

### `lookup`

`lookup` se utiliza para detectar si una palabra pertenece al transductor y, en este caso, devuelve su lema y características. El transductor puede devolver más de una solución. La variable receptora debe ser un vector si desea recuperar todas las soluciones posibles.

Ejemplo:
```cpp
t.lookup("class") devuelve: class +Singular+Noun
```

Puede limitar el procesamiento de una cadena con un umbral de distancia de edición y acciones.

```cpp
t.lookup("cliss", 1, a_change) devuelve: class +Singular+Noun
```

### `lookdown`

`lookdown` se utiliza para recuperar la forma superficial correcta de una palabra utilizando su lema y características.

Ejemplo:
```cpp
t.lookdown("class +Plural+Noun") devuelve: classes
```

### `parse`

`parse` divide una cadena en tokens y devuelve, para cada token, su lema+características como un vector de todas las posibilidades.

Ejemplo:
```cpp
transducer t(_current + "english.tra");
string sentence = "The lady drinks a glass of milk.";
vector v = t.parse(sentence);
printjln(v);
```

Salida:
```
['The', 'The +0+3+0+3+Prop+WordParticle+Sg+NOUN', 'the +0+3+0+3+Det+Def+SP+DET']
['lady', 'lady +4+8+4+8+Noun+Sg+NOUN']
['drinks', 'drink +9+15+9+15+Verb+Trans+Pres+3sg+VERB', 'drink +9+15+9+15+Noun+Pl+NOUN']
['a', 'a +16+17+16+17+Det+Indef+Sg+DET']
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Verb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

Nota: `parse` también devuelve la posición de cada palabra en la oración inicial.

## Expresiones regulares

Las expresiones regulares procesadas por el transductor son muy limitadas:

1. `%c`: Define un carácter, donde `c` es un carácter UTF8.
2. `$..`: Define una cadena.
3. `u-u`: Define un intervalo entre dos caracteres Unicode.
4. `[..]`: Define una secuencia de caracteres.
5. `{...}`: Define una disyunción de cadenas.
6. `.+`: La estructura debe ocurrir al menos una vez.
7. `(..)`: Define una estructura opcional.
8. `!n`: Inserta una estructura de características junto con su número en el vector de características (n>=1).

Ejemplos:
```cpp
transducer t;
// Esta expresión reconoce números romanos
t.compilergx("{DMCLXVI}+!1", ["\t+Rom"]);
// Esta expresión reconoce cualquier tipo de números, incluido el separador decimal y las expresiones exponenciales.
t.compilergx("({-+}){0-9}+!1(%.{0-9}+!2({eE}({-+}){0-9}+!3))", ["+Card", "+Dec", "+Exp+Dec"]);
// Para reconocer números ordinales
t.compilergx("{[1st][2nd][3rd]}!1", ["+Ord"]);
t.compilergx("[3-9]([0-9]+)$th!1", ["+Ord"]);
// Queremos reconocer cualquier cadena compuesta por el alfabeto griego
t.compilergx("{α-ω0-9}+!1", ["+Greek"]);

int i;
string s;
for (i in <945,970,1>) s += i.chr();
println(t.lookup("MMMDDD")); // MMMDDD +Rom
println(t.lookup("1234")); // 1234 +Card
println(t.lookup("1.234")); // 1.234 +Dec
println(t.lookup("1.234e-8")); // 1.234e-8 +Exp+Dec
println(t.lookup("1st")); // 1st +Ord
println(t.lookup("2nd")); // 2nd +Ord
println(t.lookup("3rd")); // 3rd +Ord
println(t.lookup("4th")); // 4th +Ord
println(t.lookup(s)); // αβγδεζηθικλμνξοπρςστυφχψ +Greek
```