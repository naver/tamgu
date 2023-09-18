# 36 Tipo de Llamada

Este objeto se utiliza para realizar un seguimiento de las funciones, las cuales luego pueden ser ejecutadas. La llamada se realiza utilizando el nombre de la variable como una función.

## Ejemplo

```c++
int display(int e) {
    print("DISPLAY:", e, "\n");
    e += 10;
    return e;
}

int myfunc;
myfunc = display;
int i = myfunc(100);   // display: DISPLAY:100
print("I=", i, "\n");   // display: I=110
```

En el ejemplo anterior, la función `display` toma un parámetro entero `e`, imprime "DISPLAY:" seguido del valor de `e`, incrementa `e` en 10 y devuelve el valor actualizado de `e`. Luego, la variable `myfunc` se asigna a la función `display`. Finalmente, se llama a la variable `myfunc` con el argumento `100` y el resultado se almacena en la variable `i`. La salida será "DISPLAY:100" e "I=110".