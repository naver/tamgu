# Lenguaje funcional: à la Lisp

Tamgu ofrece un intérprete de Lisp que es muy similar a la definición original del lenguaje, con dos excepciones importantes:

1. Una expresión Lisp debe ir precedida de una barra invertida (\).
2. El lenguaje expone las funciones y operadores básicos de Lisp, pero también permite el uso de instrucciones personales de Tamgu.

## Método

El intérprete de Lisp también expone un nuevo tipo llamado `lisp`, que es una variación de un vector. Este tipo tiene dos métodos:

- `eval(cadena)`: devuelve la evaluación de una cadena como una instrucción Lisp.
- `load(nombre_archivo)`: carga un archivo y evalúa su contenido.

Nota: Para estas dos instrucciones, las comillas dobles y la barra invertida no son necesarias.

**IMPORTANTE**: Es posible evitar el uso de Lisp "sobrecargado" con comillas dobles y barras invertidas en un archivo, si los dos primeros caracteres en este archivo son: `()`. Estos caracteres activan el modo Lisp puro.

## Operadores básicos de Lisp

La versión de Lisp en Tamgu expone las siguientes funciones:

1. `apply`: aplica una función a una lista de argumentos.
2. `and`: se utiliza en expresiones booleanas.
3. `append`: concatena listas.
4. `atomp`: verifica si un elemento es un átomo.
5. `body`: devuelve el cuerpo de una función.
6. `block`: permite un bloque de instrucciones. La última instrucción en el bloque devuelve su valor.
7. `or`: expresión booleana.
8. `xor`: expresión booleana, solo puede tener dos argumentos.
9. `break`: sale de un bucle.
10. `car`: devuelve el primer elemento de una lista.
11. `cdr`: devuelve el resto de la lista.
12. `cond`: condición.
13. `cons`: construye una lista a partir de dos elementos.
14. `consp`: verifica si el elemento es una lista.
15. `defun`: define una función.
16. `_dropwhile`: omite elementos hasta que uno no coincida con la condición.
17. `eq`: compara dos átomos.
18. `eval`: evalúa una cadena o una expresión Lisp.
19. `_filter`: aplica un filtro a una lista.
20. `_foldl`, `_foldr`: aplica una operación o una función lambda a una lista.
21. `_foldl1`, `_foldr1`: aplica una operación o una función lambda a una lista.
22. `for`: bucles en una lista.
23. `if`: prueba una condición.
24. `key`: devuelve o establece un valor en un contenedor.
25. `keys`: devuelve un intervalo de valores dentro de un contenedor.
26. `label`: asocia una expresión con una etiqueta.
27. `lambda`: función aplicada a v1, v2.
28. `list`: crea una lista a partir de elementos.
29. `_map`: aplica una operación a una lista.
30. `negation`: expresión booleana, niega el argumento.
31. `nullp`: verifica si el valor es nulo.
32. `numberp`: verifica si el valor es un número.
33. `quote`: cita una expresión.
34. `return`: devuelve un valor de un bucle.
35. `_scanl`, `_scanr`: aplica una operación a una lista.
36. `_scanl1`, `_scanr1`: aplica una operación a una lista.
37. `self`: bucles en una función lambda.
38. `setq`: crea o modifica una variable.
39. `_takewhile`: almacena elementos hasta que uno no coincida con la condición.
40. `while`: bucles.
41. `zerop`: verifica si un valor es 0.
42. `_zip`: combina listas.
43. `_zipwith`: combina listas con una operación intermedia o una función lambda.

## Ejemplos

Aquí hay algunos ejemplos de cómo usar el intérprete de Lisp de Tamgu:

```lisp
; Un ejemplo de 'apply'
(setq f '+)
(apply f '(1 2 3 4))

; Un ejemplo de 'and'
(and (> i 10) (< i 20))

; Un ejemplo de 'append'
(append '(a b c) '(e f g))

; Un ejemplo de 'block'
(block
  (print 'ok)
  (+ 10 20)
)

; Un ejemplo de 'while'
(while (< i 10)
  (if (== i 5)
    (break)
  )
)

; Ejemplos de 'car' y 'cdr'
(car '(a b c))
(cadr '((a b c d) ef))

; Ejemplo de 'cond'
(cond
  ((< o 10) (print o))
  ((> o 100) (print o))
)

; Ejemplo de 'cons'
(cons 'a '(b c))

; Definición de una función
(defun add (x y) (+ x y))
(add 10 20)

; Ejemplo de '_filter'
; COMPARATOR puede ser una comparación citada o una función lambda
(vector v = [1..10])
(println (_filter '(< 3) v))
(println (_filter (lambda (x) (< x 3)) v))

; Ejemplo de bucle 'for'
(vector v = [1..10])
(for i v (print i))

; Ejemplo de 'if'
(if (== i 10)
  (print i)
  (+ i 1)
)

; Ejemplo de 'key'
(key m "test")

; Ejemplo de 'lambda'
(int j = ((lambda (x) (+ 1 x)) 2))

; Ejemplo de 'list'
(list 'a 'b '(c d))

; Ejemplo de '_map'
(vector v = [1..5])
(println (_map '* v))
(println (_map '( - 1) v))
(println (_map '(1 -) v))
(println (_map (lambda (x) (* x 2)) v))

; Ejemplo de 'self' en una función lambda
(p = ((lambda (x) (if (< x 20) (self (+ x x)) x)) 1))

; Ejemplo de '_dropwhile'
(v = (_dropwhile '(< 10) '(1 2 3 4 10 20 30)))

; Ejemplo de '_takewhile'
(v = (_takewhile '(< 10) '(1 2 3 4 10 20 30)))

; Ejemplo de '_foldl'
(int res = (_foldl '+ 10 '(10 20 30)))

; Ejemplo de '_scanl'
(v = (_scanl 1 (lambda (acc x) (+ acc (* 2 x))) '(10 20 30)))

; Ejemplo de '_zip'
(v = (_zip '(10 20) '(11 21) '(12 22)))

; Ejemplo de '_zipwith'
(v = (_zipwith '+ '(10 20) '(11 21) '(12 22)))
(v = (_zipwith (lambda (x y z) (- x z (+ 1 y))) '(10 20) '(11 21) '(12 22)))
```

## Modo Lisp puro

Puede agregar fácilmente instrucciones Lisp a sus programas con la restricción de usar la barra invertida (\) delante de la expresión. Sin embargo, si desea experimentar en una consola, estas restricciones podrían dificultar su juego con el lenguaje. Para solucionar esto, hemos introducido `lispmode(bool)`, que activa un modo Lisp puro específico donde ninguna de estas restricciones es necesaria. Para volver al intérprete común, debe escribir `(lispmode false)`.

En el caso de la consola de Tamgu en Linux y Mac OS, puede activar este modo llamando a `tamgu –lisp`. Dentro de la consola, también puede usar `lispmode` para alternar entre los dos modos.

La última forma de activar el modo Lisp puro es agregar `()` como los dos primeros caracteres en un archivo. Cuando un archivo contiene estos dos caracteres, el formateador podrá considerar los paréntesis como separadores de expresiones, similar en este espíritu a "llaves".

Ejemplo:

```lisp
()
(defun test(x) (+ x x))
```