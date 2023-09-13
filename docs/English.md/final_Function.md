# _final Function

The `_final` function is called whenever a frame object is deleted. Usually, an object that is declared in a function or loop is deleted once that function or loop ends.

## Important

- This function has no parameters.
- Calling this function does not delete the object.
- The content of this function cannot be debugged as it is called from within the destructor, independently from the rest of the code.

## Example

```cpp
frame myframe {
    int i = 10; // every new frame will instantiate i with 10
    string s = "initial";

    function _initial(int ij) {
        i = ij;
    }

    function _final() {
        print("IN MYFRAME: " + s + "\n");
    }
}

int i = 10;
while (i >= 0) {
    // A new instance of myframe is created:
    // At the end of each iteration, the _final function will be called
    myframe item(i);
    i--;
}
```

Note: This text has been converted into a GitHub README.md file.