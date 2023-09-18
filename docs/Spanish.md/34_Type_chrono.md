# Tipo Chrono

El tipo `Chrono` es similar a `Time`, pero está implementado basado en `std::chrono::high_resolution_clock`. Proporciona el mismo método `reset`, pero permite un cálculo de tiempo más preciso.

Al crear un objeto `Chrono`, debes especificar cómo se debe manejar la duración utilizando las siguientes banderas:
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

El valor predeterminado es `c_second`.

## Ejemplo

```cpp
Ring r;
Chrono c1(c_nanosecond);
for (int i = 0; i < 100000; i++) {
    r.pushLast(i);
}
Chrono c2(c_nanosecond);
float d;
d = c2 - c1; // en nanosegundos
println(d);
```

En este ejemplo, se crea un objeto `Ring` llamado `r`, y se crea un objeto `Chrono` llamado `c1` con una duración de `c_nanosecond`. Luego, se ejecuta un bucle para insertar valores en el anillo. Después de eso, se crea otro objeto `Chrono` llamado `c2` con la misma duración. Se calcula la diferencia entre `c2` y `c1` y se almacena en `d`, que representa el tiempo transcurrido en nanosegundos. Finalmente, se imprime el valor de `d`.