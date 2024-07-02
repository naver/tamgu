# Tipo Grammar

El tipo `grammar` está diseñado para proporcionar a los programadores una forma poderosa de describir estructuras de cadenas complejas. Es particularmente útil para detectar subcadenas específicas en un texto que involucran dígitos, letras mayúsculas o signos de puntuación en un orden estricto.

## Métodos

Hay dos funciones que se exponen con este tipo:

1. `apply(cadena|vector)`: Aplica una gramática a un texto, transformándolo en un vector de caracteres o un vector de tokens.
2. `load(regla, int saltarblancos)`: Carga las reglas como una cadena o un vector de reglas. El parámetro `saltarblancos` es opcional y puede tener los valores:
   - 0: Todos los caracteres se tienen en cuenta en la gramática. Este es el valor predeterminado cuando se omite `saltarblancos`.
   - 1: Los espacios en blanco y las tabulaciones se omiten automáticamente antes de aplicar un objetivo a las subcadenas. También se omiten los caracteres finales al final de las cadenas.
   - 2: Se omiten todos los espacios, incluyendo los retornos de carro.

El operador "in" también se puede usar con una gramática para detectar si una cadena es compatible con la gramática.

## Reglas

Las reglas se pueden implementar como un solo texto o como un vector de cadenas, donde cada cadena representa una regla.

### Formato de la Regla

El formato de una regla es el siguiente:

```
cabeza := (~) elemento [,;] elemento .
```

donde `elemento` puede ser:
- Una cadena: Encerrada entre comillas o comillas simples.
- ? : Cualquier carácter.
- %a : Cualquier carácter alfabético.
- %c : Cualquier carácter en minúscula.
- %C : Cualquier carácter en mayúscula.
- %d : Un dígito.
- %e : Un carácter de emoji.
- %H : Un carácter Hangul.
- %r : Un retorno de carro.
- %s : Un carácter de espacio.
- %S : Un carácter separador (espacio o retorno de carro).
- %p : Un signo de puntuación.
- %? : El carácter "?".
- %% : El carácter "%".
- 0, 1, 2, ..., 9 : Cualquier dígito, representado por su código de carácter.
- $cadena : Una cadena de cualquier longitud (igual que "cadena").
- cabeza : La cabeza de otra regla.

Negación: Todos los elementos pueden ser negados con "~" excepto las cabezas.

Disyunción: Use ";" cuando necesite una disyunción entre dos elementos, "," en caso contrario.

Estrella de Kleene: Use "+" o "*" para iterar sobre cada uno de estos elementos.

Coincidencia más larga: Si usa "++" o "**", el bucle consumirá la cadena hasta el elemento más alcanzable.

Opcional: Use "(elemento)" para caracteres o cabezas opcionales.

Todas las reglas deben terminar con un ".".

Cuando el nombre de una cabeza comienza con un "_", la cadena se extrae pero su etiqueta no se almacena.

Casos específicos:
- ?_ : Cualquier carácter, pero no se almacena.
- %a_ : Cualquier carácter alfabético, pero no se almacena.
- %c_ : Cualquier carácter en minúscula, pero no se almacena.
- %C_ : Cualquier carácter en mayúscula, pero no se almacena.
- %d_ : Un dígito, pero no se almacena.
- %e_ : Un emoji, pero no se almacena.
- %H_ : Un carácter Hangul, pero no se almacena.
- %r_ : Un retorno de carro, pero no se almacena.
- %s_ : Un carácter de espacio, pero no se almacena.
- %S_ : Un carácter separador, pero no se almacena.
- %p_ : Un signo de puntuación, pero no se almacena.
- etiqueta_ : Una llamada a una regla sin almacenamiento.

La adición de un "_" al final de estas opciones permite el reconocimiento de un carácter o un grupo de caracteres que no se almacena en el resultado final.

### Ejemplo

```python
# Esta gramática reconoce una palabra o un número, solo para una cadena...
cadena r = @"
bloque := palabra;número.
palabra := %a+.
número := %d+.
"@;

# Cargamos nuestra gramática
gramática g(r);

# La aplicamos a la cadena "the"
mapa m = g.apply("the")
# Salida: {'bloque':[{'palabra':['the']}]}

m = g.apply("123")
# Salida: {'bloque':[{'número':['123']}]}

# Sin embargo, si aplicamos esta gramática a "Test 123", fallará. Necesitamos agregar
# dos cosas a la gramática:
# a) Tener en cuenta los espacios
# b) Iterar para reconocer cada token en la cadena
cadena r = @"
base := _bloque+.
_bloque := palabra;número;%s.
palabra := %a+.
número := %d+.
"@;

# Hemos agregado una nueva disyunción con %s para tener en cuenta los espacios. Luego
# hemos agregado una regla "base" que itera sobre bloque.
m = g.apply("Test 123")
# Salida: {'base':[{'bloque':[{'palabra':['Test']}]},{'bloque':[' ']},{'bloque':[{'número':['123']}]}]}

# Podemos usar el operador "_" para eliminar información innecesaria de la salida, como "bloque".
cadena r = @"
base := bloque+.
bloque := palabra;número;%s.
palabra := %a+.
número := %d+.
"@;

m = g.apply("Test 123")
# Salida: {'base':[{'palabra':['Test']},' ',{'número':['123']}]}
```

