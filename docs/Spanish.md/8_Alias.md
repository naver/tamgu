## Alias

Un alias es similar a una función, pero con algunas diferencias específicas:

a) Los parámetros se describen sin ningún tipo.
b) Los argumentos en un alias pueden ser modificados.
c) Un alias puede ser utilizado para reemplazar una llamada a una estructura compleja.
d) Un alias puede ser utilizado como una función simple, en cuyo caso debe devolver su valor con `return`.

### Patrón

Una función alias se implementa de la siguiente manera:

```
alias nombre(a, b, ..., z) = código;
```

### Ejemplo

```python
alias eliminando(a) = a.replace("-", "").replace(r"%p", " ").lower();
alias mostrar(a, b) = {
    println(a, b);
}

a = eliminando(a);
mostrar("Test", a);
```

### Patrones complejos

El siguiente ejemplo muestra cómo se puede crear una función alias que acceda al contenido de un marco:

```python
alias obtener(a, i) = a.personas.nombres[i];
```

Este alias se puede utilizar tanto para recuperar un elemento como para modificarlo. Cuando necesites modificar un elemento, debes usar los siguientes corchetes: `[*...]` para proporcionar los diferentes argumentos.

#### Ejemplo

```python
// Dos descripciones de marcos
marco ListaDePersonas {
    svector nombres;
    función _inicial() {
        nombres = ["a".."e"];
    }
    función cadena() {
        return nombres;
    }
}

marco Elementos {
    ListaDePersonas personas;
    función cadena() {
        return personas;
    }
}

// Definimos un alias para acceder a nombres en ListaDePersonas
alias obtener(a, i) = a.personas.nombres[i];

// Declaramos un elemento de tipo Elementos.
Elementos elementos;

// Accedemos al valor de nombres en la posición 1
println(obtener(elementos, 1));

// Modificamos un valor de nombres en la posición 1
// Observa el uso de [*elementos, 1]
obtener[*elementos, 1] = "B";
```