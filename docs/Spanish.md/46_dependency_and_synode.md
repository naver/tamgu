# Dependencia y Synode

Las dependencias son un objeto lingüístico fundamental que se ha vuelto esencial en el procesamiento del lenguaje natural moderno. Tamgu ofrece una implementación específica de estas dependencias basada en el motor de predicados. El objetivo de esta implementación es tomar la salida de un analizador de dependencias (como el analizador de Stanford) como entrada y realizar un análisis adicional sobre ella.

Las dependencias se evalúan como predicados que conectan nodos sintácticos. Nuestro sistema proporciona un segundo tipo llamado synode, que representa un nodo en un árbol constituyente.

## Synode

Un synode es un nodo sintáctico definido por un conjunto de características (implementado como un mapss) y su posición en el árbol constituyente. Un synode tiene los siguientes métodos:

1. _initial(map m): Crea un nodo sintáctico con las características dadas.
2. addchild(synode): Agrega un nodo hijo.
3. addnext(synode): Agrega un nodo siguiente.
4. addprevious(synode): Agrega un nodo anterior.
5. after(synode): Devuelve verdadero si el nodo está después del nodo dado bajo el mismo padre.
6. attributes(): Devuelve los atributos de características como un vector.
7. before(synode): Devuelve verdadero si el nodo está antes del nodo dado bajo el mismo padre.
8. child(): Devuelve el primer nodo hijo o verifica si coincide con el parámetro.
9. children(): Devuelve la lista de hijos para un nodo dado o verifica si el nodo es un hijo.
10. definitions(mapss): Establece las definiciones de características válidas para todos los synodes.
11. last(): Devuelve el último nodo hijo o verifica si coincide con el parámetro.
12. nbchildren(): Devuelve el número de hijos directos.
13. next(synode): Devuelve el siguiente nodo o verifica si coincide con el parámetro.
14. parent(): Devuelve el nodo padre o verifica si coincide con el parámetro.
15. previous(synode): Devuelve el nodo anterior o verifica si coincide con el parámetro.
16. precede(synode): Devuelve verdadero si el nodo está antes (en cualquier lugar del árbol).
17. sibling(synode): Verifica si el nodo es un hermano (ya sea una hermana o un descendiente).
18. sisters(synode): Devuelve la lista de nodos hermanos o verifica si el nodo es un nodo hermano.
19. succeed(synode): Devuelve verdadero si el nodo está después (en cualquier lugar del árbol).
20. test(string attribute): Verifica si un atributo forma parte de la estructura de características.
21. values(): Devuelve los valores de características como un vector.

## Creando un Árbol Constituyente

Un árbol constituyente se construye de arriba hacia abajo. Cuando se utiliza la función `addchild`, agrega un nodo hijo debajo del nodo actual, y cada llamada subsiguiente a esta función agrega un nuevo hijo después del nodo hijo anterior.

Ejemplo:

```cpp
// Creamos nuestro nodo NP
synode np({"pos": "np"});

// Luego tres nodos léxicos
synode det({"pos": "det", "surface": "the"});
synode adj({"pos": "adj", "surface": "big"});
synode noun({"pos": "noun", "surface": "dog"});

// Los agregamos debajo de np, uno después del otro
np.addchild(det);
np.addchild(adj);
np.addchild(noun);

// Mostramos los nodos de manera indentada
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 4);
    Display(x.next(), i);
}

Display(np, 0);
```

Resultado:
```
#0['pos': 'np']
    #0['pos': 'det', 'surface': 'the']
    #0['pos': 'adj', 'surface': 'big']
    #0['pos': 'noun', 'surface': 'dog']
```

Observa el `#0`, que indica que el synode no es una variable de dependencia.

## Tipo Dependency

Una dependencia es una relación entre dos synodes. Las dependencias se pueden crear directamente utilizando el tipo `dependency`, que luego se puede almacenar en la base de conocimientos con `assertz`, o con una regla de dependencia. Una dependencia consta de un nombre, un conjunto de características y una lista de argumentos.

