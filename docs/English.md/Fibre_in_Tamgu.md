# Fibre in Tamgu

A fiber is a lightweight thread of execution. Like threads, fibers share the same address space. However, fibers use cooperative multitasking while threads use preemptive multitasking. Threads often rely on the kernel's thread scheduler to preempt a busy thread and resume another thread, whereas fibers yield themselves to run another fiber while executing.

In Tamgu, fibers are implemented on top of Taskell functions. These functions must contain an iteration in order to be executed as fibers, as there is no actual yield function.

The `fibre` type exposes the following methods:

1. `run()`: Executes the fibers that were recorded.
2. `block()`: Fibers are stored in a linked chain, which is iterated from the beginning to the end. When a new fiber is appended to this list, it becomes the new terminal element. `block()` is a way to define the current terminal element of that list as the iteration boundary. New fibers can still be appended, but they will not be executed until `unblock()` is called. If `block()` is called again with a different current terminal element, then the previous boundary is moved to this new terminal element.
3. `unblock()`: Releases the limit on iteration.

To declare a fiber, you first need to implement a Taskell function and then store this Taskell function into a fiber variable.

```tamgu
<MyFunc(a1, a2, ..., an) = ....>
fibre f = MyFunc;
```

To record a new fiber, you simply call it with some parameters:

```tamgu
f(p1, p2, ..., pn);
```

If `MyFunc` does not contain an iteration, it will be automatically executed. If you need to store some output, you can use the stream operator.

```tamgu
vector v;
v.push(ivector());
v[-1] <<< f(200, 210);
```

Ending a fiber can be done in two ways: either the iterator reaches an end, or the instruction `break` is called:

```tamgu
<MyFunc(x, y) : if (x == 10) break else (x + 1)...>
```

Fibers can be executed within a thread, but only one fiber can execute `run()` at a time in one thread. However, threads can record new fibers in parallel without any problems.

## Example

```tamgu
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
// We initialize a fiber variable with myfiber
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
// The fibers are launched from a thread...
int n;
for (i in <0, 60, 20>) {
    recording(n, i, i + 19);
    n++;
}
// We wait for all fibers to record
waitonjoined();
//-----------------------------------------------------------------------
// We execute them...
running();
```