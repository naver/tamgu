# Functional Language: à la Lisp in Tamgu

Tamgu offers a Lisp interpreter that is very close to the original language definition, with two important exceptions:

- A Lisp expression must be preceded with a backslash (\\).
- The language exposes the basic functions and operators of Lisp, but also allows for the use of Tamgu personal instructions.

## Method

The Lisp interpreter also exposes a new type: lisp, which is a variation on vector. This operator only exposes two methods:

- `eval(string)`: returns the evaluation of a string as a Lisp instruction.
- `load(filename)`: load a file name and evaluate its content.

Note: For these two instructions, the double quote and the backslash are useless.

**IMPORTANT:** It is possible to avoid using the "overloaded" Lisp with double quotes and backslashes in a file, if the two first characters in this file are: `()`. These characters trigger the pure Lisp mode.

```lisp
(defun somme (x y)
  (+ x y)
)

(println (somme 10 20))
(println (somme "a" "b"))
```

## Basic Lisp Operators

The Lisp version in Tamgu exposes the following functions:

1. `apply`: `(apply f (a1 a2 a3...))`
2. `and`: This operator is used in Boolean expressions.
3. `append`: This operator is used to append lists together.
4. `atomp`: Check if an element is an atom.
5. `body`: return the body of a function.
6. `_takewhile`: store the elements until one does not match the condition. `(_takewhile CMP LIST)`: The CMP is either a quoted comparison or a lambda. `(_takewhile '(< 10) '(1 2 3 4 10 11 12))` returns: `(1 2 3 4)`.
7. `while`: Loop: `(while cond e1 e2 e3...)`
8. `zerop`: Check if a value is 0: `(zerop e)`
9. `_zip`: Combine lists together: `(_zip '(a b c) '(e f g))` yields `((a e) (b f) (c g))`.
10. `_zipwith`: Combine lists together with an intermediate operation or lambda: `(_zipwith '+ '(1 2) '(3 4) '(5 6))` yields: `(9 12)`.

## Some Examples

An 'apply' example:

```lisp
(setq f '+)
(apply f '(1 2 3 4))
```

A 'and' example:

```lisp
(and (> i 10) (< i 20))
```

An 'append' example:

```lisp
(append '(a b c) '(e f g)) ; yields (a b c e f g)
```

A 'block' example:

```lisp
(block
  (print 'ok)
  (+ 10 20)
)
```

A 'while' example:

```lisp
(while (< i 10)
  (if (i == 5)
    (break)
  )
)
```

'car, cdr' examples:

```lisp
(car '(a b c)) ; yields a
(cadr '((a b c d) ef)) ; yields (b c d)
```

'cond' example:

```lisp
(cond ((< o 10) (print o)) ((> o 100) (print o)))
```

'cons' example:

```lisp
(cons 'a '(b c)) ; yields (a b c)
```

A function definition:

```lisp
(defun add (x y) (+ x y))
(add 10 20) ; yields 30
```

## Pure Lisp Mode

You can easily add Lisp instructions to your programs, with the constraint of using a backslash in front of the expression.

However, if you want to experiment in a console, these constraints might hamper your playing with the language. We have introduced `lispmode(bool)`, which triggers a specific pure Lisp mode, in which none of these constraints is necessary anymore. To return to the common interpreter, you need to write `(lispmode false)`.

In the case of the Tamgu shell, on Linux and Mac OS, you can trigger this mode by calling: `tamgu –lisp`. Within the shell, you can also use `lispmode` to toggle between the two modes.

The last way to trigger a pure Lisp mode is to add `()` as the two first characters in a file. When a file contains these two characters, the indenter will be able to consider parentheses as expression separators, similar in this spirit to "braces".

Example:

```lisp
()
(defun test(x) (+ x x))
```