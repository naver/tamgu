42.6 Functions in a Taskell Expression

You can call any function or method, either in Taskell or Tamgu, in a Taskell expression. For instance, let's implement a simple "trim" on strings:

```taskell
<trim1 (w) = x | let x = w.trim()> //the simplest one
<trim2 (w) = x | let x = <trim w>> //pure Taskell call
//We define a Tamgu function
function Trim(string c) {
    return c.trim();
}
<trim3 (w) = x | let x = Trim(w)> //Through an external function
<trim4 (w) = x | let x = <Trim w>> //The same, but with a Taskell flavor
```

Note that any method or function can be called from within a Taskell expression as long as it matches the element type.

```taskell
<adding (v) = <sum v>>;  
```

There is no actual difference between calling a function in a Tamgu way or using a Taskell expression.

Example: Sorting a list

```taskell
<fastsort ([])= []>  //if the list is empty, we return an empty "list"
<fastsort ([fv:v])= (mn &&& fv &&& mx) | //we merge the different sublists...
    let mn = fastsort (<a | a < - v, a<=fv>), //we apply our "sort" on the list that contains the elements smaller than fv (First Value)
    let mx = fastsort (<a | a < - v, a >fv>)> //we apply our "sort" on the list that contains the elements larger than fv
```

Haskell functions

We have departed from the actual function declaration in Haskell, where arguments are separated with a space. The main reason is the fact that "(...)" offers a better visual clue to detect these functions in a multi-paradigm language such as Tamgu.

A regular Haskell function should be declared as: `adding x y = x + y`.

Handling functions

Taskell is a functional language, which means, among other things, that Taskell is able to take functions as input, but can also return functions as results.

Functions or methods as arguments

Taskell functions accept other functions as arguments. In that case, the type is "call". In this specific implementation, we do not allow for a refined description of this call as it is possible with actual Taskell implementation.

```taskell
<Invert :: call -> string -> string>
<Invert (f, x) = <f x>>
println(Invert(reverse, "test")); //return tset
```

For instance, the above function accepts a call as an argument and applies this function on x.

Functions as results

Taskell functions can also return functions as results.

```taskell
<fonction :: call>
<renvoie :: _ -> _ -> int>
```