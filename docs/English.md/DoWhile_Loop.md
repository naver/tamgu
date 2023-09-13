# Do-While Loop

The `do-while` loop is a control flow statement in programming languages that executes a block of code repeatedly until a specified condition is false. 

The syntax for the `do-while` loop is as follows:

```
do {
    // code to be executed
} while (condition);
```

The key difference between the `do-while` loop and the `while` loop is that the `do-while` loop executes the code block at least once, even if the condition is false from the beginning.

## Example

Here's an example of using the `do-while` loop in Java:

```java
int i = 10;
do {
    System.out.println("I = " + i);
    i -= 1;
} while (i > 0);
```

In this example, the loop will execute as long as the value of `i` is greater than 0. The output will be:

```
I = 10
I = 9
I = 8
I = 7
I = 6
I = 5
I = 4
I = 3
I = 2
I = 1
```

As you can see, the code block is executed before the condition is checked.