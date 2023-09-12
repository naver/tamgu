# 36 Type call in Tamgu

This object is used to keep track of functions that can be executed later. The call is done using the variable name as a function.

## Example

```tamgu
function display(int e) {
    print("DISPLAY:", e, "\n");
    e += 10;
    return e;
}

call myfunc;
myfunc = display;
int i = myfunc(100); // display: DISPLAY:TEST
print("I=", i, "\n"); // display: I=110
```

In this example, the `display` function takes an integer `e` as an argument. It prints the value of `e` with the prefix "DISPLAY:", adds 10 to `e`, and returns the updated value. 

The `call` object is used to store the `myfunc` variable, which is then assigned the `display` function. Finally, the `myfunc` variable is called with the argument `100`, and the returned value is stored in the `i` variable. The values of `i` and `e` are then printed.