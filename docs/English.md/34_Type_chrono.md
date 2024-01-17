# Types: chrono and clock

The `chrono` type is similar to `time`, but it is implemented based on `std::chrono::high_resolution_clock`. It provides the same `reset` method, but allows for more precise time calculation.

When creating a `Chrono` object, you must specify how the duration should be handled using the following indicators:
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

The default value is `c_second`.

## Methods

1. `reset()`: Resets the clock.
2. `start()`: Starts the chrono.
3. `stop()`: Stops the chrono and returns the intermediate time.
4. `unit(int i)`: 1 for _second_, 2 for _milliseconds_, 3 for _microseconds_, 4 for _nanoseconds_.

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

In this example, a `Ring` object `r` is created, and a `Chrono` object `c1` is created with a duration of `c_nanosecond`. Then, a loop is executed to add values to the ring. After that, another `Chrono` object `c2` is created with the same duration. The difference between `c2` and `c1` is calculated and stored in `d`, representing the elapsed time in nanoseconds. Finally, the value of `d` is displayed.

# Type clock

The `clock` type is similar to `chrono`, but it is implemented based on `std::chrono::system_clock`. This type in particular provides additional methods for handling UTC time.

## Methods

Added to the methods of `chrono`:

1. `utc()`: Returns the clock in UTC format.
2. `utc(string fmt)`: Initializes the clock from a string in UTC format.
3. `timezone(string tm)`: Initializes the time zone with a string of the type: "+|-dd:dd".
4. `format(string fmt)`: Returns a string in the fmt format. An additional `%fm` tag is also added to return the milliseconds.
5. `milliseconds()`: Returns the milliseconds.

## Initialization

When creating a `clock` object, you might provide a time zone argument in the form of a string: "+|-dd:dd".

### Example

```cpp
clock d("-05:30");
```
