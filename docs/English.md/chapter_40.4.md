# Taskell: Declaring Local Variables

In Taskell, there are different ways to declare local variables in an expression.

## Using the `let` operator

The `let` operator is used to associate a variable or a vector pattern with an expression. Here are some examples:

- `<a | let a=10>`
- `<a+b+c | let [a,b,c] = [1,2,3]>`
- `<a+b | let a=10, let b=20>`

If the return value has been declared with `<x | ...>`, multiple `let` expressions can be separated with a `,`.

## Using Types

In the above examples, the `let` declaration allows for dynamic typing, where the expression type is defined by its content. However, you can also use a specific type if needed. For example: `<a+b | int a=10, int b=20>`

## Evaluation

If an iteration is declared in your expression, then the `let` will be reevaluated at each iteration. For example: `<a | x <- [1..10], let a=x*2>`

## Using `in` expressions

It is also possible to return a value through an `in` expression. In this case, the different declarations will share one single `let`. Note that in this case, you cannot use a specific type. For example: `<let a=10, b=20 in a+b>`

## Static Variable

If a `let` variable is declared together with an iteration, then this variable is reinitialized at each iteration. The keyword `static` can be utilized in this case to initialize this variable only once. Here's an example:

```taskell
// We display our string and concatenate it with x
function display(self u, int x) {
    u = u + x;
    println(u);
    return true;
}

// Here tst is static
<statique() = display(tst, i) | i <- [1..3], static string tst="val">

// Here tst is re-initialized at each iteration
<dynamique() = display(tst, i) | i <- [1..3], string tst="val">

statique();
println("------");
dynamique();
```

Output:
```
val1
val12
val123
------
val1
val2
val3
```

## Using the `where` operator

The `where` operator is used to declare global variables. It is placed at the end of a Taskell expression. Its evaluation is always done once before any other analysis. Here's an example: `<a | let a=w+10, where w=20>`

There can be multiple declarations in a `where`, each separated by `;` or `,`. For example: `<a | let a=w1*w2, where w1=20, w2=30>`

## Note

You can also declare functions in a `where`, which will be local to that Taskell expression. Here's an example:

```taskell
<description(l) = ("Liste=" + <what l>) |
    where
        <what([]): "empty">;
        <what([a]): "one">;
        <what(xs): "large">>
```

- `description([])` yields `empty`
- `description([1])` yields `one`
- `description([1,2,3])` yields `large`

## Guard

The Taskell language provides a mechanism similar to a switch/case: the `guard`. A guard is a succession of tests associated with an action, each test is introduced with a `|`. The default value is introduced with the keyword `otherwise`. Here's an example:

```taskell
<imb(bmi) = | bmi <= 10 = "small" | bmi <= 20 = "medium" | otherwise = "large">
```

`imb(12)` yields "medium" as a response.

## Inserting Tamgu code: `{...}`

You can also insert regular Tamgu code in the middle of your Taskell expressions. Simply declare these instructions between `{...}`. For example: `<x | x <- [1..10], {println(x);}>`. In this case, each value of `x` will be displayed while iterating in the value domain.