## Sub-gramáticas

Las sub-gramáticas se introducen dentro de "[...]". En estos corchetes, es posible definir una disyunción de cadenas de expresiones regulares de caracteres. Estas expresiones son especialmente útiles cuando se aplica una gramática a un vector de cadenas, donde las cadenas se pueden comparar a nivel de caracteres con la expresión en sí. Cada expresión debe separarse de la siguiente con un "|". No se puede llamar a una regla desde dentro de los corchetes, por lo que una cadena como "dog" será equivalente a "$dog".

### Ejemplo

```python
cadena diccionario = @"
prueba := %a, palabra,%a.
palabra := [%C,(" -"),%c+|prueba|ser|perro|gato].
"@;

gramática g(diccionario);
ustring s = "El C -at bebe";
uvector v = s.tokenize();
vector res = g.apply(v);
println(res);
```

## Vector vs Mapa

Si se reemplaza la variable receptora por un vector, la estructura de salida es diferente. El nombre de la regla de la cabeza se inserta en la estructura final como primer elemento. Por ejemplo, si aplicamos la gramática a la misma cadena pero con un vector como salida, obtenemos: `['base',['palabra','Test'],' ',['número','123'],' ',['código','T234e']]`.

## Entrada es una Cadena o un Vector

Si la entrada es una cadena, cada carácter detectado se agrega a la cadena de salida. Sin embargo, si la entrada es un vector de caracteres, el resultado de salida se mantiene como un vector de caracteres.

### Ejemplo

```python
# Esta gramática reconoce una palabra o un número
cadena r = @"
base := _bloque+.
_bloque := código;palabra;número;%s.
palabra := %a+.
número := %d+.
código := %C,%d+,%c.
"@;

# Cargamos nuestra gramática
gramática g(r);

# Dividimos una cadena en un vector de caracteres
cadena s = "Test 123 T234e";
svector vs = s.split("");

# Aplicamos la gramática al vector de caracteres
vector v = g.apply(vs);
println(v);
```

## Función

Es posible asociar una función con una gramática. La firma de la función es la siguiente: `función llamadagramática(cadena cabeza, estructura propia, int pos)`. Esta función se llama para cada nueva estructura calculada para una determinada cabeza. Si esta función devuelve false, entonces el análisis de esa regla falla. `pos` es la última posición en la cadena hasta la cual tuvo lugar el análisis sintáctico.

### Ejemplo

```python
# Esta gramática reconoce una palabra o un número
cadena r = @"
base := _bloque+.
_bloque := código;palabra;número;%s.
palabra := %a+.
número := %d+.
código := %C,%d+,%c.
"@;

# Esta función se llama para cada nueva regla que tiene éxito
función llamadagramática(cadena cabeza, propia v, int ps) {
    println(cabeza,v,ps);
    return true;
}

# Cargamos nuestra gramática
gramática g(r) con llamadagramática;

# Dividimos una cadena en un vector de caracteres
cadena s = "Test 123 T234e";

# Aplicamos la gramática a la cadena de caracteres
mapa m = g.apply(s);
println(m);
```

## Modificación de la Estructura

La estructura también se puede modificar en la función asociada con la gramática, pero se debe tener cuidado al realizar modificaciones.

### Ejemplo

```python
función llamadagramática(cadena cabeza, propia v, int ps) {
    # Si la cabeza es una palabra, modificamos la cadena interna
    si (cabeza == "palabra") {
        println(cabeza,v);
        v[0] += "_unapalabra";
    }
    return true;
}
```

## Desde Dentro de una Regla

También se puede llamar a una función desde dentro de una regla. La firma es la siguiente: `función llamadaregla(propia estructura, int pos)`.

### Ejemplo

```python
# Esta gramática reconoce una palabra o un número
cadena r = @"
base := _bloque+.
_bloque := código;palabra;número;%s.
palabra := %a+.
número := %d+.
código := %C,%d+,%c,llamadacódigo.
"@;

# Cargamos nuestra gramática
gramática g(r);

# Dividimos una cadena en un vector de caracteres
cadena s = "Test 123 T234e";

# Aplicamos la gramática al vector de caracteres
mapa m = g.apply(s);
println(m);
```

## Análisis de HTML

```python
# evaluate es un método básico para reemplazar cada entidad HTML por su contraparte UTF8.
función evaluar(propia s, int p) {
    s[1] = s[1].evaluate();
    return true;
}

# Esta es nuestra gramática HTML
# No mantenemos los caracteres de espacio entre etiquetas, por lo tanto: %s_ en objecto
cadena htmlgrm = @"
html := _objeto+.
_objeto := etiqueta;%s_;texto.
etiqueta := "<",?+,">".
texto := _caracteres,evaluar.
_caracteres := ~"<"+.
"@;

# Compilamos nuestra gramática
gramática ghtml(htmlgrm);

# Aplicamos la gramática a un texto HTML
vector rgram = ghtml.apply(html_text);
```