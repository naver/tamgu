# Langage fonctionnel : à la Lisp

Tamgu propose un interpréteur Lisp qui est très proche de la définition originale du langage, avec deux exceptions importantes :

1. Une expression Lisp doit être précédée d'un antislash (\).
2. Le langage expose les fonctions et opérateurs de base de Lisp, mais permet également l'utilisation d'instructions personnelles Tamgu.

## Méthode

L'interpréteur Lisp expose également un nouveau type appelé `lisp`, qui est une variation d'un vecteur. Ce type possède deux méthodes :

- `eval(chaine)`: renvoie l'évaluation d'une chaîne en tant qu'instruction Lisp.
- `load(nomfichier)`: charge un fichier et évalue son contenu.

Note : Pour ces deux instructions, les guillemets doubles et l'antislash ne sont pas nécessaires.

**IMPORTANT** : Il est possible d'éviter d'utiliser le Lisp "surchargé" avec des guillemets doubles et des antislash dans un fichier, si les deux premiers caractères de ce fichier sont : `()`. Ces caractères déclenchent le mode Lisp pur.

## Opérateurs de base de Lisp

La version Lisp dans Tamgu expose les fonctions suivantes :

1. `apply` : applique une fonction à une liste d'arguments.
2. `and` : utilisé dans les expressions booléennes.
3. `append` : concatène des listes.
4. `atomp` : vérifie si un élément est un atome.
5. `body` : renvoie le corps d'une fonction.
6. `block` : permet un bloc d'instructions. La dernière instruction du bloc renvoie sa valeur.
7. `or` : expression booléenne.
8. `xor` : expression booléenne, ne peut avoir que deux arguments.
9. `break` : sort d'une boucle.
10. `car` : renvoie le premier élément d'une liste.
11. `cdr` : renvoie le reste de la liste.
12. `cond` : condition.
13. `cons` : construit une liste à partir de deux éléments.
14. `consp` : vérifie si l'élément est une liste.
15. `defun` : définit une fonction.
16. `_dropwhile` : saute les éléments jusqu'à ce qu'un élément ne corresponde pas à la condition.
17. `eq` : compare deux atomes.
18. `eval` : évalue une chaîne ou une expression Lisp.
19. `_filter` : applique un filtre à une liste.
20. `_foldl`, `_foldr` : applique une opération ou une fonction lambda à une liste.
21. `_foldl1`, `_foldr1` : applique une opération ou une fonction lambda à une liste.
22. `for` : boucle dans une liste.
23. `if` : teste une condition.
24. `key` : renvoie ou définit une valeur dans un conteneur.
25. `keys` : renvoie un intervalle de valeurs à partir d'un conteneur.
26. `label` : associe une expression à une étiquette.
27. `lambda` : fonction appliquée à v1, v2.
28. `list` : crée une liste à partir d'éléments.
29. `_map` : applique une opération à une liste.
30. `negation` : expression booléenne, négation de l'argument.
31. `nullp` : vérifie si la valeur est nulle.
32. `numberp` : vérifie si la valeur est un nombre.
33. `quote` : met une expression entre guillemets.
34. `return` : renvoie une valeur d'une boucle.
35. `_scanl`, `_scanr` : applique une opération à une liste.
36. `_scanl1`, `_scanr1` : applique une opération à une liste.
37. `self` : boucle dans une fonction lambda.
38. `setq` : crée ou modifie une variable.
39. `_takewhile` : stocke les éléments jusqu'à ce qu'un élément ne corresponde pas à la condition.
40. `while` : boucle.
41. `zerop` : vérifie si une valeur est égale à 0.
42. `_zip` : combine des listes.
43. `_zipwith` : combine des listes avec une opération intermédiaire ou une fonction lambda.

## Exemples

Voici quelques exemples d'utilisation de l'interpréteur Lisp de Tamgu :

