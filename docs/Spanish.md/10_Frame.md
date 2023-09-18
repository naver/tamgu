# 10 Frame

La clase `Frame` se utiliza para declarar estructuras de datos complejas junto con funciones. Proporciona las siguientes características:

- Las variables miembro se pueden instanciar dentro del frame.
- Se puede declarar un método `_initial`, que se llamará automáticamente para cada nueva instancia de este frame.
- Se puede declarar un sub-frame dentro del cuerpo del frame, que hereda automáticamente los métodos y variables del frame superior.
- Es posible redefinir una función dentro de un sub-frame.
- También se pueden declarar funciones y variables privadas dentro de un frame.

## Ejemplo

```cpp
frame myframe {
    int i = 10; // cada nuevo frame instanciará i con 10
    string s = "inicial";
    
    function display() {
        print("EN MI FRAME: " + s + "\n");
    }
    
    frame mysubframe {
        function display() {
            print("EN MI SUB-FRAME: " + s + "\n");
        }
    }
}
```

## 10.1 Uso de un frame

Se declara un objeto frame con el nombre de su frame como tipo. 

Ejemplo:

```cpp
myframe first; // crea una primera instancia
mysubframe subfirst; // crea una instancia de sub-frame

// Podemos recrear una nueva instancia
first = myframe; // equivalente a "new myframe" en C++ o Java

// Para ejecutar una función del frame
myframe.display();
```

## 10.2 Función `_initial`

Se puede asociar una función creadora con un frame. Esta función se llama automáticamente cuando se crea una nueva instancia de ese frame.

Ejemplo:

```cpp
frame myframe {
    int i = 10; // cada nuevo frame instanciará i con 10
    string s = "inicial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function display() {
        print("EN MI FRAME: " + s + "\n");
    }
}

// Se crea una nueva instancia de myframe
myframe second(10); // los parámetros se pasan a la función _initial como en C++
```

## 10.3 Función `_final`

La función `_final` se llama cada vez que se elimina un objeto frame. Por lo general, un objeto que se declara en una función o en un bucle se elimina una vez que esta función o bucle termina.

Importante:
- Esta función no tiene parámetros.
- La llamada a esa función no elimina el objeto.
- El contenido de esta función no se puede depurar, ya que se llama desde el destructor, independientemente del resto del código.

Ejemplo:

```cpp
frame myframe {
    int i = 10; // cada nuevo frame instanciará i con 10
    string s = "inicial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function _final() {
        print("EN MI FRAME: " + s + "\n");
    }
}

int i = 10;
while (i >= 0) {
    // Se crea una nueva instancia de myframe
    // Al final de cada iteración, se llamará a la función _final
    myframe item(i);
    i--;
}
```

## 10.4 Orden de inicialización

Cuando se declaran elementos dentro de un frame, la llamada a la función `_initial` se realiza de arriba hacia abajo hasta sus hijos. Además, si un elemento dentro de un frame F se instancia dentro de la función `_initial` de ese frame F, entonces esta declaración tiene prioridad sobre cualquier otra declaración.

Ejemplo:

```cpp
// Se declaran dos frames
frame within {
    int i;
    
    // Con una función constructora específica
    function _initial(int j) {
        i = j * 2;
        println("dentro de _initial", i);
    }
}

// Este frame declara un frame específico "within"
frame test {
    int i;
    
    // En este caso, declaramos un frame específico, cuya declaración depende de la variable i
    within w(i);
    
    // Nuestra función _initial para ese frame...
    function _initial(int k) {
        i = k;
        println("test _initial", k);
    }
}

// Creamos una instancia de prueba: t1 con un valor inicial de 20
test t1(20);
```

## 10.5 Creación dentro del constructor

Es posible crear un elemento frame tanto declarando su inicialización directamente en la lista de campos del frame como dentro del propio constructor. Cuando la construcción del elemento frame se realiza en el constructor, basta con una declaración simple; cualquier otra declaración sería redundante.

Ejemplo:

