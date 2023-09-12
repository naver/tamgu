## Methods

The `treg` type exposes two methods:

1. `compile(string rgx)`: compiles a string into a `treg` object.
2. `match(string s)`: checks if the string `s` matches the Tamgu regular expression.

Example:

```cpp
treg tt("%C+");
if (tt.match("ABCDE"))
    println("Yes");
if (tt == "aABCDE")
    println("Yes");
else
    println("No");
```

You can also use `r` strings to perform the same type of operation:

```cpp
if (r"%C+" == "ABCDE")
    println("Yes");
```

An `r` expression can be written with either double quotes (`""`) or single quotes (`''`).