```lisp
; Exemple d'utilisation de 'apply'
(setq f '+)
(apply f '(1 2 3 4))

; Exemple d'utilisation de 'and'
(and (> i 10) (< i 20))

; Exemple d'utilisation de 'append'
(append '(a b c) '(e f g))

; Exemple d'utilisation de 'block'
(block
  (print 'ok)
  (+ 10 20)
)

; Exemple d'utilisation de 'while'
(while (< i 10)
  (if (== i 5)
    (break)
  )
)

; Exemples d'utilisation de 'car' et 'cdr'
(car '(a b c))
(cadr '((a b c d) ef))

; Exemple d'utilisation de 'cond'
(cond
  ((< o 10) (print o))
  ((> o 100) (print o))
)

; Exemple d'utilisation de 'cons'
(cons 'a '(b c))

; Exemple de définition de fonction
(defun add (x y) (+ x y))
(add 10 20)

; Exemple d'utilisation de '_filter'
; Le COMPARATEUR est soit une comparaison entre guillemets, soit une fonction lambda
(vector v = [1..10])
(println (_filter '(< 3) v))
(println (_filter (lambda (x) (< x 3)) v))

; Exemple de boucle 'for'
(vector v = [1..10])
(for i v (print i))

; Exemple d'utilisation de 'if'
(if (== i 10)
  (print i)
  (+ i 1)
)

; Exemple d'utilisation de 'key'
(key m "test")

; Exemple d'utilisation de 'lambda'
(int j = ((lambda (x) (+ 1 x)) 2))

; Exemple d'utilisation de 'list'
(list 'a 'b '(c d))

; Exemple d'utilisation de '_map'
(vector v = [1..5])
(println (_map '* v))
(println (_map '( - 1) v))
(println (_map '(1 -) v))
(println (_map (lambda (x) (* x 2)) v))

; Exemple d'utilisation de 'self' dans une fonction lambda
(p = ((lambda (x) (if (< x 20) (self (+ x x)) x)) 1))

; Exemple d'utilisation de '_dropwhile'
(v = (_dropwhile '(< 10) '(1 2 3 4 10 20 30)))

; Exemple d'utilisation de '_takewhile'
(v = (_takewhile '(< 10) '(1 2 3 4 10 20 30)))

; Exemple d'utilisation de '_foldl'
(int res = (_foldl '+ 10 '(10 20 30)))

; Exemple d'utilisation de '_scanl'
(v = (_scanl 1 (lambda (acc x) (+ acc (* 2 x))) '(10 20 30)))

; Exemple d'utilisation de '_zip'
(v = (_zip '(10 20) '(11 21) '(12 22)))

; Exemple d'utilisation de '_zipwith'
(v = (_zipwith '+ '(10 20) '(11 21) '(12 22)))
(v = (_zipwith (lambda (x y z) (- x z (+ 1 y))) '(10 20) '(11 21) '(12 22)))
```

## Mode Lisp pur

Vous pouvez facilement ajouter des instructions Lisp à vos programmes en respectant la contrainte d'utiliser l'antislash (\) devant l'expression. Cependant, si vous souhaitez expérimenter dans une console, ces contraintes peuvent entraver votre exploration du langage. Pour remédier à cela, nous avons introduit `lispmode(bool)`, qui déclenche un mode Lisp pur spécifique où aucune de ces contraintes n'est nécessaire. Pour revenir à l'interpréteur classique, vous devez écrire `(lispmode false)`.

Dans le cas du shell Tamgu sur Linux et Mac OS, vous pouvez déclencher ce mode en appelant `tamgu –lisp`. Dans le shell, vous pouvez également utiliser `lispmode` pour basculer entre les deux modes.

La dernière façon de déclencher un mode Lisp pur est d'ajouter `()` comme les deux premiers caractères dans un fichier. Lorsqu'un fichier contient ces deux caractères, l'indenteur pourra considérer les parenthèses comme des séparateurs d'expressions, dans le même esprit que les "accolades".

Exemple :

```lisp
()
(defun test(x) (+ x x))
```