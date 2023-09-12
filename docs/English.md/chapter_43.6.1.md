## Examples

### Hanoi Tower

The following program solves the Hanoi Tower problem for you.

```prolog
% We declare our predicate
move.

% Note the variable names, which all start with a "?"
move(1, ?X, ?Y, _) :- 
    println('Move the top disk from', ?X, 'to', ?Y).

move(?N, ?X, ?Y, ?Z) :- 
    ?N > 1, 
    ?M is ?N - 1, 
    move(?M, ?X, ?Z, ?Y), 
    move(1, ?X, ?Y, _), 
    move(?M, ?Z, ?Y, ?X).

% The result will be assigned to res
res :- move(3, "left", "right", "centre").
println(res).
```

If you run this example, you obtain:

```
Move the top disk from left to right
Move the top disk from left to centre
Move the top disk from right to centre
Move the top disk from left to right
Move the top disk from centre to left
Move the top disk from centre to right
Move the top disk from left to right
move(3, 'left', 'right', 'centre')
```

### Ancestor

With this program, you can find the common female ancestor between different people's parent relationship.

```prolog
% We declare all our predicates
k:2 = parent('george', 'andy') --> ancestor('andy', ?Z19), female(?Z19)
r:3 = ancestor(?X, ?X) --> ancestor('andy', ?Z19), female(?Z19)
```