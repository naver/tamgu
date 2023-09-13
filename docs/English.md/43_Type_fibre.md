# 43 Type Fibre

A fibre is a lightweight thread of execution. Like threads, fibres share the same address space. However, fibres use cooperative multitasking while threads use preemptive multitasking. Threads often rely on the kernel's thread scheduler to preempt a busy thread and resume another thread, whereas fibres yield themselves to allow another fibre to run while executing.

In Tamgu, fibres are implemented on top of Taskell functions. These functions must contain an iteration in order to be executed as fibres since there is no actual yield function.

The `fibre` type exposes the following methods:

1. `run()`: Executes the fibres that have been recorded.
2. `block()`: Fibres are stored in a linked chain that is iterated from the beginning to the end. When a new fibre is appended to this list, it becomes the new terminal element. `block()` is used to define the current terminal element of that list as the iteration boundary. New fibres can still be appended, but they will not be executed until `unblock()` is called. If `block()` is called again with a different current terminal element, then the previous boundary is moved to this new terminal element.
3. `unblock()`: Releases the limit on iteration.

## Declaring a Fibre

To declare a fibre, you first need to implement a Taskell function and then store this Taskell function into a fibre variable.

```cpp
<Myfunc(a1, a2, ..., an) = ...>
fibre f = MyFunc;
```

To record a new fibre, you simply call it with some parameters:

```cpp
f(p1, p2, ..., pn);
```

If `MyFunc` does not contain an iteration, it will be automatically executed. If you need to store some output, you can use the stream operator.

```cpp
vector v;
v.push(ivector());
v[-1] <<< f(200, 210);
```

## Ending a Fibre: `break` or End of Iteration

There are two ways to end a fibre: either the iterator reaches an end or the `break` instruction is called.

```cpp
<Myfunc(x, y) : if (x == 10) break else (x + 1) ...>
```

## Threads

Fibres can be executed within a thread, but only one fibre can execute the `run()` method at a time in one thread. However, threads can record new fibres in parallel without any problems.

## Example

```cpp
vector v;
int i;

// We initialize a vector of integers to store the results...
for (i in <3>)
    v.push(ivector());

//-----------------------------------------------------------------------
function compute(int i, int x, int y) {
    return i + x + y;
}

<myfiber(x, y) = compute(i, x, y) | i <- [x..y]>

// We initialize a fibre variable with myfiber
fibre f = myfiber;

//-----------------------------------------------------------------------
// Recording is done from a thread...
joined thread recording(int i, int x, int y) {
    println("Fiber:", i);
    v[i] <<< f(x, y);
}

function running() {
    f.run();
}

//-----------------------------------------------------------------------
// The fibres are launched from a thread...
int n;

for (i in <0, 60, 20>) {
    recording(n, i, i + 19);
    n++;
}

// We wait for all fibres to record
waitonjoined();

//-----------------------------------------------------------------------
// We execute them...
running();

println(v);
```

**Result:**

```
Fiber: 0
Fiber: 2
Fiber: 1
[[19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38],
[79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98],
[139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158]]
```