Métodos:
1. _initial([nombre, características, arg1, arg2, ...]): Crea una dependencia con un nombre (cadena), un conjunto de características (un mapss) y una lista de argumentos, cada uno de tipo synode.
2. features(): Devuelve las características de la dependencia.
3. name(): Devuelve el nombre de la dependencia.
4. rule(): Devuelve el ID de la regla que creó esta dependencia.

Ejemplo:

```cpp
// Creamos dos nodos léxicos
synode det({"pos": "det", "surface": "the"});
synode noun({"pos": "noun", "surface": "dog"});

dependency d(["DET", {"direct": "+"}, det, noun]);

// Lo agregamos a la base de conocimientos
assertz(d);

println(d);
```

Resultado:
```
DET['direct': '+']({"pos": "det", "surface": "the"}, {"pos": "noun", "surface": "dog"})
```

## Regla de Dependencia

Una regla de dependencia sigue el patrón:

```
If ([^|~] dep[características] (#x[características], #y) y/o dep(#w, #z)...)
    depres(#n, #nn), ..., depres(#n, #nn) / ~ / #x[..], #x[..].
```

En esta regla, `x`, `y`, `w`, `z`, `n`, `nn` son enteros que representan objetos synode. La regla coincide con las dependencias en la base de conocimientos con las que se almacenan en la regla. Si se encuentra una coincidencia, se crean nuevas dependencias utilizando las mismas variables.

La regla puede combinar llamadas de función y predicados con dependencias. El símbolo `^` indica que la dependencia se modificará. Solo se puede modificar una dependencia a la vez en una regla. El símbolo `~` representa la negación. Si se coloca antes de una dependencia, significa que la dependencia no debe estar presente en la base de conocimientos.

Si se reemplaza la salida de la regla con `~`, la regla se aplicará, pero no se crearán dependencias.

Hecho:
La forma más sencilla de agregar una dependencia a la base de conocimientos es insertarla como un hecho:

```
dep[características](#1, #2).
```

## Características

La estructura de características en una regla de dependencia sigue reglas específicas:
- Las comillas son opcionales alrededor de los atributos y valores.
- El símbolo `+` es el valor predeterminado para los atributos con un solo valor.

Operadores:
- `atributo`: Verifica la existencia del atributo.
- `atributo: valor`: Compara el atributo con el valor.
- `atributo: ~`: El atributo no debe tener ningún valor.
- `atributo ~: valor`: El atributo no debe tener el valor especificado.
- `atributo = valor`: Asigna al atributo el valor especificado.
- `atributo = ~`: Elimina el atributo de la estructura de características.
- `atributo -: gram`: Compara el atributo con una TREG (Expresión Regular de Tamgu).
- `atributo ~ -: gram`: Compara el atributo con una TREG, que debería fallar.

Stop:
El predicado `stop` se puede utilizar para detener la evaluación de una gramática. Cuando el motor de evaluación encuentra este predicado, detiene la evaluación de la gramática, omitiendo las reglas restantes.

```cpp
If (stop) ~.
```

_dependencies():
Este método desencadena un análisis de dependencias aplicando reglas a la base de conocimientos.

_setvalidfeatures(mapss características):
Este método establece restricciones en las características válidas que se pueden utilizar para synodes y dependencias. Las características se definen como pares de atributo/valor en un map. Si un atributo puede tomar cualquier valor, como el lema de una palabra, el valor debe ser una cadena vacía. El valor predeterminado es `+`.

Ejemplo:

```cpp
mapss características = {'Obl': '+', 'lemma': '', 'c_person': '+', 'CR4': '+', 'Punct': '+', 'surface': ''};
_setvalidfeatures(características);
```

## Ejemplo

