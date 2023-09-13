## Alias

An alias is similar to a function, but with some specific differences:

a) Parameters are described without any types.
b) Arguments in an alias can all be modified.
c) An alias can be used to replace a complex structure call.
d) An alias can be used as a simple function, in which case it must return its value with `return`.

### Pattern

An alias function is implemented in the following way:

```
alias name(a, b, ..., z) = code;
```

### Example

```python
alias removing(a) = a.replace("-", "").replace(r"%p", " ").lower();
alias display(a, b) = {
    println(a, b);
}

a = removing(a);
display("Test", a);
```

### Complex Patterns

The example below shows how you can create an alias function that accesses the content of a frame:

```python
alias get(a, i) = a.persons.pnames[i];
```

This alias can be used both to retrieve an element or to modify it. When you need to modify an element, you must use the following brackets: `[*...]` to provide the different arguments.

#### Example

```python
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

frame Items {
    ListOfPerson persons;
    function string() {
        return persons;
    }
}

// We define an alias to access pnames in ListOfPerson
alias get(a, i) = a.persons.pnames[i];

// We declare an element of type Items.
Items itms;

// We access pnames value at position 1
println(get(itms, 1));

// We modify a pnames value at position 1
// Note the use of [*itms, 1]
get[*itms, 1] = "B";
```