```cpp
// Este frame declara un frame específico "within"
frame test {
    int i;
    
    // En este caso, posponemos la creación real del elemento al constructor: _initial
    within w;
    
    // Nuestra función _initial para ese frame...
    function _initial(int k) {
        i = k;
        // Reemplazamos la descripción anterior por una nueva
        // Esta declaración subsume la otra anterior
        w = within(100);
        println("test _initial", k);
    }
}

// Creamos una instancia de prueba: t1 con un valor inicial de 20
test t1(20);
```

Importante:
Si se requieren parámetros del constructor para "w" y no se realiza ninguna creación de ese elemento "w" en el constructor, entonces Tamgu mostrará un error sobre los parámetros que faltan.

## 10.6 Variables comunes

Tamgu proporciona una forma sencilla de declarar variables de clase. Una variable de clase es una variable cuyo valor se comparte entre todas las instancias de un frame dado.

Ejemplo:

```cpp
frame myframe {
    common int i; // cada frame tendrá acceso a la misma instancia común de esa variable
}

myframe t1;
myframe t2;
t1.i = 10;
t2.i = 15;
println(t1.i, t2.i); // muestra para ambas variables: 15 15
```

## 10.7 Funciones y miembros privados

Se pueden declarar ciertas funciones o variables como privadas en un frame. Una función o una variable privada solo se puede acceder desde dentro del frame.

Ejemplo:

```cpp
frame myframe {
    int i = 10; // cada nuevo frame instanciará i con 10
    
    // variable privada
    private string s = "inicial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    // función privada
    private function modify(int x) {
        i = x;
        s = "Modificado con: " + x; // puedes modificar "s" aquí
    }
    
    function display() {
        modify(1000); // puedes llamar a "modify" aquí
        print("EN MI FRAME: " + s + "\n");
    }
}

myframe test;

// Instrucciones no permitidas en miembros privados del frame...
test.modify(100); // esta instrucción no es válida ya que "modify" es privado
println(test.s); // esta instrucción no es válida ya que "s" es privado
```

## 10.8 Sub-framing o Enriqueciendo un Frame

Tamgu permite al programador enriquecer o sub-framear un frame existente. Se puede implementar una descripción de frame en unos pocos pasos. Por ejemplo, se puede comenzar con una primera descripción y luego decidir enriquecerla más tarde en el programa.

### Enriquecimiento

```cpp
// Comenzamos con una definición limitada de un frame...
frame myframe {
    int i = 10; // cada nuevo frame instanciará i con 10
}

// Añadimos algún código aquí después...

// Luego enriquecemos este frame con algo más de código
// Todo lo que necesitamos es usar la misma instrucción de frame anterior, agregando algo nuevo
frame myframe {
    function display() {
        println(i);
    }
}
```

### Sub-frames

Un sub-frame es un frame que se declara dentro de otro frame (el frame principal), del cual hereda tanto variables como funciones. Un sub-frame puede reemplazar las funciones del frame principal con nuevas versiones y agregar sus propias variables.

```cpp
// Si queremos añadir sub-frames...
frame myframe {
    // Ahora podemos añadir nuestro sub-frame...
    frame subframe { ... }
}
```

### Usando la Definición Superior: frame::function

Si necesita utilizar la definición del frame principal en lugar del sub-frame actual, Tamgu proporciona un mecanismo muy similar a otros lenguajes como C++ o Java. El nombre de la función debe ir precedido del nombre del frame junto con "::".

Ejemplo:

```cpp
// Llamando a subframes...
// Definimos un frame de prueba, en el que definimos un frame de subprueba
frame test {
    int i;
    
    function _initial(int k) {
        i = k;
    }
    
    function display() {
        println("En test", i);
    }
    
    frame subtest {
        string x;
        
        function display() {
            println("En subtest", i);
            test::display(); // llamará a la otra definición de display desde test
        }
    }
}

// Creamos dos objetos
test t(1);
subtest st(2);

// Luego llamamos a los diferentes métodos
t.display(); // muestra: "En test, 1"
st.display(); // muestra: "En subtest, 2" y "En test, 2"
st.test::display(); // muestra: "En test, 2"
```

## 10.9 Funciones del sistema

Las funciones del sistema se utilizan para asignar los diferentes operadores del lenguaje Tamgu a los métodos de un frame.

### Funciones de comparación

