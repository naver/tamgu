## System Functions

### Exit: `_exit()`

This function is used to exit a program definitively.

### Error on Key: `_erroronkey(bool)`

By default, any attempt to access a value in a map with an unknown key does not raise an exception. The function `_erroronkey(bool)`, which should be placed at the very beginning of your code, modifies this behavior.

### Stack Size: `_setstacksize(size)`

The stack size is initially set to 1000 function calls. You can modify this value with this function. However, if your stack size is too large, then your program might crash as it could exceed the actual stack size of your system.

### Number of Threads: `_setthreadlimit(nb)`

The number of actual threads that can run in parallel is initially set to 1000 on Windows and 500 on other systems. You can modify this value to increase the number of threads that can run in parallel.

### Valid Features: `_setvalidfeatures(mapss features)`

This method is used to put constraints on the valid features that can be used for both synodes and dependencies. See the synode, dependency section for more details.

### Number of Threads to be Joined Together: `_setjoinedlimit(nb)`

By default, up to 256 threads can be "joined" together. You can modify this number with this function.

### Initial Environment Variables: `_setenv(varname, value)`

It is possible to set environment variables at launch time with this function.

### Tamgu Version: `_version()`

Returns a string with version information about Tamgu.

### Mirror Display: `_mirrordisplay(bool)`

This function is used to set the mirror display from within a GUI. When activated, "print" displays values both on the GUI output and the command window output.

### Function Argument Compatibility: `_loosecompatibility(bool v)`

By default, a call to a function that requires integer or string parameters with arguments that are neither fails at compile time:

```cpp
// call requires two ints
function call(int x, string y) {
    println(x, y);
}

// This will fail
call("9", 12);
```

It is possible to relax this constraint with `_loosecompatibility(true)`. Then the above call will be accepted, and arguments will be automatically converted into their correct values. Numbers and strings will be automatically converted into one another.

### Memory Management: `_poolstats()`

This function is used to return the current state of different object pools. For efficiency reasons, some objects are managed in pools, which allows for the reusability of recurrent objects (such as strings, integers, floats, vectors, or maps). These objects are not deleted but cleared and yielded back for reuse when needed. The result of this function is a `mapsi` object.