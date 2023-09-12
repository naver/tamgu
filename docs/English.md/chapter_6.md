# Alias in Tamgu

An alias in Tamgu is similar to a function, but it has some specific differences:

a) Parameters are described without specifying their types.
b) Arguments in an alias can all be modified.
c) An alias can be used to replace a complex structure call.
d) An alias can be used as a simple function. In this case, it must return its value using the `return` keyword.

## Pattern

An alias function is implemented in the following way:

```
alias name(a, b...z) = code;
```

Example:

```
alias removing(a) = a.replace("-", "").replace(r"%p", " ").lower();
alias display(a, b) = {
    println(a, b);
}

a = removing(a);
display("Test", a);
```

## Complex patterns

The example below shows how you can create an alias function that accesses the content of a frame:

```
alias get(a, i) = a.persons.pnames[i].
```

This alias can be used both to retrieve an element or to modify it.

When you need to modify an element, you must use the following brackets: `[*...]` to provide the different arguments.

Example:

```tamgu
// Two frame descriptions
frame ListOfPerson {
    svector pnames;
    function _initial() {
        pnames = ["a".."e"];
    }
    function string() {
        return pnames;
    }
}
```