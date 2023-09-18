# Function, Autorun, Thread

In Tamgu, a function is declared using the keyword `function`, followed by its name and parameters. A value can be returned using the keyword `return`. Parameters are always sent as values, except if the type is `self`. It should be noted that a function does not provide any type information for its return value.

## Enforcing Return Type

A function can specify a return type by using the `::` symbol after the argument list. For example:

```
function toto(int i) :: int {
    i += 10;
    return i;
}
```

In this example, the function `toto` is declared to return an `int`.

## Autorun

An `autorun` function is automatically executed after its declaration. Autorun functions are only executed in the main file. If you have autorun functions in a file that is called from another file, those functions will not be executed.

Example:

```
autorun waitonred() {
    println("Loading:", _paths[1]);
}
```

## Thread

When a `thread` function is launched, it is executed in an independent system thread.

Example:

```
thread toto(int i) {
    i += 10;
    return i;
}

int j = toto(10);
print("J=" + j + "\n");
```

Output:

```
J=20
```

### Protected Thread

A `protected` thread prevents two threads from accessing the same lines of code at the same time. A protected thread sets a lock at the beginning of its execution, which is released once the function is executed. This ensures that different calls to a protected function will be done in sequence and not at the same time. Protected should be used for code that is not reentrant.

Example:

```
// We implement a synchronized thread
protected thread launch(string n, int m) {
    int i;
    println(n);
    // We display all our values
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // We launch our thread
    launch("Premier", 2);
    launch("Second", 4);
    println("End");
}

principal();
```

Output:

```
End
Premier
0 1
Second
0 1 2
```

### Exclusive Thread

An `exclusive` thread is similar to a protected thread, but with one difference. In the case of a protected thread, the protection is at the method level, while with an exclusive thread, it is at the frame level. This means that exclusive threads only allow one thread to have access to the frame object at a time.

Example:

```
// This frame exposes two methods
frame disp {
    // Exclusive
    exclusive thread edisplay(string s) {
        println("Exclusive:", s);
    }
    // Protected
    protected thread pdisplay(string s) {
        println("Protected:", s);
    }
}

// We also implement a task frame
frame task {
    string s;
    // With a specific "disp" instance
    disp cx;

    function _initial(string x) {
        s = x;
    }

    // We call our local instance with protected
    function pdisplay() {
        cx.pdisplay(s);
    }

    // We call our local instance with exclusive
    function edisplay() {
        cx.edisplay(s);
    }

    // We call the global instance with exclusive
    function display(disp c) {
        c.edisplay(s);
    }
}

// The common instance
disp c;
vector v;
int i;
string s = "T";

for (i = 0; i < 100; i++) {
    s = "T" + i;
    task t(s);
    v.push(t);
}

// In this case, the display will be ordered as protected is not reentrant
// Only one pdisplay can run at a time
for (i = 0; i < 100; i++)
    v[i].pdisplay();

// In this case, the display will be a mix of all edisplay working in parallel
// Since exclusive only protects methods within one instance, and we have different
// instances in this case...
for (i = 0; i < 100; i++)
    v[i].edisplay();

// In this last case, we have one single common object "disp" shared by all "task"
// The display will be again ordered as with protected, since this time we run into the same
// "c disp" instance.
for (i = 0; i < 100; i++)
    v[i].display(c);
```

### Joined and Waitonjoined

A thread can be declared as joined, which means the main thread will wait for the completion of all the threads that were launched before completing its own code. You can use `waitonjoined()` to wait for these threads to finish. You can use as many `waitonjoined()` as necessary in different threads. `waitonjoined()` only waits for joined threads that were launched within a given thread.

### Atomic Values

Most data structures (maps, vectors, strings, etc.) are protected in threads against concurrent access through a lock. However, these locks can be costly in terms of time and space. To address this, Tamgu provides lock-free data structures (atomic types) such as `a_int`, `a_string`, or `a_mapii`. These data structures allow for a more efficient use of the machine, as threads accessing these types are not put on hold. However, their implementation makes them slower than other types in a non-threading context. They are only useful when shared across threads.

### Stream Operator '<<<'

When you launch a thread, the result of that thread cannot be directly stored in a variable using the `=` operator, as a thread lives its own life without any links to the calling code. Tamgu provides a specific operator for this task: `<<<`, also known as the stream operator. A stream is a variable that is connected to the thread in such a way that the values returned by the thread can be stored within that variable. Of course, the variable must exist independently from the thread.

Example:

```cpp
// We create a thread as a "join" thread, in order to be able to use waitonjoined.
// This thread simply returns 2 * x
joined thread Test(int i) {
    return (i * 2);
}

// Our launch function, which will launch 10 threads
function launch() {
    // We first declare our map storage variable within this function
    treemapi m;
    int i = 0;

    // We then launch 10 threads and store the result of each into m at a specific position
    for (i in <0,10,1>)
        m[i] <<< Test(i);

    // We wait for all threads to finish
    waitonjoined();

    // We display our final value
    println(m); // {0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```

## Multiple Definitions

Tamgu allows for multiple definitions of functions sharing the same name but differing in their parameter definition. For example, you can implement a `display(string s)` and a `display(int s)`. In this case, the system will choose the most suitable function based on the argument list of the function call.

Example:

```cpp
function testmultipledeclaration(string s, string v) {
    println("String:", s, v);
}

function testmultipledeclaration(int s, int v) {
    println("Int:", s, v);
}

// We declare our variables
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// In this case, the system will choose the right function according to its argument list
testmultipledeclaration(s1, s2); // The string implementation
testmultipledeclaration(i, j); // The integer implementation
```

## Default Arguments

Tamgu provides a mechanism to declare default arguments in a function. You can define a value for a parameter that can be omitted from the call.

Example:

```cpp
function acall(int x, int y = 12, int z = 30, int u = 43) {
    println(x + y + z + u);
}

acall(10, 5); // The result is: 88 = 10 + 5 + 30 + 43
```

Note: Only the last parameters in a declaration list can be optional.

## Specific Flags: Private & Strict

Functions can also be declared with two specific flags: `private` and `strict`.

### Private

When a function is declared as `private`, it cannot be accessed from outside the current Tamgu file. If a Tamgu program is loaded from another Tamgu program, private functions are unreachable from the loader.

Example:

```cpp
// This function is invisible from external loaders
private function toto(int i) {
    i += 10;
    return i;
}
```

### Strict

By default, when a function is declared in Tamgu, the language tries to convert each argument from the calling function into the parameters expected by the function implementation. However, in some cases, stricter parameter checking is required. The `strict` flag helps solve this problem by demanding strict parameter control for a function.

Example:

```cpp
strict function teststrictdeclaration(int s, int v) {
    println("Int:", s, v);
}

// We declare our variables
string s1 = "s1";
string s2 = "s2";

// In this case, the system will fail to find the right function for these parameters
// and will return an error message
teststrictdeclaration(s1, s2); // No string implementation
```