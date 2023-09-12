## Using a Frame

A frame object is declared using the frame's name as its type.

### Example

```cpp
myframe first; // creating the first instance
mysubframe subfirst; // creating a sub-frame instance

// We can create a new instance
first = myframe(); // equivalent to "new myframe" in C++ or Java

// To execute a frame's function
myframe.display();
```