# Functional Language: à la Lisp

Tamgu offers a Lisp interpreter that is very close to the original language definition, with two important exceptions:

1. A Lisp expression must be preceded with a backslash (\).
2. The language exposes the basic functions and operators of Lisp, but also allows for the use of Tamgu personal instructions.

## Method

The Lisp interpreter also exposes a new type called `lisp`, which is a variation of a vector. This type has two methods:

- `eval(string)`: returns the evaluation of a string as a Lisp instruction.
- `load(filename)`: loads a file and evaluates its content.

Note: For these two instructions, the double quote and the backslash are unnecessary.

**IMPORTANT**: It is possible to avoid using the "overloaded" Lisp with double quotes and backslashes in a file, if the two first characters in this file are: `()`. These characters trigger the pure Lisp mode.

## Basic Lisp Operators

The Lisp version in Tamgu exposes the following functions:

1. `apply`: applies a function to a list of arguments.
2. `and`: used in Boolean expressions.
3. `append`: appends lists together.
4. `atomp`: checks if an element is an atom.
5. `body`: returns the body of a function.
6. `block`: allows for a block of instructions. The last instruction in the block returns its value.
7. `or`: Boolean expression.
8. `xor`: Boolean expression, can have only two arguments.
9. `break`: breaks from a loop.
10. `car`: returns the first element of a list.
11. `cdr`: returns the rest of the list.
12. `cond`: condition.
13. `cons`: constructs a list from two elements.
14. `consp`: checks if the element is a list.
15. `defun`: defines a function.
16. `_dropwhile`: skips elements until one does not match the condition.
17. `eq`: compares two atoms.
18. `eval`: evaluates a string or a Lisp expression.
19. `_filter`: applies a filter to a list.
20. `_foldl`, `_foldr`: applies an operation or a lambda to a list.
21. `_foldl1`, `_foldr1`: applies an operation or a lambda to a list.
22. `for`: loops in a list.
23. `if`: tests a condition.
24. `key`: returns or sets a value in a container.
25. `keys`: returns an interval of values from within a container.
26. `label`: associates an expression with a label.
27. `lambda`: function applied on v1, v2.
28. `list`: creates a list out of elements.
29. `_map`: applies an operation to a list.
30. `negation`: Boolean expression, negates the argument.
31. `nullp`: checks if the value is null.
32. `numberp`: checks if the value is a number.
33. `quote`: quotes an expression.
34. `return`: returns a value from a loop.
35. `_scanl`, `_scanr`: applies an operation to a list.
36. `_scanl1`, `_scanr1`: applies an operation to a list.
37. `self`: loops in a lambda.
38. `setq`: creates or modifies a variable.
39. `_takewhile`: stores elements until one does not match the condition.
40. `while`: loops.
41. `zerop`: checks if a value is 0.
42. `_zip`: combines lists together.
43. `_zipwith`: combines lists together with an intermediate operation or lambda.

## Examples

Here are some examples of using the Tamgu Lisp interpreter:

```lisp
; An 'apply' example
(setq f '+)
(apply f '(1 2 3 4))

; An 'and' example
(and (> i 10) (< i 20))

; An 'append' example
(append '(a b c) '(e f g))

; A 'block' example
(block
  (print 'ok)
  (+ 10 20)
)

; A 'while' example
(while (< i 10)
  (if (== i 5)
    (break)
  )
)

; 'car', 'cdr' examples
(car '(a b c))
(cadr '((a b c d) ef))

; 'cond' example
(cond
  ((< o 10) (print o))
  ((> o 100) (print o))
)

; 'cons' example
(cons 'a '(b c))

; A function definition
(defun add (x y) (+ x y))
(add 10 20)

; '_filter' example
; The COMPARATOR is either a quoted comparison or a lambda
(vector v = [1..10])
(println (_filter '(< 3) v))
(println (_filter (lambda (x) (< x 3)) v))

; 'for' Loop example
(vector v = [1..10])
(for i v (print i))

; 'if' example
(if (== i 10)
  (print i)
  (+ i 1)
)

; 'key' example
(key m "test")

; 'lambda' example
(int j = ((lambda (x) (+ 1 x)) 2))

; 'list' example
(list 'a 'b '(c d))

; '_map' example
(vector v = [1..5])
(println (_map '* v))
(println (_map '( - 1) v))
(println (_map '(1 -) v))
(println (_map (lambda (x) (* x 2)) v))

; 'self' example in a lambda
(p = ((lambda (x) (if (< x 20) (self (+ x x)) x)) 1))

; '_dropwhile' example
(v = (_dropwhile '(< 10) '(1 2 3 4 10 20 30)))

; '_takewhile' example
(v = (_takewhile '(< 10) '(1 2 3 4 10 20 30)))

; '_foldl' example
(int res = (_foldl '+ 10 '(10 20 30)))

; '_scanl' example
(v = (_scanl 1 (lambda (acc x) (+ acc (* 2 x))) '(10 20 30)))

; '_zip' example
(v = (_zip '(10 20) '(11 21) '(12 22)))

; '_zipwith' example
(v = (_zipwith '+ '(10 20) '(11 21) '(12 22)))
(v = (_zipwith (lambda (x y z) (- x z (+ 1 y))) '(10 20) '(11 21) '(12 22)))
```

## Pure Lisp Mode

You can easily add Lisp instructions to your programs with the constraint of using the backslash (\) in front of the expression. However, if you want to experiment in a console, these constraints might hamper your playing with the language. To address this, we have introduced `lispmode(bool)`, which triggers a specific pure Lisp mode where none of these constraints are necessary. To return to the common interpreter, you need to write `(lispmode false)`.

In the case of the Tamgu shell on Linux and Mac OS, you can trigger this mode by calling `tamgu –lisp`. Within the shell, you can also use `lispmode` to toggle between the two modes.

The last way to trigger a pure Lisp mode is to add `()` as the two first characters in a file. When a file contains these two characters, the indenter will be able to consider parentheses as expression separators, similar in this spirit to "braces".

Example:

```lisp
()
(defun test(x) (+ x x))
```