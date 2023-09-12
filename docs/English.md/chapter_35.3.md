# While Loop

The `while` loop is composed of a single Boolean expression. It allows you to repeatedly execute a block of code as long as the Boolean expression evaluates to `true`.

## Syntax

```java
while (boolean) {
    // code to be executed
}
```

## Usage

You can use the `continue` statement to skip to the next iteration of the loop, or the `break` statement to exit the loop prematurely.

## Example

```java
int i = 10;
while (i > 0) {
    System.out.println("I = " + i);
    i--;
}
```

In this example, the loop will continue to execute as long as the value of `i` is greater than 0. The value of `i` is printed at each iteration and decremented by 1.