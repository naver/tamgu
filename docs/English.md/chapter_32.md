# Chrono in Tamgu

The `chrono` type in Tamgu is similar to `time` but is implemented based on `std::chrono::high_resolution_clock`. It provides the same `reset` method but offers more precise time computation.

When creating a `chrono` object, you need to specify the duration handling using the following flags:
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

The default value is `c_second`.

Example usage:

```cpp
ring r;
chrono c1(c_nanosecond);

for (int i = 0; i < 100000; ++i) {
    r.pushlast(i);
}

chrono c2(c_nanosecond);
float d;
d = c2 - c1; // in nanoseconds

println(d);
```

In this example, a `chrono` object `c1` is created with duration handling set to `c_nanosecond`. The `ring` `r` is then populated with integers. Another `chrono` object `c2` is created with the same duration handling. The difference between `c2` and `c1` is computed and stored in `d`, representing the time elapsed in nanoseconds. Finally, `d` is printed.