# Thread

When a thread function is launched, it is executed as an independent system thread.

Example:

```cpp
int toto(int i) {
    i += 10;
    return i;
}

int j = toto(10);
print("J=" + j + "\n");
```

Execution:

```
J=20
```

## Protected Thread

"Protected" prevents two threads from accessing the same lines of code at the same time.

A protected thread sets a lock at the beginning of its launch, which is released once the function is executed. This ensures that different calls to a protected function will be done in sequence and not at the same time. Protected should be used for code that is not reentrant.

Example:

```cpp
protected thread launch(string n, int m) {
    int i;
    println(n);
    for (i = 0; i < m; i++)
        print(i + " ");
    println();
}

function principal() {
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

## Exclusive Thread

Exclusive is very similar to protected, with one difference. In the case of protected, the protection is at the method level, while with exclusive it is at the frame level. In this sense, exclusive works exactly as synchronized in Java.

In the case of a protected function, only one thread can have access to this method at a time. On the other hand, if a method is exclusive, only one thread can have access to the frame object at a time. This means that different threads can execute the same method if this method is executed within different instances.

In other words, in a protected thread, we use a lock that belongs to the method, while in an exclusive thread, we use a lock that belongs to the frame instance.

Example:

```cpp
frame disp {
    exclusive thread edisplay(string s) {
        println("Exclusive: " + s);
    }

    protected thread pdisplay(string s) {
    }
}

for (i = 0; i < 100; i++)
    v[i].pdisplay();

for (i = 0; i < 100; i++)
    v[i].edisplay();

for (i = 0; i < 100; i++)
    v[i].display(c);
```

## Joined and Waitonjoined

A thread can be declared as joined if the main thread is supposed to wait for the completion of all the threads that were launched before completing its own code. You can use waitonjoined() to wait for these threads to finish.

You can use as many waitonjoined() as necessary in different threads. waitonjoined() only waits on "joined threads" that were launched within a given thread.

## Atomic Values

Most data structures (maps, vectors, strings, etc.) are protected in threads against concurrent access through a lock. However, these locks are often costly in terms of time and space. Tamgu provides some specific lock-free data structures (or atomic types) such as a_int, a_string, or a_mapii. These data structures allow for a much more efficient use of the machine since when accessing these types, the threads are not put on hold. However, their implementation makes them slower than other types in a non-threading context. Their use is only useful when they are shared across threads.

## Stream Operator '<<<'

When you launch a thread, the result of that thread cannot be directly stored in a variable with the operator "=", since a thread lives its own life without any links to the calling code. Tamgu provides a specific operator for this task: <<<, also called the stream operator. A stream is a variable that is connected to the thread in such a way that the values returned by the thread can be stored within that variable. Of course, the variable must exist independently from the thread.

Example:

```cpp
joined thread Test(int i) {
    return (i * 2);
}

function launch() {
    treemapi m;
    int i = 0;
    for (i in <0, 10, 1>)
        m[i] <<< Test(i);
    waitonjoined();
    println(m); //{0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```