# Motor de Inferencia

Tamgu incluye un motor de inferencia que se puede mezclar libremente con instrucciones regulares de Tamgu. Este motor de inferencia es muy similar a Prolog pero con algunas particularidades:

a) Los predicados no necesitan ser declarados de antemano para que Tamgu distinga entre predicados y funciones normales. Sin embargo, si necesitas usar un predicado que se implementará más adelante en el código, debes declararlo de antemano.

b) No es necesario declarar variables de inferencia. Sin embargo, sus nombres son muy diferentes de los nombres tradicionales de Prolog: deben ir precedidos de "?".

c) Cada cláusula de inferencia termina con "." y no con ";".

d) Los términos se pueden declarar de antemano (como variables de término). Sin embargo, si no deseas declararlos, debes preceder su nombre con "?" como en el caso de las variables de inferencia.

e) Se pueden adjuntar probabilidades a los predicados, que se utilizan para elegir como primer camino aquel con las probabilidades más altas.

**Nota**: Para una descripción adecuada del lenguaje Prolog, consulta la documentación correspondiente.

## Tipos

Tamgu expone tres tipos específicos para objetos de inferencia:

### Predicate (Predicado)

Este tipo se utiliza para declarar predicados, que se utilizarán en cláusulas de inferencia. Este tipo expone los siguientes métodos:

1. `name()`: devuelve la etiqueta del predicado
2. `size()`: devuelve el número de argumentos
3. `_trace(bool)`: activa o desactiva el rastreo para este predicado cuando es el predicado de llamada.

### Term (Término)

Este tipo se utiliza para declarar términos, que se utilizarán en cláusulas de inferencia (consulte el ejemplo de NLP a continuación).

### Otros tipos de inferencia: Lista y Mapa Asociativo

Tamgu también proporciona las listas tradicionales a la Prolog, que se pueden usar con el operador "|" para manejar la descomposición de listas (consulte el ejemplo de NLP a continuación para una demostración de este operador).

**Ejemplo:**

