# Tamgu Specific Instructions

Tamgu provides all the necessary operations to handle various algorithms, including if, else, elif, switch, for, and while statements. It also supports boolean operators such as and (&&), or (||), and xor. These boolean operators combine different elements with a boolean value. The and and or operators can also be written as && and ||, respectively. 

- The `and` operator succeeds if all elements are true.
- The `or` operator succeeds if at least one element is true.
- The `xor` operator succeeds if at least one element is true but fails if they are all true.

## if-elif-else Statements

The if-elif-else statements in Tamgu are used to perform conditional branching. The syntax is as follows:

```python
if (boolean_expression) {
    // code block
}
elif (boolean_expression) {
    // code block
}
...
else {
    // code block
}
```

## ifnot Statement

The `ifnot` statement in Tamgu is used to chain different instructions and returns the value of the first instruction that returns anything but false, null, or empty. It is commonly used to handle complex sequences of operations. 

Example:

```python
map m1 = { "a": 1, "b": 2, "c": 3 };
map m2 = { "A": 4, "B": 5, "C": 6 };
map m3 = { "aa": 7, "bb": 8, "cc": 9 };
int i = (m1["A"] ifnot m2["B"] ifnot m3["bb"]) + 24;
println(i); // Output: 29

function calling(string s) {
    return (m1[s] ifnot m2[s] ifnot m3[s]);
}

println(calling('bb')); // Output: 8
```

## switch Statement

The `switch` statement in Tamgu is used to list a series of tests for a single object. The syntax is as follows:

```python
switch (expression) (with function) {
    v1: {
        // code block
    }
    v2: {
        // code block
    }
    default: {
        // code block
    }
}
```

The `v1`, `v2`, etc. can be either a string or an integer or a float. The expression is evaluated once and compared with `v1`, `v2`, etc. It is also possible to replace the simple comparison between the elements with a call to a function, which should return `true` or `false`.

Example:

```python
// We test whether one value is larger than the other
function tst(int i, int j) {
    if (j >= i)
        return true;
    return false;
}

int s = 10;

// We test through test
switch (s) with tst {
    1: println("1");
    2: println("2");
    20: println("20"); // This will be the selected occurrence
}
```

## for Statements

There are different flavors of `for` loops in Tamgu. Here is a presentation of each of them.

### `for (expression; boolean; next)`

This `for` loop is composed of three parts: an initialization, a boolean expression, and a continuation part. The syntax is as follows:

```python
for (expression; boolean; next) {
    // code block
}
```

You can use `continue` or `break` to either go to the next element or break in the middle of a loop.

Example:

```python
for (i = 0; i < 10; i += 1)
    print("I=", i, "\n");
```

### `for (var in container)`

This type of `for` loop is used to iterate over a container, such as a string or a file. The syntax is as follows:

```python
for (var in container) {
    // code block
}
```

Example:

```python
// Looping through a file
file f('myfile.txt', "r");
string s;
for (s in f)
    println(s);

// Looping through a vector of ints
vector v = [1, 2, 3, 4, 5, 6];
int i;
for (i in v)
    println(l);
```

### `for (i in <start, end, increment>)`

This type of `for` loop is a fast loop that is about 30% to 50% faster than its equivalent. Each value in the range can be instantiated through variables. However, once the loop has started, no element can be modified, including the variable that receives the different values. The syntax is as follows:

```python
for (i in <start, end, increment>) {
    // code block
}
```

Example:

```python
int i, j = 1;
int v;
time t1;

// Looping from 0 to 100000, with an increment of 1.
for (i in <0, 100000, j>)
    v = i;

time t2;
float diff = t2 - t1;
println("Elapsed time for fast 'for':", diff);

time t3;
for (i = 0; i < 100000; i += j)
    v = i;

time t4;
diff = t4 - t3;
println("Elapsed time for regular 'for'", diff);
```

## while Statement

The `while` statement in Tamgu is composed of a single boolean expression. The syntax is as follows:

```python
while (boolean) {
    // code block
}
```

You can use `continue` or `break` to either go to the next element or break in the middle of a loop.

Example:

```python
int i = 10;
while (i > 0) {
    print("I=", i, "\n");
    i -= 1;
}
```

