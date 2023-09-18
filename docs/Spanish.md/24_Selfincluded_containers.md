# Contenedores Autoincluidos

En Tamgu, existen dos tipos de contenedores: contenedores de valores y contenedores de objetos.

## Contenedores de Valores
Los contenedores de valores incluyen svector, ivector y mapii. Estos contenedores almacenan valores.

## Contenedores de Objetos
Los contenedores de objetos incluyen vector, map, treemap, table y list. Estos contenedores almacenan objetos.

## Bucles en Contenedores de Objetos
Es posible incluir un contenedor dentro de otro contenedor, incluso incluyendo un contenedor dentro de sí mismo. Sin embargo, cuando un contenedor se incluye a sí mismo, no se puede mostrar la estructura completa. En su lugar, el sistema reemplazará la auto-referencia con "[...]" o "{...}" si es un map.

Ejemplo:

```cpp
vector v = [1..5];
v.push(v);
println(v);
```

Salida: `v es: [1,2,3,4,5,[...]]`, donde "[...]" indica una auto-referencia.

Es importante tener en cuenta que si un contenedor se contiene a sí mismo, puede haber un problema al recorrer la estructura, ya que podría resultar en un bucle infinito.

## Marcado de Métodos
Tamgu proporciona tres métodos para ayudar a lidiar con este problema:

a) `mark(byte v)`: Este método marca un contenedor de objetos.

b) `mark()`: Este método devuelve la marca en un contenedor de objetos.

c) `unmark()`: Este método desmarca todas las marcas dentro de un contenedor de objetos.

Ejemplo:

```cpp
vector v = [10..50:10];
v.push([60..80:10]);
v.push(v);
println(v);

function traversal(self v, int i) {
  if (i == v.size()) {
    println('Fin del vector');
    return;
  }
  if (v[i].mark() == 1) {
    println("Ya analizado");
    return;
  }
  println(i, v[i]);
  v[i].mark(1);
  if (v[i].iscontainer()) {
    println("Este valor es un contenedor...");
    traversal(v[i], 0);
  }
  traversal(v, i + 1);
}

// Marcamos nuestro vector para evitar recorrerlo dos veces
v.mark(1);
traversal(v, 0);

// Desmarcamos todos los elementos a la vez
v.unmark();

// Salida:
// [10,20,30,40,50,[60,70,80],[...]], contiene una auto-referencia
// La ejecución
// 0 10
// 1 20
// 2 30
// 3 40
// 4 50
// 5 [60,70,80]
// Este valor es un contenedor...
// 0 60
// 1 70
// 2 80
// Fin del vector
// Ya analizado
// fin
```