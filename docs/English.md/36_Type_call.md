# 36 Type Call

This object is used to keep track of functions, which can then be executed. The call is made using the variable name as a function.

## Example

```c++
int display(int e) {
    print("DISPLAY:", e, "\n");
    e += 10;
    return e;
}

int myfunc;
myfunc = display;
int i = myfunc(100);   // display: DISPLAY:100
print("I=", i, "\n");   // display: I=110
```

In the example above, the `display` function takes an integer parameter `e`, prints "DISPLAY:" followed by the value of `e`, increments `e` by 10, and returns the updated value of `e`. The variable `myfunc` is then assigned the `display` function. Finally, the `myfunc` variable is called with the argument `100`, and the result is stored in the `i` variable. The output will be "DISPLAY:100" and "I=110".