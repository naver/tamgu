# Persistent Variables

In a GUI context, you have the option to create persistent variables that can track certain values when launching your programs repeatedly. There are different types of persistent variables available: `ithrough`, `fthrough`, `uthrough`, `sthrough`, `vthrough`, and `mthrough`. You can declare a variable with any of these types to keep track of different experiments. These variables are never reinitialized between runs.

## Example

```java
// This variable will keep track of the number of times this program was run
ithrough icount;
icount += 1;
println(icount);
```