```tamgu
predicate alist;
// en esta cláusula, C es el resto de la lista...
alist([?A,?B|?C],[?A,?B],?C) :- true.
v=alist([1,2,3,4,5], ?X,?Y);
println(v); // [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

Tamgu también proporciona un mapa asociativo, que se implementa como un mapa Tamgu, pero en el que el orden de los argumentos es significativo.

**Ejemplo:**

```tamgu
predicate assign,avalue;
avalue(1,1) :- true.
avalue(10,2) :- true.
avalue(100,3) :- true.
avalue("fin",4) :- true.
assign({?X:?Y,?Z:?V}) :- avalue(?X,1), avalue(?Y,2), avalue(?Z,3), avalue(?V,4).
vector v=assign(?X);
println(v); // [assign({'100':'fin','1':10})]
```

Como se puede ver en este ejemplo, tanto las claves como los valores pueden depender de variables de inferencia. Sin embargo, el orden en el que se declaran estas claves:valor de asociación es importante. Por lo tanto, {?X:?Y,?Z:?V} es diferente de {?Z:?V,?X:?Y}.

### PredicateVar (PredicadoVar)

Este tipo se utiliza para manejar predicados y explorar sus nombres y valores. Un PredicateVar se puede ver como una función, cuyos parámetros son accesibles a través de su posición en la lista de argumentos como un vector. Este tipo expone los siguientes métodos:

1. `map()`: devuelve el predicado como un mapa: [nombre:nombre,'0':arg0,'1':arg1,...]
2. `name()`: devuelve el nombre del predicado
3. `query(predicate|nombre,v1,v2,v3)`: construye y evalúa un predicado sobre la marcha
4. `remove()`: elimina el predicado de la memoria
5. `remove(db)`: elimina el predicado de la base de datos db
6. `size()`: devuelve el número de argumentos
7. `store()`: almacena el predicado en la memoria
8. `store(db)`: almacena el valor del predicado en la base de datos db
9. `vector()`: devuelve el predicado como un vector: [nombre,arg0,arg1,...]

Cabe destacar que el método "predicate" (predicado), que existe tanto para un mapa como para un vector, transforma el contenido de un vector o un mapa en un predicado siempre que su contenido imite la salida del predicado de vector() y map().

**Ejemplo:**

```tamgu
vector v=['female','mary'];
predicatevar fem;
fem=v.predicate(); // transformamos nuestro vector en un predicado
fem.store(); // lo almacenamos en la base de datos de hechos
v=fem.query(female,?X); // construimos una consulta de predicado sobre la marcha
v=fem.query(female,'mary'); // construimos una consulta de predicado con una cadena
```

## Cláusulas

Una cláusula se define de la siguiente manera:

```tamgu
predicate(arg1,arg2,...,argn) :- pred(arg...),pred(arg,...), etc. ;
```

Hecho en una base de conocimientos. Un hecho se puede declarar en un programa con la siguiente instrucción:

```tamgu
predicate(val1,val2,...).
```

o

```tamgu
predicate(val1,val2,...) :- true.
```

en realidad es equivalente a la sintaxis anterior.

**loadfacts(pathname)**: carga de una base de conocimientos grande

Si tienes un archivo que almacena una base de conocimientos muy grande, que contiene solo hechos, es más eficiente utilizar `loadfacts` con la ruta del archivo para acelerar la carga. Esta instrucción debe colocarse al principio de tu archivo. Comenzará a cargar en tiempo de compilación.

**Disyunción**

Tamgu también acepta disyunciones en cláusulas con el operador ";", que se puede utilizar en lugar de "," entre predicados.

**Ejemplo:**

```tamgu
predicate madre,padre;
madre("jeanne","marie").
madre("jeanne","rolande").
padre("bertrand","marie").
padre("bertrand","rolande").
predicate parent;
// Luego declaramos nuestra regla como una disyunción...
parent(?X,?Y) :- madre(?X,?Y);padre(?X,?Y).
parent._trace(true);
vector v=parent(?X,?Y);
println(v);
```

**Resultado:**

```
r:0=parent(?X,?Y) --> parent(?X6,?Y7)
e:0=parent(?X8,?Y9) --> madre(?X8,?Y9)
k:1=madre('jeanne','marie').
success:2=parent('jeanne','marie')
k:1=madre('jeanne','rolande').
success:2=parent('jeanne','rolande')
[parent('jeanne','marie'),parent('jeanne','rolande')]
```

**Cortar, Fallar y Detener**

Tamgu también proporciona un corte, que se expresa con el tradicional "!". También proporciona la palabra clave fail, que se puede utilizar para forzar el fallo de una cláusula. Stop detiene toda la evaluación.

**Funciones**

Tamgu también proporciona algunas funciones regulares del lenguaje Prolog, como:

**Función asserta(pred(...))**

Esta función afirma (inserta) un predicado al principio de la base de conocimientos. Ten en cuenta que esta función solo se puede utilizar dentro de una declaración de cláusula.

**assertz(pred(...))**

Esta función afirma (inserta) un predicado al final de la base de conocimientos. Ten en cuenta que esta función solo se puede utilizar dentro de una declaración de cláusula.

**retract(pred(...))**

Esta función elimina un predicado de la base de conocimientos. Ten en cuenta que esta función solo se puede utilizar dentro de una declaración de cláusula.

**retractall(pred)**

Esta función elimina todas las instancias del predicado "pred" de la base de conocimientos. Si se utiliza retractall sin ningún parámetro, entonces limpia toda la base de conocimientos. Ten en cuenta que esta función solo se puede utilizar dentro de una declaración de cláusula.

**Función: predicatedump(pred) o findall(pred)**

Esta función, cuando se utiliza sin ningún parámetro, devuelve todos los predicados almacenados en la memoria como un vector. Si proporcionas el nombre de un predicado como una cadena, entonces se vuelca como un vector todos los predicados con el nombre especificado.

**Ejemplo:**

```tamgu
// Ten en cuenta que necesitas declarar "parent" si quieres usarlo en un assert
predicate parent;
adding(?X,?Y) :- asserta(parent(?X,?Y)).
adding("Pierre","Roland");
println(predicatedump(parent));
```

**Nombre Universal de Predicado**

Cuando se buscan hechos en una base de conocimientos, es posible hacer una consulta sin un nombre de predicado específico. En ese caso, se puede usar: "_" en lugar del nombre del predicado.

**Ejemplo:**

```tamgu
// Nuestra base de conocimientos
father("george","sam").
father("george","andy").
mother("andy","mary").
mother("sam","christine").
// Nuestra regla, coincidirá con cualquier hecho anterior
parent(?A,?B) :- _(?A,?B).
```

También podemos usar algunas variables específicas: _0.._9, que pueden devolver el nombre del predicado con el que hizo coincidencia.

**Ejemplo:**

```tamgu
// Nuestra regla, coincidirá con cualquier hecho anterior
parent(?A,?B,?P) :- _1(?A,?B), ?P is _1.
```

Este código devuelve:

```
parent("andy","mary","mother")
parent("sam","christine","mother")
parent("george","sam","father")
parent("george","andy","father")
```

**Recursividad de Cola**

La recursividad de cola es un mecanismo que transforma una recursión en un proceso iterativo. Para activar el mecanismo de recursividad de cola, debes modificar el nombre del último elemento de la regla agregando un "#". La recursividad de cola se activa si este último elemento coincide con la regla actual.

La recursividad de cola transforma el recorrido de un vector, por ejemplo, en un proceso iterativo. Sin embargo, al final del proceso, se pierde el valor inicial del vector.

**Ejemplo:**

```tamgu
vector v = [1..10];
// Fin de la recursión
traverse([],0).
// El último elemento está marcado para la recursividad de cola
traverse([?X|?Y],?A) :- println(?X,?Y), traverse#(?Y,?A).
vector vv = traverse(v,?A);
println(vv);
```

**Resultado:**

```
1 [2,3,4,5,6,7,8,9,10]
2 [3,4,5,6,7,8,9,10]
3 [4,5,6,7,8,9,10]
4 [5,6,7,8,9,10]
5 [6,7,8,9,10]
6 [7,8,9,10]
7 [8,9,10]
8 [9,10]
9 [10]
10 [] // El primer argumento ahora es []
[traverse([],0)]
```

**Función de Devolución de Llamada**

Un predicado se puede declarar con una función de devolución de llamada, cuya firma es la siguiente:

```tamgu
function OnSuccess(predicatevar p, string s) {
    println(s,p);
    return true;
}
```

El segundo argumento de la función corresponde al parámetro dado a parent en la declaración. Si la función devuelve true, entonces el motor de inferencia intenta otras soluciones; de lo contrario, se detiene.

**Resultado:**

Si ejecutamos nuestro ejemplo anterior, obtenemos:

```
Parent: parent('John','Mary')
Parent: parent('John','Peter')
```

**DCG**

Tamgu también acepta reglas DCG (Gramática de Cláusulas Definidas) con algunas modificaciones a la definición original. Primero, las variables de Prolog deben denotarse con ?V como en las otras reglas. En tercer lugar, los átomos solo se pueden declarar como cadenas.

**Ejemplo:**

```tamgu
predicate sentence,noun_phrase,verb_phrase;
term s,np,vp,d,n,v;
sentence(s(np(d("the"),n("cat")),vp(v("eats"),np(d("a"),n("bat"))))) --> [].
```

**Lanzamiento de una Evaluación**

Las evaluaciones se lanzan exactamente de la misma manera que se lanzaría una función. Por supuesto, puedes proporcionar tantas variables de inferencia como desees, pero solo puedes lanzar un predicado a la vez, lo que impone que tu expresión primero debe declararse como una cláusula si deseas que incluya más de un predicado.

**Importante**: Si la variable receptora es un vector, se proporcionarán todos los análisis posibles. Se recorrerá completamente el árbol de evaluación. Si la variable receptora es cualquier otra cosa, entonces cuando se encuentre una solución, se detendrá la evaluación.

**Mapeo de Métodos a Predicados**

La mayoría de los métodos de objetos se asignan a predicados de una manera muy sencilla. Por ejemplo, si una cadena exporta el método "trim", entonces se crea un "p_trim" con dos variables. Cada método se asigna a un predicado de esta manera. Para cada método, se agrega un prefijo: "p_" para transformar este método en un predicado. El primer argumento de este predicado es el objeto principal del método, mientras que el último parámetro es el resultado de aplicar este método a ese objeto. Por lo tanto, si s es una cadena, s.trim() se convierte en p_trim(s,?X), donde ?X es el resultado de aplicar trim a s. Si ?X se unifica, entonces el predicado comprobará si ?X es igual a s.trim().

**Ejemplo:**

```tamgu
compute(?X,?Y) :- p_log(?X,?Y).
between(?X,?B,?E), succ(?X,?Y).
```

**Errores Comunes con las Variables de Tamgu**

Si usas variables comunes en predicados, como una cadena s, una entero s, o cualquier otro tipo de variables, debes recordar que estas variables se utilizan en predicados como valores de comparación. Un ejemplo puede aclarar un poco lo que queremos decir.

**Ejemplo 1**

```tamgu
string s="test";
string sx="other";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

