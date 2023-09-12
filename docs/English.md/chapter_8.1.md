# 10 Frame in Tamgu

A frame is a class description used to declare complex data structures along with functions. 

- Member variables can be instantiated within the frame.
- A method `_initial` can be declared, which will be automatically called for each new instance of this frame.
- A sub-frame can be declared within the frame body, which automatically inherits the methods and variables from the top frame.
- Redefinition of a function is possible within a sub-frame.
- Private functions and variables can also be declared within a frame.

**Example:**

```tamgu
frame myframe {
    int i = 10; // every new frame will instantiate i with 10
    string s = "initial";
    
    function display() {
        print("IN MYFRAME: " + s + "\n");
    }
    
    frame mysubframe {
        function display() {
            print("IN MYSUBFRAME: " + s + "\n");
        }
    }
}
```