Tamgu también proporciona una forma de ayudar a definir funciones de comparación específicas entre diferentes elementos de un frame. Estas funciones tienen un nombre específico, aunque se activarán mediante los siguientes operadores: `>`, `<`, `==`, `!=`, `<=` y `>=`.

Cada función tiene un solo parámetro que se compara con el elemento actual.

A continuación se muestra una lista de estas funciones:

1. `equal`: función ==(frame b);
2. `different`: función !=(frame b);
3. `inferior`: función <(frame b);
4. `superior`: función >(frame b);
5. `inferior equal`: función <=(frame b);
6. `superior equal`: función >=(frame b);

Cada una de estas funciones debe devolver verdadero o falso según su prueba.

Ejemplo:

```cpp
// Implementación de un operador de comparación en un frame
frame comp {
    int k;
    
    // Implementamos el operador inferior
    function <(autre b) {
        if (k < b)
            return true;
        return false;
    }
}

// Creamos dos elementos
comp one;
comp two;

// one es 10 y two es 20
one.k = 10;
two.k = 20;

// one es inferior a two y se llama al método inferior anterior
if (one < two)
    println("OK");
```

### Funciones aritméticas

Tamgu proporciona un mecanismo para implementar funciones específicas para los diferentes operadores numéricos. Estas funciones deben tener dos operadores, excepto para `++` y `--`. Deben devolver un elemento del mismo frame que sus argumentos.

1. `plus`: función +(frame a, frame b);
2. `minus`: función -(frame a, frame b);
3. `multiply`: función *(frame a, frame b);
4. `divide`: función /(frame a, frame b);
5. `power`: función ^^(frame a, frame b);
6. `shift left`: función <<(frame a, frame b);
7. `shift right`: función >>(frame a, frame b);
8. `mod`: función %(frame a, frame b);
9. `or`: función |(frame a, frame b);
10. `xor`: función ^(frame a, frame b);
11. `and`: función &(frame a, frame b);
12. `++`: función ++();
13. `--`: función --();

Ejemplo:

```cpp
frame test {
    int k;
    
    function ++() {
        k++;
    }
    
    // Es importante crear un nuevo valor, que se devuelve por la función
    function +(test a, test b) {
        test res;
        res.k = a.k + b.k;
        return res;
    }
}

test a, b, c;
c = a + b; // Llamaremos a nuestra implementación de plus anterior.
```

### Funciones de número y cadena

También se puede interpretar un objeto frame como una cadena o un número en una expresión aritmética, por ejemplo. En ese caso, se pueden implementar funciones con el nombre del tipo de objeto que se desea que su instancia frame se interprete como.

Ejemplo:

```cpp
frame test {
    int k;
    string s;
    
    // Devolvemos una instancia "test" como una cadena...
    function string() {
        return s;
    }
    
    // Devolvemos una instancia "test" como un flotante...
    function float() {
        return k;
    }
    
    // Devolvemos una instancia "test" como un entero...
    function int() {
        return k;
    }
}
```

### Intervalo e Índice

También es posible usar un objeto frame como un vector o un mapa. Luego, es posible acceder a elementos a través de un intervalo o establecer un valor a través de un índice. Para usar un objeto de esta manera, el desarrollador debe implementar la siguiente función:

1. function [](self idx, self value): Esta función inserta un elemento en un vector en la posición idx.
2. function [](self idx): Esta función devuelve el valor en la posición idx.
3. function [:](self left, self right): Esta función devuelve los valores entre las posiciones left y right.

Ejemplo:

```cpp
frame myvect {
    vector kj;
    
    // Esta función inserta un valor en el vector en la posición idx
    function [](int idx, self value) {
        kj[idx] = value;
    }
    
    // Esta función devuelve el valor en la posición idx
    function [](int idx) {
        return kj[idx];
    }
    
    // Esta función devuelve el valor entre l y r.
    function [:](int l, int r) {
        return kj[l:r];
    }
}

myvect test;
test[0] = 10; // Llamamos a la función [](...)
test[1] = 5; // Llamamos a la función [](...)
// Llamamos a la función [:](...)
println(test[0], test[1], test[0:]); // Muestra: 10 5 [10, 5]
```