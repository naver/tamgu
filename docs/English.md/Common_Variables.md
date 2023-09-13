## Common Variables

Tamgu provides a straightforward method for declaring class variables. A class variable is a variable whose value is shared among all instances of a given frame.

### Example

```cpp
frame myframe {
    common int i; // Every frame will have access to the same common instance of that variable.
}

myframe t1;
myframe t2;

t1.i = 10;
t2.i = 15;

println(t1.i, t2.i); // Display for both variables: 15 15
```