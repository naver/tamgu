# Manejo de errores en Python

En Python, hay varias formas de manejar errores: `try`, `catch`, `raise` y `iferror`. Estas técnicas se utilizan para manejar y gestionar errores en un programa.

## Declaración Try-Catch

Las declaraciones `try` y `catch` se utilizan juntas para capturar y manejar errores en Python. La declaración `catch` puede estar asociada con un parámetro de tipo cadena o entero, el cual se establece automáticamente en `null` cuando se evalúa el bloque `try`. También es posible utilizar una declaración `catch` sin una variable.

```python
s = ""
try:
    # código que puede generar un error
    ...
catch (s)
```

Cuando se detecta un error, el mensaje de error o su número se pasa a la variable especificada.

## El método `raise`

El método `raise` se utiliza para generar un error con un mensaje específico. El mensaje de error siempre debe comenzar con un número de error de tres caracteres, como "000". Este número de error debe ser mayor que 200, ya que los números de error por debajo de 200 están reservados para errores internos de KF. Sin embargo, no se realizará ninguna verificación por parte del lenguaje.

```python
raise("201 Mi error")
```

## La declaración `iferror`

La declaración `iferror` se utiliza para verificar si una instrucción falla. Si la instrucción falla, se ejecuta un bloque de código específico. El mensaje de error está disponible en la variable `_iferror`.

```python
txt = ""
iferror(txt.read(path)):
    print("Error:", _iferror)
```

## Ejemplo

Aquí hay un ejemplo que muestra el uso de las declaraciones `raise` e `iferror`:

```python
raise("201 Mi error")
txt = ""
iferror(txt.read(path)):
    print("Error:", _iferror)
```

En este ejemplo, la declaración `raise` genera un error con el mensaje "201 Mi error". La declaración `iferror` verifica si la instrucción `txt.read(path)` falla. Si lo hace, se ejecuta el bloque de código y se imprime el mensaje de error.