```cpp
// Mostramos los nodos de manera indentada
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 5);
    if (i) // cuando i==0, es la raíz de nuestro árbol, no queremos mostrar sus hermanas
        Display(x.next(), i);
}

// Preparamos nuestro árbol constituyente
synode np1 = {"bar": 2};
synode np2({"bar": 2});
synode vp({"bar": 2});
synode s({"bar": 3});
synode v({"word": "eats", "pers": 3, "pres": "+", "verb": "+"});
synode d1({"word": "the", "det": "+"});
synode n1({"word": "dog", "noun": "+"});
synode d2({"word": "a", "det": "+"});
synode n2({"word": "bone", "noun": "+"});

s.addchild(np1);
s.addchild(vp);
vp.addchild(v, np2);
np1.addchild(d1, n1);
np2.addchild(d2, n2);

// También es posible agregar o modificar características existentes como si un synode fuera un map
vp["pos"] = "verb";
np1["pos"] = "noun";
np2["pos"] = "noun";

// Nuestras dependencias iniciales
subj(v, n1).
obj(v, n2).
det(n1, d1).
det(n2, d2);

// También podemos crearlo de manera diferente
dependency dpe(["mydep", {}, n1, n2]);

// Pero luego tenemos que agregarlo a la base de conocimientos nosotros mismos
assertz(dpe);

// Esta función se llama desde una regla a continuación. El #x se convierte en un objeto synode.
// La función devuelve verdadero para evitar que la regla falle.
function DTree(synode n) {
    Display(n, 0);
    println("---------------------------");
    return true;
}

// Una regla simple que invierte los nodos
If (subj(#1, #2)) inverted(#2, #1).

// Una regla que usa restricciones en los nodos
If (subj(#1[pres, pers: 3], #2) and obj(#1, #3)) arguments(#2, #1, #3).

// Agregamos características a una dependencia
If (^subj(#1, #2)) subj[direct=+](#1, #2).

// Usamos _ para navegar entre todas las dependencias con dos argumentos, con una restricción de que dos nodos son diferentes
If (_(#1, #2) && obj(#1, #3) && #2 != #3) link(#2, #3).

// Usamos variables de dependencia _1 y _2 para evitar crear una dependencia entre los mismos argumentos
If (_1(#1, #2) && obj_2(#1, #3) && _1 != _2) other(#2, #3).

// Marcamos un nodo a través de una regla de dependencia, y también podemos usar restricciones en la estructura
If (subj(#1, #2) and obj(#1, #3) and #2[noun:+, subject=+]) ~.

// También podemos escribir esta regla usando comillas
If (subj(#1, #2) and obj(#1, #3) and #3["object"] = "+") ~.

// En este caso, accedemos al padre del nodo #1 utilizando el predicado p_parent
// Luego llamamos a DTree para mostrarlo... DTree debe devolver verdadero, de lo contrario la regla fallará
// El #3 se transforma automáticamente en un objeto synode cuando se llama a la función
If (det(#1, #2) and p_parent(#1, #3) and DTree(#3)) ~.

// Usamos una TREG como restricción en nuestra regla
If (obj(#1[word -: "e%a+"], #2)) Verb(#1).

// Lanzamos nuestro analizador de dependencias
_dependencies();

// Recopilamos todas las dependencias en la base de conocimientos
vector res = predicatedump();

Display(s, 0);
println("---------------------------");
printjln(res);
```

Resultado:
```
s
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
---------------------------
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
#0['bar': '3']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
#0['bar': '2', 'pos': 'verb']
    #0['word': 'eats', 'pers': '3', 'pres': '+', 'verb': '+']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
other({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
subj['direct': '+']({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"word": "dog", "noun": "+", "subject": "+"})
inverted({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
obj({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
det({"word": "dog", "noun": "+", "subject": "+"}, {"word": "the", "det": "+"})
det({"object": "+", "word": "bone", "noun": "+"}, {"word": "a", "det": "+"})
arguments({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
link({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
Verb({"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
```