# Synchronization in Tamgu

Tamgu offers a simple way to put threads in a wait state. The process is very easy to implement. Tamgu provides different functions for this purpose:

## 1. Methods

1. `cast(string)`: This instruction releases the execution of all threads waiting on the specified string.
2. `cast()`: This instruction releases all threads, regardless of their string state.
3. `lock(string s)`: This instruction puts a lock on a portion of code to prevent two threads from accessing the same lines at the same time.
4. `unlock(string s)`: This instruction removes a lock to allow other threads to access the content of a function.
5. `waitonfalse(var)`: This function puts a thread in a wait state until the value of the variable `var` is set to false (or zero, or anything that returns false).
6. `waitonjoined()`: This function waits for threads launched within the current thread to terminate. These threads must be declared with the `join` flag.
7. `wait(string)`: This function puts a thread in a wait state, using a string as a trigger. The wait mode is released when a `cast` is done on that string.

**Example 1:**

```cpp
// We use the string "test" as a trigger
joined thread waiting() {
    wait("test");
    println("Released");
}

// We do some job and then we release our waiting thread
attend();
decompte();
waitonjoined();
printlnerr("End");
```

## 2. Mutex: Lock and Unlock

There are cases when it is necessary to prevent certain threads from accessing the same lines at the same time, for instance, to ensure that two function calls are fully applied before another thread takes control. When a lock is set in a given function, the next lines of this function are no longer accessible to other threads until an unlock is called.

**Example:**

```cpp
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

It is recommended to use very specific strings to differentiate one lock from another.

## 3. Protected threads

The above example could have been rewritten with exactly the same behavior by using a protected function.

**Example:**

```cpp
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

This function will yield exactly the same output as the previous one. Protected threads implement a lock at the very beginning of the execution and release it once the function is terminated. However, the advantage of using lock over a protected function is the possibility to be much more precise on which lines should be protected.

## 4. Semaphores: Waitonfalse

If the above functions are useful in a multi-threaded context, they are not sufficient in some cases. Tamgu provides functions that are used to synchronize threads on variable values. These functions can only be associated with simple types such as Boolean, integer, float, or string.

The role of these two functions is for a thread to wait for a specific variable to reach a false value. False is automatically returned when a numerical variable has the value 0, when a string is empty, or when a Boolean variable is set to false.

`waitonfalse(var)`: This function puts a thread in a wait state until the variable `var` reaches the value false.

The `waitonfalse` function synchronizes the `principal` and `launch` functions together.

**Note:**

The example above could have been implemented with `wait` and `cast` as follows:

```cpp
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

However, it is important to remember that only one `cast` can be performed at a time to release threads. With a synchronous variable, the `waitonfalse` can be triggered by different threads, not just the one that would perform a `cast`.

## 5. Waitonjoined() with flag join

When a thread must wait for other threads to finish before continuing, the simplest solution is to declare each of these threads as `join`, and then use the method `waitonjoined()`.

Different threads can wait on a different set of joined threads at the same time.