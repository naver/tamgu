## Types

Tamgu exposes three specific types for inference objects:

### Predicate
This type is used to declare predicates, which will be used in inference clauses. This type exposes the following methods:
1. `name()`: return the predicate label
2. `size()`: return the number of arguments
3. `_trace(bool)`: activate or deactivate the trace for this predicate when it is the calling predicate.

### Term
This type is used to declare terms, which will be used in inference clauses (see the NLP example below).

### Other Inference Types: List and Associative Map
Tamgu also provides the traditional lists à la Prolog, which can be used with the "|" operator to handle list decomposition (see the NLP example below for a demonstration of this operator).

**Example:**
```prolog
predicate alist;
//in this clause, C is the rest of the list...
alist([?A,?B|?C],[?A,?B],?C) :- true.
v = alist([1,2,3,4,5], ?X,?Y);
println(v); → [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

Tamgu also provides an associative map, which is implemented as a Tamgu map, but in which the argument order is significant.

**Example:**
```prolog
predicate assign, avalue;
avalue(1,1) :- true.
avalue(10,2) :- true.
avalue(100,3) :- true.
avalue("fin",4) :- true.
```

**Example:**
```prolog
vector v = ['female', 'mary'];
predicatevar fem;
fem = v.predicate(); //we transform our vector into a predicate.
fem.store(); //we store it in the fact base.
v = fem.query(female, ?X); //We build a predicate query on the fly.
v = fem.query(female, 'mary'); //We build a predicate query with a string.
```