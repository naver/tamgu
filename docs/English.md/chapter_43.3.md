## Clauses

A clause is defined as follows:

```
predicate(arg1,arg2,...,argn) :- pred(arg1,...),pred(arg,...), etc.;
```

A fact is a knowledge base. It can be declared in a program using the following syntax:

```
predicate(val1,val2,...).
```

or

```
predicate(val1,val2,...) :- true.
```

The `loadfacts(pathname)` function can be used to load a large knowledge base from a file. This function should be placed at the beginning of the file to start loading at compile time.

### Disjunction

Tamgu also accepts disjunctions in clauses using the operator `;`, which can be used in place of `,` between predicates.

Example:

```
predicate mere,pere;
mere("jeanne","marie").
mere("jeanne","rolande").
pere("bertrand","marie").
pere("bertrand","rolande").
predicate parent;
parent(?X,?Y) :- mere(?X,?Y);pere(?X,?Y).
parent._trace(true);
vector v = parent(?X,?Y);
println(v);
```

Result:

```
r:0=parent(?X,?Y) --> parent(?X6,?Y7)
e:0=parent(?X8,?Y9) --> mere(?X8,?Y9)
k:1=mere('jeanne','marie').
success:2=parent('jeanne','marie')
k:1=mere('jeanne','rolande').
success:2=parent('jeanne','rolande')
[parent('jeanne','marie'),parent('jeanne','rolande')]
```

### Cut, Fail, and Stop

Tamgu provides the cut operator, expressed as `!`, which can be used to control the execution flow. It also provides the keyword `fail`, which can be used to force the failure of a clause. The `stop` keyword stops the entire evaluation.

### Functions

Tamgu provides some regular functions from the Prolog language, such as:

- `asserta(pred(...))`: Asserts (inserts) a predicate at the beginning of the knowledge base. Note that this function can only be used within a clause declaration.
- `assertz(pred(...))`: Asserts (inserts) a predicate at the end of the knowledge base. Note that this function can only be used within a clause declaration.
- `retract(pred(...))`: Removes a predicate from the knowledge base. Note that this function can only be used within a clause declaration.
- `retractall(pred)`: Removes all instances of the predicate `pred` from the knowledge base. If used without any parameters, it cleans the whole knowledge base. Note that this function can only be used within a clause declaration.
- `predicatedump(pred)` or `findall(pred)`: Returns all predicates stored in memory as a vector. If a predicate name is provided as a string, it dumps all predicates with the specified name.

Example:

```
predicate parent;
adding(?X,?Y) :- asserta(parent(?X,?Y)).
adding("Pierre","Roland");
println(predicatedump(parent));
```

### Universal Predicate Name

When querying facts in a knowledge base without a specific predicate name, you can use `_` in place of the predicate name.

Example:

```
father("george","sam").
father("george","andy").
mother("andy","mary").
mother("sam","christine").
parent(?A,?B) :- _(?A,?B).
```

You can also use specific variables `_0` to `_9`, which return the matched predicate name.

Example:

```
parent(?A,?B,?P) :- _1(?A,?B), ?P is _1.
```

### Tail Recursion

Tail recursion is a mechanism that transforms recursion into an iterative process. To activate tail recursion, modify the name of the last element of the rule by adding `#`.

Example:

```
vector v = [1..10];
traverse([],0).
traverse([?X|?Y],?A) :-
    println(?X,?Y),
    traverse#(?Y,?A).
vector vv = traverse(v,?A);
println(vv);
```

### Callback Function

A predicate can be declared with a callback function. The signature of the callback function is as follows:

```
function OnSuccess(predicatevar p, string s) {
    println(s,p);
    return true;
}
```

To associate the callback function with a predicate, use the following syntax:

```
string s = "Parent:";
predicate parent(s) with OnSuccess;
parent("John","Mary") :- true.
parent("John","Peter") :- true.
parent(?X,?Y);
```

The callback function is called each time the evaluation on `parent` is successful. The second argument in the function corresponds to the parameter given to `parent` in the declaration. If the function returns true, the inference engine tries other solutions; otherwise, it stops.

Result:

```
Parent: parent('John','Mary')
Parent: parent('John','Peter')
```
