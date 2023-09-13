## Basics

### Declaring a Taskell-like instruction

All Taskell instructions in Tamgu should be declared between `<..>`, which the internal Tamgu compiler utilizes to detect a Taskell formula.

Example:
```cpp
vector v = <map (+1) [1..10]>;
```
The above instruction adds 1 to each element of the vector.

### Simplest structure

The simplest structure for a Taskell program is simply to return a value such as:
- `<1>`

You can return a calculation:
- `<3+1>`

In that case, the system will return one single atomic value.

Example:
- `<12+3>` returns 15...

### Utilization of: >, <, |, << and >>

These operators can cause some issues when used inside a Taskell formula, since they can confuse the compiler with opening or closing Taskell brackets. To avoid this problem, you need to insert these expressions between parentheses.

Example:
- `<x | x < - [-5..5], (x > 0)>` yields `[1,2,3,4,5]`
- `<(x << 1) | x <- [0..5]>` yields `[0,2,4,6,8,10]`
- `<(12|3)>` yields 15

### Iteration

The Taskell language provides a very convenient and efficient way to represent lists. In Tamgu, these lists are implemented as "vectors", which can be exchanged between different structures.

The most basic Taskell instruction has the following form:
- `<x | x < - v, Boolean>`

It returns a list as a result, which reads as:
1. We add x to our current result list.
2. We get x by iterating into v, i.e., `x <- v`.
3. We put a Boolean constraint, which can be omitted.

The reason why it returns a list is due to the iteration in the expression.

Example:
- `<x | x < - [-5..5], x!=0>` yields `[-5,-4,-3,-2,-1,1,2,3,4,5]`

Note: You can use the `←` character instead of `<-`. Hence, you may write the above statement as:
- `<x | x ← [-5..5], x!=0>`

### Combining

You can combine different iterators together. There are two ways to do it: either as if the two iterations were embedded one into the other, or simultaneously.

1. Embedded: The different iterators are separated with a `,`
- `<x+y | x < - v, y < - vv, (x+y > 10)>`

2. Simultaneous: The different iterators are combined with a `;`
- `<x+y | x < - v ; y <- vv, (x+y > 10)>`

Example:
- `<x+y | x < - [1..5], y < - [1..5]>` yields `[2,3,4,5,6,3,4,5,6,7,4,5,6,7,8,5,6,7,8,9,6,7,8,9,10]` (25 elements)
- `<x+y | x < - [1..5] ; y <- [1..5]>` yields `[2,4,6,8,10]` (5 elements)

### Vector pattern

You can also use vector patterns to extract elements from the list if the list is composed of sub-lists.

Example:
```cpp
vector v = [[1, "P", true], [2, "C", false], [3, "E", true]];
vector vv = <[y, t] | [y, n, t] <- v, y :> 1>;
```
The result is `[[2, false], [3, true]]`

### Iterations in maps

Tamgu also provides a specific way to iterate among maps, which in this case is quite different from what is usually available in Haskell implementations.

Tamgu already provides a mechanism to iterate among maps in `for`, with keys and values provided as a recipient to the iteration process:
```cpp
for ({x:y} in m) ...
```

The same mechanism is used here to iterate among values in a map, but also to return specific values to the recipient map.
```cpp
< {x:y} | {y:x} < - m>;
```

Example:
```cpp
// We declare our map
map m = {"a": 1, "b": 2, "c": 3, "d": 4};
// This map is the recipient to the Taskell expression...
// We iterate among key/value in m, and we return the same values inverted...
map mr = < {x:y} | {y:x} < - m>;
```
The result is `{1: 'a', 4: 'd', 2: 'b', 3: 'c'}`