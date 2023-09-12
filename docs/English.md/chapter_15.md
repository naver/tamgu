# Looping Variables in Tamgu

In Tamgu, there are five types of looping variables used to define variables whose values evolve within an interval. These variables are initialized with a vector definition and can be incremented to jump to the next value. When they reach the end of the interval, they start again from the beginning.

## Types of Looping Variables

1. `iloop`: Loops among integers.
2. `floop`: Loops among floats.
3. `bloop`: Loops between true and false.
4. `sloop`: Loops among strings.
5. `uloop`: Loops among ustrings.

## Initialization

To initialize a loop, you can use a vector or a range. For example:

```tamgu
iloop il = [1, 3..10];
```

In the above example, the variable `il` will loop between the values 1, 3, 5, 7, and 9.

If you initialize a loop with an integer, it will be considered as a new position within the associated vector. The value 0 resets the loop to the first element, and the value -1 resets the loop to the last element. For example:

```tamgu
il = 3; // the variable is now 7. The next value will be 9.
```

## Using Looping Variables as Vectors

You can return the vector value of a loop by using the `vector` method or by storing its content in a vector.

## Function Callback

You can also associate a function with a loop variable, which will be called when the last value of the initial vector is reached before looping again. The function should have the following signature:

```tamgu
function callback(loopVar, intPos);
```

Here's an example:

```tamgu
iloop i = [1..4];
for (int k in <0, 10, 1>) {
    print(i, " ");
    i++;
}
```

The system will print: `1 2 3 4 1 2 3 4 1 2`.