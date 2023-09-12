## Functions

The Taskell language also provides a way to declare functions. These functions can be declared anywhere and can also be called as Tamgu functions.

### How to declare a Taskell function?

A function is declared in the following way:

```taskell
<name(a1, a2...) = Taskell expression>
```

They can be called from a Tamgu program with: `name(p1, p2...)`.

Example:

```taskell
<one(x) = x + 1>
int val = one(12);
println(val); // val is 13

<plusone(v) = x + 1 | x <- v>
vector vect = plusone([1..10]);
println(vect); // vect is [2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
```

Note that the function argument list declaration is different from the one that is defined in true Haskell.

### Description of Argument Types

You can describe how a function can handle return and argument types with some simple rules.

You declare the argument types and the return type with the name of the function followed by the operator "::".

```taskell
<MyFunc :: int -> int -> int>
```

The last element after the last "->" is the return type. The size of the arguments should match the size of the function.

If one of the arguments must be a function, then the type will be "call". Again, this is a difference with Haskell, where descriptions can be much more refined.

Important: If you provide declarations, then all subsequent MyFunc implementations will inherit this same declaration.

When an argument in the list does not require a specific type, you can replace it with "_".

```taskell
<Test :: int -> _ -> vector ...>
```

In this example, the second argument does not request any specific type.

Example:

```taskell
<Mult :: int -> _ -> float -> float>
<Mult(x, y, z) = 3 * x - y + z>
int j = 17 + Mult(10, 2, 3);
println(j); // result is 48 (first definition of Mult)
```

### Declarations in Taskell Expressions

You can also add return types in Taskell expressions, exactly in the same way as functions, just before the expression itself. However, in that case, the parameter is empty.

```taskell
<:: ivector x | x <- [1..10]> // this expression will return an ivector
```

### Without Declarations

If you declare a Taskell function without a declaration, Tamgu declares each non-atomic element as a "self" variable.

Hence, "plusone" declaration is equivalent to:

```taskell
function plusone(self, v) {...}
```

However, the arguments of these functions can be either atomic values (integer, float, or string) or vector declarations.

### Multiple Declarations

It is possible to declare a function in more than one Taskell expression. In that case, the argument list can contain atomic values. When the expression is evaluated, the parameters are tested against the arguments of the function. If there is no match, then the system tries the next declaration.

Example:

```taskell
<fibonacci(0) = 0>
<fibonacci(1) = 1>
<fibonacci(n) = a | let a = fibonacci(n - 1) + fibonacci(n - 2)>
fibonacci(10); // returns 55
```

When "n" is 0 or 1, it matches against the first or second definition, which then returns the adequate value.

### break

The "break" can be used to "fail" the current function declaration. For instance, you might want to go to the next declaration if the number of elements is more than a certain value.

Example:

```taskell
<myloop(v): if (v.size() > 10) break else v[0]>
<myloop(v): v[10]>
<myloop([1..10])> // yields 1, the list size is 10
<myloop([1..20])> // yields 11, the list size is 20
```

### case x of pattern -> result, pattern -> result... otherwise result

This instruction is very similar to a switch case, but with a big difference. It compares x to patterns and not just values. For instance, you can provide vector patterns in the list as a way to create local variables.

Example:

```taskell
// In this case, we test each value against 1, 2 and we return 12, 24, or 34
vector v = <case x of 1 -> 12, 2 -> 24 otherwise 34 | x <- [1..10]>;
println(v); // v is [12, 24, 34, 34, 34, 34, 34, 34, 34, 34]

// We prepare a vector in which we have: [[1, 2, 3, 4], ..., [1, 2, 3, 4]]
vector v = <replicate 5 [1..4]>;
println(v); // v is [[1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4], [1, 2, 3, 4]]

// We match the sub-lists against vector patterns
vector v = <case x of [a, b] -> (a + b), [a, b, c, 4] -> (a + b - c) otherwise <sum x> | x <- v>;
println(v); // v is [0, 0, 0, 0, 0]
```

### Iteration on lists in the arguments

Taskell can iterate on lists in a very similar way as Prolog. You can use the same operator "|" as in Prolog or you can use the ":" operator as in Haskell to define how the list should be split.

Example:

```taskell
<see([]) = "empty">
<see([first:rest]) = [a, first] | let a = see(rest)>
see(['a'..'e']); // yields [[[[['empty', 'e'], 'd'], 'c'], 'b'], 'a']
```