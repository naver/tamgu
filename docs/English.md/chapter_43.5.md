## Launching an Evaluation

Evaluations are launched in the same way as a function. You can provide as many inference variables as you want, but you can only launch one predicate at a time. If you want to include more than one predicate, you should first declare your expression as a clause.

### Important

If the recipient variable is a vector, all possible analyses will be provided and the evaluation tree will be fully traversed. If the recipient variable is anything else, the evaluation will stop whenever a solution is found.

### Mapping Methods to Predicates

Most object methods are mapped into predicates in a simple way. For example, if a string exports the method "trim", a "p_trim" predicate with two variables is created. Each method is mapped to a predicate by adding the prefix "p_". 

The first argument of this predicate is the head object of the method, while the last parameter is the result of applying this method to that object. For example, if `s` is a string and `s.trim()` is called, it becomes `p_trim(s,?X)`, where `?X` is the result of applying `trim` to `s`. If `?X` is unified, the predicate will check if `?X` is the same as `s.trim()`.

**Example:**  
`compute(?X,?Y) :- p_log(?X,?Y).`  
`between(?X,?B,?E), succ(?X,?Y)`

- `between(?X,?B,?E)` checks if the value `?X` is between `?B` and `?E`.
- `succ(?X,?Y)` returns the successor of `?X`. `succ` can also be used as a term, but in that case, it only uses one argument.

### Common Mistakes with Tamgu Variables

If you use common variables in predicates, such as string `s`, integer `s`, or any other types of variables, you need to remember that these variables are used in predicates as comparison values. Here's an example to clarify what we mean:

**Example 1:**
```
string s = "test";
string sx = "other";
predicate comp;
comp._trace(true);
comp(s, 3) :- println(s).
comp(sx, ?X).
```
**Execution:**
```
r:0 = comp(s, 3) --> comp(other, ?X172) --> Fail
```
This clause failed because `s` and `sx` have different values.