## Sub-framing or Enriching a Frame

Tamgu enables programmers to enrich or sub-frame an existing frame. A frame description can be implemented in a few steps. For instance, one can start with an initial description and later decide to enrich it in the program.

### Enriching

```cpp
// We start with a limited definition of a frame...
frame myframe {
    int i = 10; // every new frame will instantiate i with 10
}

// We add some code here...
...

// Then we enrich this frame with some more code
// All we need to do is use the same frame instruction as above, adding some new stuff
frame myframe {
    function display() {
        println(i);
    }
}
```

### Sub-frames

A sub-frame is a frame that is declared within another frame (the parent frame), from which it inherits both variables and functions. A sub-frame can replace functions from the parent frame with new versions and add its own variables.

```cpp
// If we want to add some sub-frames...
frame myframe {
    // We can now add our sub-frame...
    frame subframe { ... }
}
```

### Using Upper Definition: frame::function

If you need to use the definition of the parent frame instead of the current thread, Tamgu provides a mechanism that is very similar to other languages such as C++ or Java. The function name must be preceded with the frame name followed by "::".

**Example:**

```cpp
// Calling subframes...
// We define a test frame in which we define a subtest frame
frame test {
    int i;

    function _initial(int k) {
        i = k;
    }

    function display() {
        println("In test", i);
    }

    frame subtest {
        string x;

        function display() {
            println("In subtest", i);
            test::display(); // will call the other display definition from test
        }
    }
}

// We create two objects
test t(1);
subtest st(2);

// We then call the different methods
t.display(); // display: "In test, 1"
st.display(); // display: "In subtest, 2" and "In test, 2"
st.test::display(); // display: "In test, 2"
```