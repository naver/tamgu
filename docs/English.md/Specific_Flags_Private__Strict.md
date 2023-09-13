## Specific Flags: Private & Strict

Functions can also be declared with two specific flags that are inserted before the function keyword: private and strict.

**Note:**
If you wish to use both flags in the same definition, private should precede strict.

### Private [function | thread | autorun | polynomial]

When a function is declared private, it cannot be accessed from outside the current Tamgu file. If a Tamgu program is loaded from within another Tamgu program, private functions are unreachable from the loader.

**Example:**
```javascript
// This function is invisible from external loaders...
private function toto(int i) {
    i += 10;
    return i;
}
```

### Strict [function | thread | polynomial]

By default, when a function is declared in Tamgu, the language tries to convert each argument from the calling function into the parameters expected by the function implementation. However, this mechanism might be too loose in certain cases where stricter parameter checking is required. The strict flag helps solve this problem, as any function declared with this flag will demand strict parameter control.

**Example:**
```javascript
strict function testStrictDeclaration(int s, int v) {
    println("Int:", s, v);
}

// We declare our variables
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// In this case, the system will choose the right function according to its argument list...
testMultipleDeclaration(s1, s2); // the string implementation
testMultipleDeclaration(i, j); // the integer implementation
```