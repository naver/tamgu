# Synchronization

Tamgu offers a simple way to put threads in a wait state. The process is very simple to implement. Tamgu provides different functions for this purpose:

## Methods

1. `cast(string)`: This instruction releases the execution of all threads waiting on the specified string.
2. `cast()`: This instruction releases all threads, regardless of their string state.
3. `lock(string s)`: This instruction puts a lock on a portion of code to prevent two threads from accessing the same lines at the same time.
4. `unlock(string s)`: This instruction removes a lock to allow other threads to access the content of a function.
5. `waitonfalse(var)`: This function puts a thread in a wait state until the value of the variable `var` is set to false (or zero, or anything that returns false).
6. `waitonjoined()`: This function waits for threads launched within the current thread to terminate. These threads must be declared with the `join` flag.
7. `wait(string)`: This function puts a thread in a wait state using a string as a trigger. The wait mode is released when a `cast` is done on that string.

### Example 1

```java
// We use the string "test" as a trigger
joined thread waiting() {
    wait("test");
    println("Released");
}

// We do some job and then we release our waiting thread
joined thread counting() {
    int nb = 0;
    while (nb < 10000)
        nb++;
    cast("test");
    println("End counting");
}

waiting();
counting();
waitonjoined();
println("Out");
```

Execution:

If we execute the program above, Tamgu will display in the following order:

```
End counting
Released
Out
```

### Example 2

```java
int nb = 1000;

joined thread decompte() {
    while (nb > 1) {
        nb--;
    }
    printlnerr("End counting", nb);
    nb--;
}

joined thread attend() {
    waitonfalse(nb);
    printlnerr("Ok");
}

attend();
decompte();
waitonjoined();
printlnerr("End");
```

### Mutex: lock and unlock

There are cases when it is necessary to prevent certain threads from accessing the same lines at the same time, for instance, to ensure that two function calls are fully applied before another thread takes control. When a lock is set in a given function, the next lines of this function are no longer accessible to other threads until an unlock is called.

Example:

```java
// We implement our thread
thread launch(string n, int m) {
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
}
```

If we run it, we obtain a display that is quite random, as threads execute in an undetermined order, only known to the kernel.

```
Premier
Second
0 1
0 1 2 3
```

This order can be imposed with locks, which will prevent the kernel from executing the same bunch of lines at the same time.

We must add locks into the code to prevent the system from merging lines in a terrible output:

```java
// We re-implement our thread with a lock
thread launch(string n, int m) {
    lock("launch"); // We lock here, no one can pass anymore
    int i;
    println(n);
    // We display all our values
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
    unlock("launch"); // We unlock with the same string, to allow passage.
}
```

Then, when we run this piece of code again, we will have a completely different output, which is more in line with what we expect:

```
Premier
0 1
Second
0 1 2 3
```

This time the lines will display according to their order in the code.

Important: The lock strings are global to the entire code, which means that a lock somewhere can be unlocked somewhere else. It also means that a lock on a given string might block another part of the code that would use the same string to lock its own lines. It is therefore recommended to use very specific strings to differentiate one lock from another.

### Protected threads

The above example could have been rewritten with exactly the same behavior by using a protected function.

```java
// We re-implement our thread as a protected function
protected thread launch(string n, int m) {
    int i;
    println(n);
    // We display all our values
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}
```

This function will yield exactly the same output as the one above. Protected threads implement a lock at the very beginning of the execution and release it once the function is terminated. However, the advantage of using locks over a protected function is the possibility to be much more precise on which lines should be protected.

### Semaphores: waitonfalse

If the above functions are useful in a multi-threaded context, they are not enough in some cases. Tamgu provides functions that are used to synchronize threads on variable values. These functions can only be associated with simple types such as Boolean, integer, float, or string.

The role of these two functions is for a thread to wait for a specific variable to reach a false value. False is automatically returned when a numerical variable has the value 0, when a string is empty, or when a Boolean variable is set to false.

`waitonfalse(var)`: This function will put a thread in a wait state until the variable `var` reaches the value false.

Example:

```java
// First, we declare a variable stopby
// Important: its initial value must be different from 0
int stopby = 1;

// We implement our thread
thread launch(int m) {
    // We reset stopby with the number of loops
    stopby = m;
    int i;
    // We display all our values
    for (i = 0; i < m; i++) {
        print(i, " ");
        // We decrement our stopby variable
        stopby--;
    }
}

function principal() {
    // We launch our thread
    launch(10);
    // We wait for stopby to reach 0...
    waitonfalse(stopby);
    println("End");
}

principal();
```

The execution will delay the display of "End" until every single `i` has been output on the screen.

```
0 1 2 3 4 5 6 7 8 9 End
```

If we remove the `waitonfalse`, the output will be rather different:

```
End 0 1 2 3 4 5 6 7 8 9
```

As we can see in this example, Tamgu will first display the message "End" before displaying any other values. The `waitonfalse` synchronizes `principal` and `launch` together.

Note: The example above could have been implemented with `wait` and `cast` as below:

```java
// We implement our thread
thread launch(int m) {
    int i;
    // We display all our values
    for (i = 0; i < m; i++)
        print(i, " ");
    cast("end");
}

function principal() {
    // We launch our thread
    launch(10);
    wait("end");
    println("End");
}

principal();
```

However, one should remember that only one `cast` can be performed at a time to release threads. With a synchronous variable, the `waitonfalse` can be triggered by different threads, not just the one that would perform a `cast`.

### `waitonjoined()` with flag `join`

When a thread must wait for other threads to finish before continuing, the simplest solution is to declare each of these threads as `join` and then use the method `waitonjoined()`.

Different threads can wait on a different set of joined threads at the same time.

Example:

```java
// A first thread with a join
join thread jdisplay(string s) {
    print(s + "\r");
}

// Which is launched from this thread also "join"
join thread launch(int x) {
    int i;
    for (i = 0; i < 5000; i++) {
        string s = "Thread:" + x + "=" + i;
        jdisplay(s);
    }
    // We wait for our local threads to finish
    waitonjoined();
    println("End:" + x);
}

// We launch two of them
launch(0);
launch(1);
// And we wait for them to finish...
waitonjoined();
println("Termination");
```