## Initialization Order

When items are declared within a frame, the call to the `_initial` function is done from top to bottom, starting with the parent frame and then its children.

Furthermore, if an item within a frame F is instantiated within the `_initial` function of that frame F, then this declaration takes precedence over any other declarations.

### Example

```java
// We declare two frames
frame within {
    int i;
    
    // With a specific constructor function
    function _initial(int j) {
        i = j * 2;
        println("within _initial", i);
    }
}

// Our function _initial for that frame...
function _initial(int k) {
    i = k;
    // We replace the previous description with a new one
    // This declaration subsumes the other one above
    w = within(100);
    println("test _initial", k);
}

// We create a test instance: t1 with an initial value of 20
test t1(20);
```

### Execution

The execution yields the following result:

```
test _initial 20
within _initial 200
```

As we can see from this example, the explicit initialization of "w" in `_initial` replaces the declaration "within w(i);" which becomes redundant.