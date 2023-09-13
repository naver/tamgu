# Default Arguments

Tamgu provides a mechanism to declare default arguments in a function. This allows you to define a value for a parameter that can be omitted from the function call.

Here's an example:

```cpp
void acall(int x, int y = 12, int z = 30, int u = 43) {
    cout << (x + y + z + u) << endl;
}

acall(10, 5); // the result is: 88 = 10 + 5 + 30 + 43
```

Note: Only the last parameters in a declaration list can be optional.