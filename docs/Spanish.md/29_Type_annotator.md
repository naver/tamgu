# Anotador

## Descripción

Un `anotador` es una herramienta de anotación de texto basada en reglas. Te permite definir reglas para etiquetar patrones específicos en un texto. Las reglas del anotador se dividen en cuatro grupos: reglas de léxico, reglas de anotación, reglas globales y reglas de eliminación.

## Sintaxis de las Reglas

Una regla en la herramienta `anotador` es una expresión regular que combina tokens y categorías semánticas. Devuelve una etiqueta que está asociada con el fragmento de texto coincidente en los tokens originales.

El cuerpo de una regla se escribe después del símbolo `<-` o `←`. Los operadores disponibles en una regla son:

- `{t1,t2,t3...}`: Coincide con cualquiera de los tokens listados.
- `?`: Coincide con cualquier token.
- `#etiqueta`: Coincide con una etiqueta semántica producida por una regla o una etiqueta de léxico.
- `#{l1,l2...}`: Coincide si la etiqueta es una de las etiquetas listadas (disyunción).
- `#[l1,l2...]`: Coincide si la etiqueta coincide con todas las etiquetas listadas (conjunción).
- `*`, `+ (:c)`: Operadores de Kleene que pueden estar limitados por un contador.
- `~`: Negación (solo de elementos atómicos).
- `(...)`: Secuencia opcional de tokens.
- `[...]`: Secuencia de tokens.
- `>..<`: Omite los tokens en esta secuencia en la anotación final.
- `<call p1 p2>`: Llama a una función que devuelve verdadero o falso.
- `token`: Coincide con un solo token.
- `'rgx'`: Coincide con una expresión regular basada en la sintaxis de expresiones regulares POSIX.
- `"metas"`: Coincide con una expresión regular específica de Tamgu basada en metacaracteres.

## Métodos del Anotador

El tipo `anotador` expone varios métodos para investigar las anotaciones producidas por las reglas:

- `compile(string rules)`: Compila las reglas almacenadas en la cadena `rules` en la variable `anotador`. Solo esta variable puede acceder a las reglas.
- `select(uvector labels)`: Selecciona las reglas cuyas etiquetas están definidas en `labels`.
- `clear()`: Borra la selección de etiquetas.
- `selection()`: Devuelve la selección de etiquetas.
- `lexicon(transducer t)`: Asocia un léxico transductor de propósito general al `anotador`.
- `compilelexicons()`: Precompila los léxicos. Si no se llama antes del análisis, el primer análisis los compilará, lo que puede introducir retrasos.
- `spans(bool)`: Devuelve tanto las clases de anotación como sus desplazamientos si `true`, o solo las clases de anotación si `false`.
- `parse(ustring txt, bool keeplex)`: Aplica las reglas a una `ustring`.
- `apply(uvector tokens, bool keeplex)`: Aplica las reglas a un vector de tokens.
- `apply(vector morphos, bool keeplex)`: Aplica las reglas a un vector de tokens analizados morfológicamente.
- `apply(annotator a, bool keeplex)`: Aplica las reglas a la estructura calculada por un análisis o aplicación anterior del `anotador`.

## Función de Retorno

Puedes asociar una función de retorno a un objeto `anotador`. Esta función de retorno se llama después de que se haya realizado el análisis pero antes de aplicar las reglas a los textos. La función de retorno tiene la siguiente firma:

```cpp
function catching(vector v, annotator a) {
    v = pos.tagger(v);
    return v;
}
```

La función `catching` se puede utilizar para agregar más análisis a las anotaciones. El vector `v` es una lista de subvectores, donde cada subvector contiene la palabra, su lema, características, desplazamiento izquierdo y desplazamiento derecho. La función debe devolver un vector con la misma estructura.

## Reglas de Léxico

Una regla de léxico se utiliza para describir el vocabulario de un dominio. Asocia palabras con etiquetas específicas. Las reglas de léxico pueden ser palabras simples o expresiones de varias palabras. Las expresiones de varias palabras solo se reconocen como un solo token si se escriben utilizando expresiones regulares de Tamgu.

## Reglas de Anotación

Una regla de anotación es una etiqueta asociada con un cuerpo. El cuerpo puede incluir elementos opcionales, disyunción de elementos, expresiones regulares, palabras omitidas, etc. Devuelve una lista de etiquetas con sus fragmentos en el texto original como desplazamientos.

## Reglas Globales

Las reglas globales comienzan con un `#` o `~` y son similares a las reglas de anotación. Sin embargo, no se aplican a lo largo del texto, sino después de que el texto se haya procesado por completo para manejar las anotaciones. Las reglas globales pueden agregar nuevas anotaciones o eliminar las existentes.

## Ejemplo

```cpp
// Léxico
@positivo  ← genial.
@positivo  ← mejor.
@comida  ← sushi.

// Reglas de anotación
comida ← (#positivo), #comida +.

// Crear anotador
anotador r;

// Compilar reglas
r.compile("@positivo  ← genial.\n@positivo  ← mejor.\n@comida  ← sushi.\ncomida ← (#positivo), #comida +.");

// Aplicar reglas al texto
ustring txt = "El sushi estaba genial.";
r.parse(txt, true);
```

En el ejemplo anterior, se crea el `anotador`, se compilan las reglas y luego se aplican al texto "El sushi estaba genial.". Las anotaciones resultantes se almacenarán en el objeto `anotador`.