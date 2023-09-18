# 43 Tipo Fibra

Una fibra es un hilo de ejecución ligero. Al igual que los hilos, las fibras comparten el mismo espacio de direcciones. Sin embargo, las fibras utilizan la multitarea cooperativa mientras que los hilos utilizan la multitarea preemptiva. Los hilos a menudo dependen del planificador de hilos del kernel para interrumpir un hilo ocupado y reanudar otro hilo, mientras que las fibras se ceden a sí mismas para permitir que se ejecute otra fibra mientras se ejecutan.

En Tamgu, las fibras se implementan sobre las funciones de Taskell. Estas funciones deben contener una iteración para ejecutarse como fibras, ya que no existe una función de cedida real.

El tipo `fibre` expone los siguientes métodos:

1. `run()`: Ejecuta las fibras que se han registrado.
2. `block()`: Las fibras se almacenan en una cadena enlazada que se itera desde el principio hasta el final. Cuando se agrega una nueva fibra a esta lista, se convierte en el nuevo elemento terminal. `block()` se utiliza para definir el elemento terminal actual de esa lista como el límite de la iteración. Todavía se pueden agregar nuevas fibras, pero no se ejecutarán hasta que se llame a `unblock()`. Si se llama a `block()` nuevamente con un elemento terminal actual diferente, entonces el límite anterior se mueve a este nuevo elemento terminal.
3. `unblock()`: Libera el límite de la iteración.

## Declarar una Fibra

Para declarar una fibra, primero debes implementar una función de Taskell y luego almacenar esta función de Taskell en una variable de fibra.

```cpp
<Mifunc(a1, a2, ..., an) = ...>
fibre f = Mifunc;
```

Para registrar una nueva fibra, simplemente llámala con algunos parámetros:

```cpp
f(p1, p2, ..., pn);
```

Si `Mifunc` no contiene una iteración, se ejecutará automáticamente. Si necesitas almacenar alguna salida, puedes usar el operador de flujo.

```cpp
vector v;
v.push(ivector());
v[-1] <<< f(200, 210);
```

## Finalizar una Fibra: `break` o Fin de la Iteración

Hay dos formas de finalizar una fibra: o el iterador alcanza un final o se llama a la instrucción `break`.

```cpp
<Mifunc(x, y) : if (x == 10) break else (x + 1) ...>
```

## Hilos

Las fibras se pueden ejecutar dentro de un hilo, pero solo una fibra puede ejecutar el método `run()` a la vez en un hilo. Sin embargo, los hilos pueden registrar nuevas fibras en paralelo sin problemas.

## Ejemplo

```cpp
vector v;
int i;

// Inicializamos un vector de enteros para almacenar los resultados...
for (i in <3>)
    v.push(ivector());

//-----------------------------------------------------------------------
function compute(int i, int x, int y) {
    return i + x + y;
}

<mifibra(x, y) = compute(i, x, y) | i <- [x..y]>

// Inicializamos una variable de fibra con mifibra
fibre f = mifibra;

//-----------------------------------------------------------------------
// La grabación se realiza desde un hilo...
joined thread recording(int i, int x, int y) {
    println("Fibra:", i);
    v[i] <<< f(x, y);
}

function running() {
    f.run();
}

//-----------------------------------------------------------------------
// Las fibras se lanzan desde un hilo...
int n;

for (i in <0, 60, 20>) {
    recording(n, i, i + 19);
    n++;
}

// Esperamos a que todas las fibras se graben
waitonjoined();

//-----------------------------------------------------------------------
// Las ejecutamos...
running();

println(v);
```

**Resultado:**

```
Fibra: 0
Fibra: 2
Fibra: 1
[[19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38],
[79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98],
[139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158]]
```