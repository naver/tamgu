# Tipos predefinidos

Tamgu proporciona muchos objetos diferentes, cada uno con un conjunto específico de métodos. Cada uno de estos objetos viene con una lista de métodos predefinidos.

## Métodos básicos

Todos los tipos a continuación comparten los mismos métodos básicos:

a) `isa(typename)`: Comprueba si una variable tiene el tipo `typename` (como una cadena).

b) `type()`: Devuelve el tipo de una variable como una cadena.

c) `methods()`: Devuelve la lista de métodos disponibles para una variable según su tipo.

d) `info(string name)`: Devuelve ayuda sobre un método específico.

e) `json()`: Devuelve la representación JSON del objeto, cuando esté disponible.

## Objeto transparente: `self` (o `let`)

`self` es un objeto transparente, similar a una especie de puntero, que no requiere ninguna transformación específica para el parámetro cuando se utiliza en una función.

Nota: La palabra clave `let` se comporta como `self`, con una gran diferencia. La primera variable almacenada en una variable `let` define el tipo de esa variable. En otras palabras, si almacenas una cadena en una variable `let`, entonces esta variable siempre se comportará como una cadena. Puedes modificar este comportamiento con el operador `:=`, que en este caso fuerza la variable `let` a un nuevo tipo.

### Ejemplo

```javascript
function compare(self x, self y) {
    if (x.type() == y.type())
        print("Estos son el mismo tipo de objetos");
}

// Por ejemplo, en este caso, la función compare recibe dos parámetros, cuyos tipos pueden variar. Una declaración self elimina la necesidad de aplicar cualquier conversión específica a los objetos que se pasan a esa función.

string s1, s2;
compare(s1, s2); // Comparamos dos cadenas

myframe i1;
myframe i2;
compare(i1, i2); // Comparamos dos marcos
```

### Ejemplo con `let`

```javascript
let l = "x=";
l += 12;
println(l); // Muestra: x=12

// Forzamos que 'l' sea un entero
l := 1;
l += 12;
println(l); // Muestra: 13
```