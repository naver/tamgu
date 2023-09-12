## Multiple Definitions

Tamgu allows for multiple definitions of functions with the same name, but differing in their parameter definitions. For example, you can implement a `display(string s)` and a `display(int s)` function. 

In this case, when multiple functions are implemented with the same name, the argument control is stricter compared to a single implementation. The system will try to find the most suitable function based on the argument list of the function call. As a result, the mechanism for translating arguments into values suitable for function parameters is no longer available. 

### Example

```javascript
function testMultipleDeclaration(string s, string v) {
    println("String:", s, v);
}

function testMultipleDeclaration(int s, int v) {
    println("Int:", s, v);
}

// Declare variables
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// The system will choose the right function based on the argument list
testMultipleDeclaration(s1, s2); // Calls the string implementation
testMultipleDeclaration(i, j); // Calls the integer implementation
```

In the example above, the system will choose the appropriate function based on the arguments passed to `testMultipleDeclaration`. The first function will be called when string arguments are provided, while the second function will be called when integer arguments are provided.