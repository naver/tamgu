## Random Number: random(), a_random()

Tamgu provides functions to generate random numbers. The `random()` function returns a random value between 0 and 99. It returns a long value. You can also provide a maximum boundary value as an argument.

The `a_random()` function is a lock-free version of `random()`.

### Example:

```cpp
float rd = random(); // value between 0 and 99
rd = random(999); // value between 0 and 999
```