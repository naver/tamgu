## Taskell Operators

Before diving into the details of the Taskell language, let's introduce some new operators that have been added to enhance the Taskell world. These operators are also available in Tamgu, but their true value lies in the way they enrich Taskell.

### Range Declarations: [a..b]

To align with the Haskell language, we have introduced a new way to declare a range of elements using the ".." operator. For example, `[1..10]` defines the vector: `[1,2,3,4,5,6,7,8,9,10]`.

### Step

By default, the step is 1, but you can set a different step. You can either directly define it using a ":" at the end of the expression. For example, `[1..10:2]` defines the vector: `[1,3,5,7,9]`. Alternatively, you can define the step by providing the next element in the definition. For example, `[1,3..10]` defines the vector: `[1,3,5,7,9]`. This step functionality also works with characters. For instance, `[‘a’,’c’..’g’]` defines the vector: `['a','c','e','g']`. The same vector can also be defined as `[‘a’..’g’:2]`.

### Infinite Ranges

Taskell also supports the concept of infinite ranges of elements. There are two cases: you can either ignore the first element or the last element of the set.

- `[1..]` defines an infinite vector that starts at 1, moving forward: `[1,2,3,4,...]`.
- `[..1]` defines an infinite vector that starts at 1, moving backward: `[1,0,-1,-2,...]`.

You can also use different steps:

- `[1..:2]` defines an infinite vector that starts at 1, moving forward: `[1,3,5,...]`.
- `[..1:2]` defines an infinite vector that starts at 1, moving backward: `[1,-1,-3,...]`.

Or:

- `[1,3..]` defines an infinite vector that starts at 1, moving forward: `[1,3,5,...]`.
- `[..-1,1]` defines an infinite vector that starts at 1, moving backward: `[1,-1,-3,...]`.

### Three New Operators: &&&, |||, and ::

These three operators are used to concatenate a list of elements or to add an element to a vector.

#### Merge: "&&&"

The "&&&" operator is used to merge different elements into a vector. If one of the elements is not a list, it is simply merged into the current list.

Example:

```
vector v = 7 &&& 8 &&& [1,2];
println(v); // [7,8,1,2]
```

This operator is similar to "++" in Taskell. Since "++" was already defined in Tamgu, we modified it to "&&&".

#### Combine: "|||"

The "|||" operator combines the values of a container with another container or combines a string with another string. When the containers are value containers, the operator "+" is used to add or concatenate the values of the first container with the second container.

Examples:

```
vector v = [1,2,3];
vector vv = [4,5,6];
println(v ||| vv); // [[1,4],[2,5],[3,6]]

ivector iv = [1,2,3];
ivector ivv = [4,5,6];
println(iv ||| ivv); // [5,7,9]

string a = "abc";
string b = "EFG";
println(a ||| b); // "aEbFcG"

map m = {'a':12, "b":24};
map mm = {'f':12, 'a':44, 'c':255};
println(m ||| mm); // {'b':24, 'c':255, 'f':12, 'a':[12,44]}

mapsi sim = {'a':12, "b":24};
mapsi simm = {'f':12, 'a':44, 'c':255};
println(sim ||| simm); // {'b':24, 'f':12, 'c':255, 'a':56}
```

#### Add: "::"

The "::" operator is similar to the previous one, but with a significant difference. It merges the element into the current vector.

Examples:

```
1::v // [1,7,8,1,2] (new value of v)

v::12 // [1,7,8,1,2,12] (new value of v)
```