Ejecución:

```
r:0=comp(s,3) --> comp(other,?X172) --> Fail
```

Esta cláusula ha fallado porque s y sx tienen valores diferentes.

**Ejemplo 2**

```tamgu
string s="test"; // ahora tienen los mismos valores
string sx="test";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

Ejecución:

```
r:0=comp(s,3) --> comp(test,?X173)
e:0=comp(test,3) --> println(s)test
success:1=comp('test',3)
```

Ten cuidado al diseñar tus cláusulas para usar variables externas como fuentes de comparación y no como instanciación. Si quieres pasar un valor de cadena a tu predicado, entonces el marcador de posición para esa cadena debe ser una variable de predicado.

**Ejemplo 3**

```tamgu
string sx="test";
predicate comp;
comp._trace(true);
comp(?s,3) :- println(?s).
comp(sx,?X);
```

Ejecución:

```
r:0=comp(?s,3) --> comp(test,?X176)
e:0=comp('test',3) --> println(?s177:test)test
success:1=comp('test',3)
```

**Ejemplos**

**Torre de Hanoi**

El siguiente programa resuelve el problema de la torre de Hanoi por ti.

```tamgu
predicate move;
move(1,?X,?Y,_) :- println('Mueve el disco superior de ',?X,' a ',?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
predicatevar res;
res=move(3,"izquierda","derecha","centro");
println(res);
```

**Ancestro**

Con este programa, puedes encontrar el ancestro común femenino entre las relaciones parentales de diferentes personas.

```tamgu
predicate ancestor,parent,male,female,test;
ancestor(?X,?X) :- true.
ancestor(?X,?Z) :- parent(?X,?Y),ancestor(?Y,?Z).
parent("george","sam") :- true.
parent("george","andy") :- true.
parent("andy","mary") :- true.
male("george") :- true.
male("sam") :- true.
male("andy") :- true.
female("mary") :- true.
test(?X,?Q) :- ancestor(?X,?Q), female(?Q).
test._trace(true);
vector v=test("george",?Z);
println(v);
```

**Un Ejemplo de NLP**

Este ejemplo corresponde a las cláusulas que se han generado a partir de la gramática DCG anterior dada como ejemplo.

```tamgu
predicate sentence,noun_phrase,det,noun,verb_phrase,verb;
term P,SN,SV,dét,nom,verbe;
sentence(?S1,?S3,P(?A,?B)) :- noun_phrase(?S1,?S2,?A), verb_phrase(?S2,?S3,?B).
noun_phrase(?S1,?S3,SN(?A,?B)) :- det(?S1,?S2,?A), noun(?S2,?S3,?B).
verb_phrase(?S1,?S3,SV(?A,?B)) :- verb(?S1,?S2,?A), noun_phrase(?S2,?S3,?B).
det(["the"|?X],?X,dét("the")) :- true.
det(["a"|?X],?X,dét("a")) :- true.
noun(["cat"|?X],?X,nom("cat")) :- true.
noun(["dog"|?X],?X,nom("dog")) :- true.
verb(["eats"|?X],?X,verbe("eats")) :- true.
vector v;
v=sentence(?X,[],?A);
println("Todas las oraciones que se pueden generar:",v);
// analizamos una oración
v=sentence(["the","dog","eats","a","bat"],[],?A);
println("El análisis:",v);
```

**Torre de Hanoi Animada**

El código a continuación muestra una animación en la que los discos se mueven de una columna a otra. Combina gráficos y predicados.

```tamgu
predicate move;
map columns={'left':[70,50,30],'centre':[],'right':[]};
function disk(window w,int x,int y,int sz,int position) {
    int start=x+100-sz;
    int level=y-50*position;
    w.rectanglefill(start,level,sz*2+20,30,FL_BLUE);
}
function displaying(window w,self o) {
    w.drawcolor(FL_BLACK);
    w.font(FL_HELVETICA,40);
    w.drawtext("Izquierda",180,200);
    w.drawtext("Centro",460,200);
    w.drawtext("Derecha",760,200);
    w.rectanglefill(200,300,20,460,FL_BLACK);
    w.rectanglefill(100,740,220,20,FL_BLACK);
    w.rectanglefill(500,300,20,460,FL_BLACK);
    w.rectanglefill(400,740,220,20,FL_BLACK);
    w.rectanglefill(800,300,20,460,FL_BLACK);
    w.rectanglefill(700,740,220,20,FL_BLACK);
    vector left=columns['left'];
    vector centre=columns['centre'];
    vector right=columns['right'];
    int i;
    for (i=0;i<left;i++)
        disk(w,100,740,left[i],i+1);
    for (i=0;i<centre;i++)
        disk(w,400,740,centre[i],i+1);
    for (i=0;i<right;i++)
        disk(w,700,740,right[i],i+1);
}
window w with displaying;
function moving(string x,string y) {
    columns[y].push(columns[x][-1]);
    columns[x].pop();
    w.redraw();
    pause(0.5);
    return true;
}
move(1,?X,?Y,_) :- moving(?X,?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
thread hanoi() {
    move(3,"izquierda","derecha","centro");
}
function launch(button b,self o) {
    hanoi();
}
button b with launch;
w.begin(50,50,1000,800,"HANOI");
b.create(20,20,60,30,FL_Regular,FL_NORMAL_BUTTON,"Iniciar");
w.end();
w.run();
```