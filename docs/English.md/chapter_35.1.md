# Specific Instructions in Tamgu

Tamgu provides a comprehensive set of operations to handle various algorithms, including if, else, elif, switch, for, and while statements. 

## Boolean Operators

Tamgu supports the following boolean operators: and (&&), or (||), and xor. These operators are used to combine different elements with boolean values. The and and or operators can also be written as && and ||, respectively. 

- `and`: Succeeds if all elements are true.
- `or`: Succeeds if at least one element is true.
- `xor`: Succeeds if at least one element is true, but fails if all elements are true.

## if-elif-else Statements

Tamgu supports if-elif-else statements for conditional branching. The syntax is as follows:

```python
if (boolean_expression) {
    // code block
}
elif (boolean_expression) {
    // code block
}
...
else {
    // code block
}
```

## ifnot Statement

The `ifnot` statement is used to chain multiple instructions and returns the value of the first instruction that is not false, null, or empty. It is commonly used for complex sequences of operations. 

```python
result = call1() ifnot call2() ifnot call3()...
```

Note that in the case of a complex sequence of operations, the entire sequence of `ifnot` statements must be enclosed in parentheses.

## Example:

```python
map m1 = { "a": 1, "b": 2, "c": 3 };
map m2 = { "A": 4, "B": 5, "C": 6 };
map m3 = { "aa": 7, "bb": 8, "cc": 9 };
int i = (m1["A"] ifnot m2["B"] ifnot m3["bb"]) + 24;
println(i); // Output: 29

function calling(string s) {
    return (m1[s] ifnot m2[s] ifnot m3[s]);
}
println(calling('bb')); // Output: 8
```

## switch Statement

The `switch` statement allows you to list a series of tests for a single object. The syntax is as follows:

```python
switch (expression) {
    v1: {
        // code block
    }
    v2: {
        // code block
    }
    default: {
        // code block
    }
}
```

The `v1`, `v2`, and `vn` can be strings, integers, or floats. The `expression` is evaluated once and compared with the values `v1`, `v2`, `vn`, etc. You can also use a function in place of a simple comparison between elements. The function should return true or false.

```python
// Example: Testing whether one value is larger than the other
function tst(int i, int j) {
    if (j >= i)
        return true;
    return false;
}

int s = 10;
// Testing through the `tst` function
switch (s) with tst {
    1: println("1");
    2: println("2");
    20: println("20"); // This will be the selected occurrence
}
```