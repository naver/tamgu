# Predefined Types

Tamgu provides many different objects, each with a specific set of methods. Each of these objects comes with a list of predefined methods.

## Basic Methods

All the types below share the same basic methods:

a) `isa(typename)`: Check if a variable has the type `typename` (as a string).

b) `type()`: Return the type of a variable as a string.

c) `methods()`: Return the list of methods available for a variable according to its type.

d) `info(string name)`: Return help about a specific method.

e) `json()`: Return the JSON representation of the object, when available.

## Transparent Object: `self` (or `let`)

`self` is a transparent object, similar to a sort of pointer, which does not require any specific transformation for the parameter when used in a function.

Note: The keyword `let` behaves as `self`, with one big difference. The first variable stored in a `let` variable defines the type of that variable. In other words, if you store a string into a `let` variable, then this variable will always behave as a string. You can modify this behavior with the operator `:=`, which in this case forces the `let` variable to a new type.

### Example

```javascript
function compare(self x, self y) {
    if (x.type() == y.type())
        print("This is the same sort of objects");
}

// For instance, in this case, the function compare receives two parameters, whose types might vary. A self declaration removes the necessity to apply any specific conversion to the objects that are passed to that function.

string s1, s2;
compare(s1, s2); // We compare two strings

myframe i1;
myframe i2;
compare(i1, i2); // We compare two frames
```

### Example with `let`

```javascript
let l = "x=";
l += 12;
println(l); // It displays: x=12

// We force 'l' to be an integer
l := 1;
l += 12;
println(l); // It displays: 13
```