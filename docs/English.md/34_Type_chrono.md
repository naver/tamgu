# Type Chrono

The `Chrono` type is similar to `Time`, but it is implemented based on `std::chrono::high_resolution_clock`. It provides the same `reset` method, but allows for more precise time computation.

When creating a `Chrono` object, you need to specify how the duration should be handled using the following flags:
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

The default value is `c_second`.

## Example

```cpp
Ring r;
Chrono c1(c_nanosecond);
for (int i = 0; i < 100000; i++) {
    r.pushLast(i);
}
Chrono c2(c_nanosecond);
float d;
d = c2 - c1; // in nanoseconds
println(d);
```

In this example, a `Ring` object `r` is created, and a `Chrono` object `c1` is created with a duration of `c_nanosecond`. Then, a loop is executed to push values into the ring. After that, another `Chrono` object `c2` is created with the same duration. The difference between `c2` and `c1` is calculated and stored in `d`, representing the elapsed time in nanoseconds. Finally, the value of `d` is printed.