## do-while Statement

The `do-while` statement in Tamgu is similar to `while`, but the first iteration is done before the boolean test. The syntax is as follows:

```python
do {
    // code block
}
while (boolean);
```

Example:

```python
int i = 10;
do {
    print("I=", i, "\n");
    i -= 1;
}
while (i > 0);
```

## Universally Unique Identifier: `uuid()`

The `uuid()` function in Tamgu returns a UUID (Universally Unique Identifier) in agreement with UUID RFC 4122, version 4 (fully random). It returns a string value containing hexadecimal values.

Example:

```python
string s = uuid(); // Example output: 80c67c4d-4c4d-14cd-e58d-eb3364cd
```

## Evaluation: `eval(string code)`

The `eval()` function in Tamgu can evaluate and run Tamgu code on the fly. The result of the evaluation is returned according to what was evaluated.

## JSON Evaluation: `evaljson(string code)`

The `evaljson()` function in Tamgu takes a JSON string as input and compiles it into either a vector or a map according to the inner structure.

## Printing Instructions

Tamgu provides several printing instructions to display results on the current display port. The `print` and `println` instructions are used to display results, while the `printerr` and `printlnerr` instructions display the results on the standard error output. The `println` and `printlnerr` versions add two features to the output: an additional space is added for values separated with a `,`, and a carriage return is added at the end of the line.

## Join Printing Instructions

The `printj`, `printjln`, `printjerr`, and `printjlnerr` instructions are used to display container values that are "joined" beforehand. They accept two, three, or four arguments. The first parameter should be a container, and the second one should be a separator string. If the container is a map, a key separator can also be supplied as the third parameter. A fourth numerical parameter can be provided to add a carriage return every n values.

Example:

```python
ivector v = [1..10];
printj(v, "-"); // Output: 1-2-3-4-5-6-7-8-9-10

map m = {1: 2, 2: 3, 4: 5, 6: 7};
printjln(m, ",", "-"); // Output: 1-2,2-3,4-5,6-7
```

## IO Redirect Functions

The `ioredirect` and `iorestate` functions are used to capture the output from stderr or stdout into a file. The `ioredirect` function redirects either stderr (if `err` is true) or stdout (if `err` is false) to a file. It returns an ID, which is used to set the output back to normal using the `iorestate` function.

Example:

```python
int o = ioredirect('C:\Tamgu\test\test.txt', true);
printlnerr("This string is now stored in file: test.txt");
iorestate(o, true); // Back to normal
```

## Pause and Sleep Functions

The `pause` and `sleep` functions are used to put a thread in pause or sleep mode. The `pause` function does not suspend the execution of a thread, while the `sleep` function does. The `pause` function takes a float as input, representing the pause duration in seconds. It can also take a second boolean parameter to display a small animation. The `sleep` function is based on the OS sleep instruction and its behavior depends on the local implementation. It takes an integer as input, representing the sleep duration in seconds.

Example:

```python
pause(0.1); // The thread will pause for 10 ms
pause(2, true); // The thread will pause for 2s, with a small animation
sleep(1); // The thread will sleep for 1s (depending on the platform)
```

## Emojis Function

The `emojis()` function returns a list of all emoji characters according to the norm v5.0 beta.

## GPS Distance Function

The `GPSdistance` function in Tamgu is used to compute the distance between two GPS points given their longitude and latitude. The fifth parameter is optional and represents the Earth radius, with a default value of 6371 km. This value also defines the unit used to compute the final distance.

Example:

```python
float d = GPSdistance(46.9001, 7.1201, 47.01, 6.1);
// Output: d is 78.382 km
```

## Read from Keyboard: `kget()`

The `kget()` function in Tamgu is used to read a string from the keyboard.

Example:

```python
string message = kget("Message:");
```

## Persistent Variables

You can create persistent variables in the context of a GUI to keep track of certain values when launching your programs repeatedly. The following persistent types are available: `ithrough`, `fthrough`, `uthrough`, `sthrough`, `vthrough`, and `mthrough`. These variables are never reinitialized between runs.

Example:

```python
// This variable will keep track of the number of times this program was run
ithrough icount;
icount += 1;
println(icount);
```