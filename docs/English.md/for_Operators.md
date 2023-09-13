# "for" Operators

There are different flavors of "for" loops in Tamgu. Here is an overview of each type.

## for (expression; boolean; next) { ... }

This type of "for" loop consists of three parts: initialization, a boolean expression, and a continuation part. You can use "continue" or "break" to either move to the next element or break out of the loop.

**Example:**
```cpp
for (i = 0; i < 10; i += 1) {
    print("I=", i, "\n");
}
```

## Multiple Initializations and Increments

Expressions in the initialization and increment parts can contain more than one element. In this case, the elements should be separated by a comma.

**Example:**
```cpp
int i, j;
// Multiple initializations and multiple increments.
for (i = 10, j = 100; i > 5; i--, j++) {
    println(i, j);
}
```

## for (var in container) { ... }

This type of "for" loop is used to iterate over a container, such as a string or a file.

**Example:**
```cpp
// Looping through a file.
file f('myfile.txt', "r");
string s;
for (s in f) {
    println(s);
}

// Looping through a vector of ints.
vector v = [1, 2, 3, 4, 5, 6];
int i;
for (i in v) {
    println(l);
}
```

## for (i in <start, end, increment>): Fast loop

This loop is equivalent to: `for (i = start; i < end; i += increment) ...`. It can also be equivalent to `for (i = start; i > end; i -= increment)` if the increment is negative. This loop is implemented in C++ and is approximately 30% to 50% faster than its equivalent.

Note that once the loop has started, no element can be modified, including the variable that receives the different values.

**Example:**
```cpp
int i, j = 1;
int v;
time t1;

// Looping from 0 to 100000 with an increment of 1.
for (i in <0, 100000, j>) {
    v = i;
}

time t2;
float diff = t2 - t1;
println("Elapsed time for fast 'for':", diff);

time t3;
for (i = 0; i < 100000; i += j) {
    v = i;
}

time t4;
diff = t4 - t3;
println("Elapsed time for regular 'for':", diff);
```

## Local Declarations

You can also declare variables within a "for" statement that are only accessible within the scope of the loop.

**Example:**
```cpp
for (int i in <0, 100000, j>) {
    println(i);
}

for (int i = 0; i < 10; i++) {
    println(i);
}
```

## Default Arguments

You can omit some of the arguments in the "for" loop for more concise code.

- `for (k in <100>)` means that the initial value is 0 and the increment is 1.
- `for (k in <1, 100>)` means that the increment is 1 by default.