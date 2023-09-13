## Creation within the constructor

In the previous section, we learned that it is possible to create a frame element by either initializing it directly in the frame field list or within the constructor itself. When the frame element is constructed in the constructor, a simple declaration is sufficient; any other declaration would be redundant.

For example:

```cpp
// This frame declares a specific "within" frame
frame test {
    int i;
    // In this case, we postpone the actual creation of the element to the constructor: _initial
    within w;
    // Our function _initial for that frame...
    function _initial(int k) {
        i = k;
        // We replace the previous description with a new one
        w = within(100);
        println("test _initial", k);
    }
}

// We create a test instance: t1 with an initial value of 20
test t1(20);
```

**Important**

If constructor parameters are required for "w" and no creation of that element "w" is done in the constructor, then Tamgu will yield an error about missing parameters.