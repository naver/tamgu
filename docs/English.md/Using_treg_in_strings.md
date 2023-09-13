## Using treg in strings

It is also possible to use treg directly in strings to extract or modify their content.

```python
uu = "That was: 0x1.0d0e44bfeec9p -3, 0x3.456aebp -1 in here."
print("Tst: <", uu[-r"%d" :" "], ">\\n")  # Tst: <1 >
println("Tst2:", uu["x":r"%d %c"])  # Tst2: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

Note that it is also possible to replace a treg expression with a variable of type treg:

```python
subt = treg(r"%d %c")
println("Tst3:", uu["x":subt])  # Tst3: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

## Taskell

You can also use these treg expressions with Taskell functions:

```python
mytreg(r"%C+") = "uppercase"
mytreg([r"%d+":v]) = "ok"
mytreg(v) = "missed"

println(mytreg("ABC"))  # uppercase
println(mytreg(["3a4", 1, 4, 5]))  # ok
```

## Prolog

You can also use them in Prolog:

```prolog
tst(r"%d+").  % the treg expression is here...
tst("machin").
truc("Ok", ?P) :- tst(?P).
vector vpred.
vpred = truc(?P, "machin").
println(vpred)  % [truc("Ok",machin)]
vpred = truc(?P, "12").
println(vpred)  % [truc("Ok",12)]
vpred = truc(?P, "nope").
println(vpred)  % []
```