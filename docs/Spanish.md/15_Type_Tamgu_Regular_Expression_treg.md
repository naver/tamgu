# Expresión Regular Tamgu

Tamgu proporciona su propio formalismo de expresiones regulares llamado `treg`. `treg` es tanto un tipo como una forma específica de escribir expresiones regulares. Como tipo, toma una cadena como argumento, que sigue la descripción a continuación. Pero también se puede proporcionar directamente como una cadena, en cuyo caso la cadena tiene la forma: `r"..."`, donde la 'r' significa expresiones regulares.

## Métodos

El tipo `treg` expone dos métodos:

1. `compile(string rgx)`: compila una cadena en un objeto `treg`.
2. `match(string s)`: verifica si la cadena `s` coincide con la expresión regular de Tamgu.

Ejemplo:

```cpp
treg tt("%C+");
if (tt.match("ABCDE"))
    println("Sí");
if (tt == "aABCDE")
    println("Sí");
else
    println("NO");
```

## Expresión r

También se pueden utilizar cadenas `r` para realizar el mismo tipo de operación:

```cpp
if (r"%C+" == "ABCDE")
    println("Sí");
```

Una expresión `r` se puede escribir con `""` o `''` (comillas dobles o comillas simples).

## Descripción del lenguaje

Una expresión regular de Tamgu es una cadena donde se utilizan meta-caracteres para introducir una cierta libertad en la descripción de una palabra. Estos meta-caracteres son:

- `%d`: representa cualquier dígito
- `%x`: representa un dígito hexadecimal (abcdef0123456789ABCDEF)
- `%p`: representa cualquier signo de puntuación perteneciente al siguiente conjunto: `< > { } [ ] ) , ; : . & | ! / \ = ~ # @ ^ ? + - * $ % ' _ ¬ £ €` “
- `%c`: representa cualquier letra minúscula
- `%C`: representa cualquier letra mayúscula
- `%a`: representa cualquier letra
- `?`: representa cualquier carácter
- `%?`: representa el carácter “?” en sí mismo
- `%%`: representa el carácter “%” en sí mismo
- `%s`: representa cualquier carácter de espacio, incluido el espacio no separable
- `%r`: representa un retorno de carro
- `%n`: representa un espacio no separable
- `~`: negación
- `\x`: carácter de escape
- `\ddd`: código de carácter en 3 dígitos exactamente
- `\xFFFF`: código de carácter en 4 hexadecimales exactamente
- `{a-z}`: entre a y z (inclusivo)
- `[a-z]`: secuencia de caracteres
- `^`: la expresión debe comenzar al principio de la cadena
- `$`: la expresión debe coincidir hasta el final de la cadena

Ejemplo:

- `dog%c` coincide con `dogs` o `dogg`
- `m%d` coincide con `m0`, `m1`, ..., `m9`

### Operadores: `*`, `+`, `()`, `([])`

Una expresión regular puede usar la convención de estrella de Kleene para definir caracteres que ocurren más de una vez.

- `x*`: el carácter puede repetirse 0 o más veces
- `x+`: el carácter debe estar presente al menos una vez
- `(x)`: el carácter es opcional
- `[xyz](+*)`: una secuencia de caracteres
- `{xyz}(+*)`: una disyunción de caracteres

Aquí, `x` puede ser un carácter o un meta-carácter. Hay un caso especial con el `*` y el `+`. Si el carácter que se va a repetir puede ser cualquier carácter, entonces se debe usar `%+` o `%*`.

Importante: Estas dos reglas también son equivalentes a `?*` o `?+`.

Ejemplo:

1. `a*ed` coincide con `aed`, `aaed`, `aaaed`, etc. (la `a` puede estar presente 0 o más veces)
2. `a%*ed` coincide con `aed`, `aued`, `auaed`, `aubased`, etc. (cualquier carácter puede ocurrir entre `a` y `ed`)
3. `a%d*` coincide con `a`, `a1`, `a23`, `a45`, `a765735`, etc.
4. `a{%d %p}` coincide con `a1`, `a/`, `a`, etc.
5. `a{bef}` coincide con `ab`, `ae` o `af`
6. `a{%dbef}` coincide con `a1`, `ab`, `ae`, `af`, `a0`, `a9`, etc.
7. `a{be}+` coincide con `ab`, `ae`, `abb`, `abe`, `abbbe`, `aeeeb`, etc.
8. `a[be]+` coincide con `abe`, `abebe`, etc.

