## 13 Predefined Types in Tamgu

Tamgu provides a variety of different objects, each with its own specific set of methods. Each of these objects comes with a list of predefined methods.

### 13.1 Basic methods

All the types listed below share the same basic methods:

a) `isa(typename)`: checks if a variable has the type `typename` (as a string).

b) `type()`: returns the type of a variable as a string.

c) `methods()`: returns the list of methods available for a variable according to its type.

d) `info(string name)`: returns help about a specific method.

e) `json()`: returns the JSON representation of the object, when available.

### 13.2 Transparent Object: self (or let)

`self` is a transparent object, similar to a pointer, which does not require any specific transformation for the parameter when used in a function.

Note: The keyword `let` behaves like `self`, with one big difference. The first variable stored in a `let` variable defines the type of that variable. In other words, if you store a string into a `let` variable, then this variable will always behave as a string. You can modify this behavior with the `:=` operator, which in this case forces the `let` variable to a new type.

Example:

```javascript
function compare(self x, self y) {
    if (x.type() == y.type())
        print("This is the same sort of objects");
}
```