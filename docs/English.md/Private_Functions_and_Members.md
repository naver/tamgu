## Private Functions and Members

In a frame, certain functions or variables can be declared as private. A private function or variable can only be accessed from within the frame.

### Example

```cpp
frame myframe {
    int i = 10; // every new frame will instantiate i with 10
    
    // private variable
    private string s = "initial";
    
    function _initial(int ij) {
        i = ij;
    }
    
    // private function
    private function modify(int x) {
        i = x;
        s = "Modified with: " + x; // you can modify "s" here
    }
    
    function display() {
        modify(1000); // you can call "modify" here
        print("IN MYFRAME: " + s + "\n");
    }
}

myframe test;

// Illegal instructions on private frame members...
test.modify(100); // this instruction is illegal as "modify" is private
println(test.s); // this instruction is illegal as "s" is private
```

In the above example, the `myframe` frame is defined with a private variable `s` and a private function `modify`. These private members can only be accessed within the `myframe` frame.

Attempting to access the private function `modify` or the private variable `s` outside of the `myframe` frame, like in the `test` object, will result in illegal instructions.