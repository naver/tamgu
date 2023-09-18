## Tipo de dato Date

El tipo de dato `Date` se utiliza para manejar fechas.

### Métodos

1. `date()`: Devuelve la fecha como una cadena de texto.
2. `day()`: Devuelve el día como un número entero.
3. `format(string f)`: Devuelve la fecha formateada como una cadena de texto. La cadena de formato utiliza una combinación de opciones. Ver más abajo para una explicación.
4. `hour()`: Devuelve la hora como un número entero.
5. `min()`: Devuelve los minutos como un número entero.
6. `month()`: Devuelve el mes como un número entero.
7. `sec()`: Devuelve los segundos como un número entero.
8. `setdate(year, month, day, hour, min, sec)`: Establece las variables de fecha y hora.
9. `year()`: Devuelve el año como un número entero.
10. `yearday()`: Devuelve el día del año como un número entero entre 0 y 365.
11. `weekday()`: Devuelve el día de la semana como un número entero entre 0 y 6, donde 0 representa el domingo.

### Operadores

- `+` y `-`: Las fechas se pueden sumar o restar.

### Opciones de formato

- `%a`: El nombre abreviado del día de la semana según la configuración regional actual.
- `%A`: El nombre completo del día de la semana según la configuración regional actual.
- `%b`: El nombre abreviado del mes según la configuración regional actual.
- `%B`: El nombre completo del mes según la configuración regional actual.
- `%c`: La representación preferida de fecha y hora para la configuración regional actual.
- `%C`: El número del siglo (año/100) como un número entero de 2 dígitos.
- `%d`: El día del mes como un número decimal (rango de 01 a 31).
- `%D`: Equivalente a `%m/%d/%y`.
- `%e`: Igual que `%d`, el día del mes como un número decimal, pero un cero inicial se reemplaza por un espacio.
- `%F`: Equivalente a `%Y-%m-%d` (el formato de fecha ISO 8601).
- `%G`: El año basado en la semana ISO 8601 con el siglo como un número decimal.
- `%g`: Igual que `%G`, pero sin el siglo, es decir, con un año de 2 dígitos (00-99).
- `%h`: Equivalente a `%b`.
- `%H`: La hora como un número decimal utilizando un reloj de 24 horas (rango de 00 a 23).
- `%I`: La hora como un número decimal utilizando un reloj de 12 horas (rango de 01 a 12).
- `%j`: El día del año como un número decimal (rango de 001 a 366).
- `%k`: La hora (reloj de 24 horas) como un número decimal (rango de 0 a 23); los dígitos individuales van precedidos de un espacio.
- `%l`: La hora (reloj de 12 horas) como un número decimal (rango de 1 a 12); los dígitos individuales van precedidos de un espacio.
- `%m`: El mes como un número decimal (rango de 01 a 12).
- `%M`: Los minutos como un número decimal (rango de 00 a 59).
- `%n`: Un carácter de nueva línea.
- `%p`: "AM" o "PM" según el valor de tiempo dado.
- `%P`: Igual que `%p` pero en minúsculas.
- `%r`: La hora en notación a.m. o p.m.
- `%R`: La hora en notación de 24 horas.
- `%s`: El número de segundos desde la época, 1970-01-01 00:00:00 +0000 (UTC).
- `%S`: Los segundos como un número decimal (rango de 00 a 60).
- `%t`: Un carácter de tabulación.
- `%T`: La hora en notación de 24 horas.
- `%u`: El día de la semana como un número decimal, rango de 1 a 7, siendo el lunes el 1.
- `%U`: El número de semana del año actual como un número decimal, rango de 00 a 53, comenzando con el primer domingo como el primer día de la semana 01.
- `%V`: El número de semana ISO 8601 del año actual como un número decimal, rango de 01 a 53, donde la semana 1 es la primera semana que tiene al menos 4 días en el nuevo año.
- `%w`: El día de la semana como un número decimal, rango de 0 a 6, siendo el domingo el 0.
- `%W`: El número de semana del año actual como un número decimal, rango de 00 a 53, comenzando con el primer lunes como el primer día de la semana 01.
- `%x`: La representación preferida de fecha para la configuración regional actual sin la hora.
- `%X`: La representación preferida de hora para la configuración regional actual sin la fecha.
- `%y`: El año como un número decimal sin un siglo (rango de 00 a 99).
- `%Y`: El año como un número decimal incluyendo el siglo.
- `%z`: La zona horaria numérica (desplazamiento de horas y minutos desde UTC).
- `%Z`: La zona horaria o nombre o abreviatura.
- `%+`: La fecha y hora en formato de fecha.
- `%%`: Un carácter '%' literal.

### Ejemplos

Ejemplo 1:

```python
Date d;
println(d.format("%Y%m%d")); // Muestra la fecha para 2015/12/25 como 20151225
```

Ejemplo 2:

```python
Date mytime;
print(mytime); // Muestra: 2010/07/08 15:19:22
```