## Macros: `grammar_macros(key, pattern)`

Algunas expresiones pueden ser un poco complejas de escribir. Tamgu proporciona el procedimiento `grammar_macros`, que crea un nuevo meta-carácter que se puede usar en expresiones. El primer argumento es un carácter que se utilizará como índice, mientras que el segundo es un patrón de expresión regular que se asociará con esta clave.

Esta función proporciona patrones para todas las llamadas a expresiones regulares de gramática.

Ejemplo:

```cpp
grammar_macros("X", "({%+ -})0x%x+(.%x+)(p({%+ -})%d+)");
string s = "ceci est: 0x1.0d0e44bfeec9p -3 dans la chaine.";
// Usamos la macro
string res = s.scan("%X");
println(res); // 0x1.0d0e44bfeec9p -3
```

IMPORTANTE: `grammar_macros` es una función del sistema, lo que significa que se ejecuta mientras se analiza el código. No se ejecutará con el resto del código una vez que se haya compilado.

## Uso de `treg` en cadenas

También es posible usar `treg` directamente en cadenas para extraer o modificar su contenido.

```cpp
string uu = "Eso fue: 0x1.0d0e44bfeec9p -3, 0x3.456aebp -1 aquí.";
print("Tst:<", uu[-r"%d":" "], ">\n"); // Tst:<1>
println("Tst2:", uu["x":r"%d %c"]); // Tst2: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

Tenga en cuenta que también es posible reemplazar una expresión `treg` con una variable de tipo `treg`:

```cpp
treg subt(r"%d %c");
println("Tst3:", uu["x":subt]); // Tst3: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

## Taskell

También se pueden usar estas expresiones `treg` con las funciones de Taskell:

```cpp
<mytreg(r"%C+") = "mayúscula">
<mytreg([r"%d+":v]) = "ok">
<mytreg(v) = "falló">
println(mytreg("ABC")); // mayúscula
println(mytreg(["3a4", 1, 4, 5])); // ok
```

## Prolog

También se pueden usar en Prolog:

```cpp
tst(r"%d+"). // la expresión `treg` está aquí...
tst("cosa").
truc("Ok", ?P) :- tst(?P).
vector vpred;
vpred = truc(?P, "cosa");
println(vpred); // [truc("Ok",cosa)]
vpred = truc(?P, "12");
println(vpred); // [truc("Ok",12)]
vpred = truc(?P, "nope");
println(vpred); // []
```

## Expresiones Regulares Posix: `preg`

Las expresiones regulares posix también están disponibles en Tamgu, de la misma manera que las expresiones regulares de Tamgu. Sin embargo, hay dos diferencias menores. Primero, para usar estas expresiones, debes anteponer tu expresión con `p` en lugar de `r`. Segundo, el tipo de objeto asociado es `preg`.

Estas expresiones regulares se basan en el esquema de expresiones regulares posix. Se pueden usar en lugar de `treg` en todas partes, excepto para el método `scan`, en cadenas y archivos.

Ejemplo:

```cpp
string str = "este tema tiene un submarino como subsecuencia";
svector vs = p'\b(sub)([^ ]*)' in str;
println(vs); // ['tema', 'submarino', 'subsecuencia']
ivector iv = p'\b(sub)([^ ]*)' in str;
println(iv); // [5, 12, 19, 28, 34, 45]
string s = p'\b(sub)([^ ]*)' in str;
println(s); // tema
s = str[p"\b(sub)([^ ]*)"];
println("S=" + s); // S=tema
s = str[-p"\b(sub)([^ ]*)"];
println("S=" + s); // S=subsecuencia
```