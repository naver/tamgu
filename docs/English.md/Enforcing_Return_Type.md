## Enforcing Return Type

In JavaScript, a function can specify a return type that is evaluated dynamically when a value is returned. The return type is declared after the argument list using the "::" syntax.

Here's an example of a function named "toto" that takes an integer argument and enforces that it returns an integer:

```javascript
function toto(i) :: int {
    i += 10;
    return i;
}
```

In this case, the function "toto" adds 10 to the input integer and returns the result. The return type is explicitly declared as "int" to enforce that the function must return an integer value.