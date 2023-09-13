# Functions, Autorun, and Threads in Tamgu

In Tamgu, a function is declared using the keyword `function`, followed by its name and parameters. To return a value from a function, the keyword `return` is used. Parameters are always passed by value, unless the type is `self`. It is important to note that a function in Tamgu does not specify any types for its return value.

## Autorun

Tamgu also supports the concept of autorun. This means that a function can be automatically executed when the program starts. To define an autorun function, simply declare a function with the name `autorun`. This function will be executed automatically at the beginning of the program.

## Threads

Tamgu provides support for multithreading through the use of threads. Threads allow for concurrent execution of multiple parts of a program. To create a thread, the keyword `thread` is used, followed by the function name and any necessary parameters. The thread will execute the specified function concurrently with the main program.

Example:

```tamgu
function myFunction(param1, param2) {
    // function body
    return result;
}

autorun() {
    // autorun function body
}

thread myThread(myFunction, param1, param2);
```

In the above example, `myFunction` is a regular function that can be called at any time in the program. The `autorun` function will be executed automatically when the program starts. The `myThread` thread will execute `myFunction` concurrently with the main program.

By using functions, autorun, and threads in Tamgu, you can create more versatile and efficient programs.