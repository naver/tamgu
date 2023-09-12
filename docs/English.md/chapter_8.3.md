## 10.2 - Initial Function

A creator function can be associated with a frame. This function is automatically called when a new instance of that frame is created.

### Example

```cpp
frame myframe {
    int i = 10; // every new frame will instantiate i with 10
    string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    function display() {
        print("IN MYFRAME: " + s + "\n");
    }
}

// A new instance of myframe is created:
myframe second(10); // the parameters are then passed to the _initial function, similar to C++
```

In this example, we have a frame called `myframe` with an initial function `_initial`. When a new instance of `myframe` is created, the `_initial` function is called with the provided parameters. The `display` function prints out the value of `s` within the frame.