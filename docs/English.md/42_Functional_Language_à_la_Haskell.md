# Taskell

Taskell is a functional language inspired by Haskell. It provides capabilities similar to Haskell, but with a specific focus on a selected range of functions. While Taskell does not behave as a full Haskell compiler, it incorporates some interesting aspects of the Haskell language.

## Before Starting: Some New Operators

Before diving into the language, let's introduce some specific operators that enrich the Taskell world.

### Range Declarations: [a..b]

To comply with the Haskell language, Taskell has added a new way to declare a range of elements using the ".." operator. For example, `[1..10]` defines the vector `[1,2,3,4,5,6,7,8,9,10]`.

### Step

By default, the step is 1, but it is possible to set a different step. You can define the step directly with a ":" at the end of the expression. For example, `[1..10:2]` defines the vector `[1,3,5,7,9]`. You can also define the step by providing the next element in the definition. For example, `[1,3..10]` defines the vector `[1,3,5,7,9]`. It also works with characters. For example, `[‘a’,’c’..’g’]` defines the vector `['a','c','e','g']`. The same vector could also be defined with `[‘a’..’g’:2]`.

### Infinite Ranges

Taskell also provides a notion of infinite range of elements. There are two cases: you can either ignore the first element of the set or the last element. `[1..]` defines an infinite vector that starts at 1, forward: `[1,2,3,4…`. `[..1]` defines an infinite vector that starts at 1, backward: `[1,0,-1,-2,-3…`. You can also use different steps. `[1..:2]` defines an infinite vector that starts at 1, forward: `[1,3,5…`. `[..1:2]` defines an infinite vector that starts at 1, backward: `[1,-1,-3…`. `[1,3..]` defines an infinite vector that starts at 1, forward: `[1,3,5…`. `[..-1,1]` defines an infinite vector that starts at 1, backward: `[1,-1,-3…`.

### Three New Operators: &&&, |||, and ::

These three operators are used to concatenate a list of elements together or to add an element to a vector.

#### Merge: &&&

This operator is used to merge different elements into a vector. If one of the elements is not a list, it is simply merged into the current list. For example, `vector v= 7 &&& 8 &&& [1,2]; println (v);` would result in `[7,8,1,2]`. This operator is similar to "++" in Haskell. Since this operator was already defined in Tamgu, it has been modified to "&&&".

#### Combine: ||| (c1 ||| c2)

This operator combines the values of a container with another container, or of a string with a string. When the containers are value containers, the operator "+" is used to add or concatenate the values of c1 with c2. For example, `vector v= [1,2,3]; vector vv = [4,5,6]; println (v ||| vv); //[1,4],[2,5],[3,6]` would be the result. Similarly, `ivector iv= [1,2,3]; ivector ivv = [4,5,6]; println (iv ||| ivv); //[5,7,9]` would be the result. The same operation can be done with strings, maps, or maps with integer values.

#### Add: ::

This operator merges the element into the current vector. For example, `1::v` would result in `[1,7,8,1,2]`, and `v::12` would result in `[1,7,8,1,2,12]`.

## Basics

### Declaring a Taskell-like Instruction

All Taskell instructions in Tamgu should be declared between "<..>", which the internal Tamgu compiler utilizes to detect a Taskell formula. For example, `vector v=<map (+1) [1..10]>;` adds 1 to each element of the vector.

### The Simplest Structure

The simplest structure for a Taskell program is to return a value such as `<1>`.

You can also return a calculation such as `<3+1>;`, in which case the system will return a single atomic value.

### Utilization of >, <, |, <<, and >>

These operators can cause issues when used inside a Taskell formula, as they can confuse the compiler with opening or closing Taskell brackets. To avoid this problem, you need to insert these expressions between parentheses.

For example, `<x | x < - [-5..5], (x > 0)>` yields `[1,2,3,4,5]`, and `< (x << 1) | x <- [0..5]>` yields `[0,2,4,6,8,10]`.

### Iteration

The Taskell language provides a convenient and efficient way to represent lists. In Tamgu, these lists are implemented as "vectors", which can be exchanged between different structures.

The most basic Taskell instruction has the following form: `<x | x < - v, Boolean>`. It returns a list as a result, with the following steps:

1. We add x to our current result list.
2. We get x by iterating through v, denoted by `x <- v`.
3. We put a Boolean constraint, which can be omitted.

The reason why it returns a list is due to the iteration in the expression. For example, `<x | x < - [-5..5], x!=0>` yields `[-5,-4,-3,-2,-1,1,2,3,4,5]`.