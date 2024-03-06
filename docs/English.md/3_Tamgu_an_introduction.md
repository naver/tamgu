# Tamgu: System Functions

The Tamgu language borrows many concepts from other languages, primarily C++ and Python. Therefore, it is quite straightforward to learn for someone with a basic knowledge of these languages.

## Some Elements of the Language

A Tamgu program contains variable declarations, function declarations, and frame (or class) declarations. Variables and functions can be declared anywhere, except within loops.

### Comments

Comments for a single line are introduced with `//`. For example:

```cpp
// This is a comment
```

Comments for a block of lines are inserted between `/@` and `@/`. For example:

```cpp
/@
This is
a block of comments.
@/
```

### Functions

A function is declared with the keyword `function`, followed by a name and some parameters.

### Frames

A frame is declared with the keyword `frame`, followed by a name. A sub-frame is simply declared as a frame within a frame.

### Function and Frame Pre-declarations

The pre-declaration of functions and frames is not necessary in Tamgu, as the interpreter first loops into the code to detect all functions and frames and declares them beforehand. Therefore, the following code is valid:

```cpp
// We call call2 from within call1
function call1(int x, int y) {
    call2(x, y);
}

// call2 is declared after call1
function call2(int x, int y) {
    println(x, y);
}
```

## System Functions

### Exit: `_exit()`

This function is used to exit the program.

### Error on Key: `_erroronkey(bool)`

By default, any attempt to access a value in a map with an unknown key does not raise an exception. The function `_erroronkey(bool)`, which should be placed at the very beginning of your code, modifies this behavior.

### Stack Size: `_setstacksize(size)`

The stack size is initially set to 1000 function calls. You can modify this value with this function. However, if your stack size is too large, then your program might crash as it could become larger than the actual stack size of your system.

### Number of Threads: `_setthreadlimit(nb)`

The number of actual threads that can run in parallel is initially set to 1000 on Windows and 500 on other systems. You can modify this value to increase the number of threads that can run in parallel.

### Valid Features: `_setvalidfeatures(mapss features)`

This method is used to put constraints on the valid features that can be used for synodes and dependencies. See the synode, dependency section for more details.

### Number of Threads to be Joined Together: `_setjoinedlimit(nb)`

By default, up to 256 threads can be "joined" together. You can modify this number with this function.

### Initial Environment Variables: `_setenv(varname, value)`

It is possible to set environment variables at launch time with this function.

### Tamgu Version: `_version()`

Returns a string with version information about Tamgu.

### Mirror Display: `_mirrordisplay(bool)`

This function is used to set the mirror display from within a GUI. When activated, "print" displays values both on the GUI output and the command window output.

### Function Argument Compatibility: `_loosecompatibility(bool v)`

By default, a call to a function that requires integer or string parameters with arguments that are neither fails at compile time. However, it is possible to relax this constraint with `_loosecompatibility(true)`. Then, the above call will be accepted, and arguments will be automatically converted into their right values. Numbers and strings will be automatically converted into one another.

### Memory Management: `_poolstats()`

This function is used to return the current state of different object pools. For efficiency reasons, some objects are managed in pools, which allows for the reusability of recurrent objects (such as strings, integers, floats, vectors, or maps). These objects are not deleted but cleared and yielded back for reuse when needed. The result of this function is a `mapsi` object.

## Passing Arguments to a Tamgu Program

When a Tamgu program is called with a list of arguments, each of these arguments becomes available to the Tamgu program through three specific system variables: `_args`, `_current`, and `_paths`.

### Example:

```bash
tamgu myprogram c:\test\mytext.txt
```

### `_args`: Argument Vector

Tamgu provides a specific variable called `_args`, which is actually a string vector that stores each argument according to its position in the declaration list.

### Example (from the call above):

```cpp
file f;
f.openread(_args[0]);
```

### `_paths` and `_current`: Path Management

Tamgu provides a vector variable called `_paths`, which stores the pathnames of the different Tamgu programs that have been loaded. The first element of this vector, `_paths[0]`, stores the current directory pathname, while `_paths[1]` stores the path of the current program file.

Tamgu also provides another variable called `_current`, which stores the path of the program file that is currently being run. The path stored in `_current` always finishes with a final separator. Actually, `_current` points to the same path as `_paths[1]`.

### `_endl`: Carriage Return

Windows and Unix use different carriage return characters. `_endl` returns the proper carriage return according to the platform value.

### `_sep`: Separator in Pathnames

Unix-based systems and Windows use different separators in pathnames between directory names. Unix requires a "/" while Windows requires a "\". Tamgu provides a specific variable called `_sep`, which returns the right separator according to your system.

### `_seteventvariable(var)`: Set the event variable.
This variable must be declared as a global variable in your code. It can be a string, a svector or a simple vector. An `event` variable can be modified with `_pushevent`, but it also receives the error message if one occurs during the execution. When the error occurs, the system returns the whole event, including the one that were stored with `pushevent`.

### `_pushevent(v1,v2,v3...)`: Push an event in the event variable
It adds a series of strings into the event variable.

```C++
string event;
_seteventvariable(event);

float v = 10000;
for (int i in <3,-1,-1>) {
    _pushevent("I =",i);
    v = v/i;
}

// Tamgu displays:

/*
I = 3
I = 2
I = 1
I = 0
0: [sequence] at 11 in file
1:  at 11 in file
2: =(0/0) at 13 in file
Cannot divide by 0 at 13 in file
*/

```


## Console

Tamgu provides a default console, which can be used to load and edit any programs. The console can be used to test small pieces of code or to check the values at the end of an execution.

You can also execute a program in debug mode, which displays the content of the stack and the variables at each step in your program.

To launch the console, run Tamgu with the `-console` option.
