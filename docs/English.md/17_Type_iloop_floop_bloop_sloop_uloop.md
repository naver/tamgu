# Looping Variables

Looping variables are used to define variables whose values evolve within a given interval. There are five types of looping variables: `iloop`, `floop`, `bloop`, `sloop`, and `uloop`.

## iloop

The `iloop` type loops among integers.

## floop

The `floop` type loops among floats.

## bloop

The `bloop` type loops between `true` and `false`.

## sloop

The `sloop` type loops among strings.

## uloop

The `uloop` type loops among ustrings.

## Initialization

To initialize a loop, you can use a vector or a range.

For example, to initialize an `iloop` variable named `il` that loops between the values 1, 3, 5, 7, and 9, you can use the following syntax:

```cpp
iloop il = [1,3..10];
```

If you initialize a loop with an integer, the value will be considered as a new position within the associated vector. The value 0 will reset the loop to the first element, and the value -1 will reset the loop to the last element.

For example, if you set `il` to 3, the variable will be updated to 7, and the next value will be 9.

```cpp
il = 3; // the variable is now 7. The next value will be 9.
```

## As a Vector

You can return the vector value of a loop by using the `vector` method or by storing its content into a vector.

## Function

You can also associate a function with a loop variable, which will be called when the last value of the initial vector is reached before looping again. The function has the following signature:

```cpp
function callback(loopVar, int pos);
```

Here's an example:

```cpp
iloop i = [1..4];
for (int k in <0,10,1>) {
    print(i," ");
    i++;
}
```

The system will print: `1 2 3 4 1 2 3 4 1 2`.