# Frame

The `Frame` class is used to declare complex data structures along with functions. It provides the following features:

- Member variables can be instantiated within the frame.
- A method `_initial` can be declared, which will be automatically called for each new instance of this frame.
- A sub-frame can be declared within the frame body, which automatically inherits the methods and variables from the top frame.
- Redefinition of a function is possible within a sub-frame.
- Private functions and variables can also be declared within a frame.

## Example

```cpp
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

## Using a frame

A frame object is declared with the name of its frame as a type. 

Example:

```cpp
myframe first; // create a first instance
mysubframe subfirst; // create a sub-frame instance

// We can recreate a new instance
first = myframe; // equivalent to "new myframe" in C++ or Java

// To run a frame's function
myframe.display();
```

## `_initial` function

A creator function can be associated with a frame. This function is automatically called when a new instance of that frame is created.

Example:

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

// A new instance of myframe is created
myframe second(10); // the parameters are then passed to the _initial function as in C++
```

## `_final` function

The `_final` function is called whenever a frame object is deleted. Usually, an object that is declared in a function or in a loop is deleted once this function or loop ends.

Important:
- This function has no parameters.
- A call to that function does not delete the object.
- The content of this function cannot be debugged as it is called from within the destructor, independently from the rest of the code.

Example:

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
    // A new instance of myframe is created
    // At the end of each iteration, the _final function will be called
    myframe item(i);
    i--;
}
```

## Initialization Order

When items are declared within a frame, the call to the `_initial` function is done from the top down to its children. Furthermore, if an item within a frame F is instantiated within the `_initial` function of that frame F, then this declaration takes precedence over any other declarations.

Example:

```cpp
// We declare two frames
frame within {
    int i;
    
    // With a specific constructor function
    function _initial(int j) {
        i = j * 2;
        println("within _initial", i);
    }
}

// This frame declares a specific "within" frame
frame test {
    int i;
    
    // In this case, we declare a specific frame, whose declaration depends on the variable i
    within w(i);
    
    // Our function _initial for that frame...
    function _initial(int k) {
        i = k;
        println("test _initial", k);
    }
}

// We create a test instance: t1 with an initial value of 20
test t1(20);
```

## Creation within the constructor

It is possible to create a frame element by either declaring its initialization directly into the frame field list or within the constructor itself. When the frame element construction is made in the constructor, a simple declaration suffices; any other declaration would be redundant.

Example:

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
        // This declaration subsumes the other one above
        w = within(100);
        println("test _initial", k);
    }
}

// We create a test instance: t1 with an initial value of 20
test t1(20);
```

Important:
If constructor parameters are required for "w" and no creation of that element "w" is done in the constructor, then Tamgu will yield an error about missing parameters.

## Common Variables

Tamgu provides a simple way to declare class variables. A class variable is a variable whose value is shared across all instances of a given frame.

Example:

```cpp
frame myframe {
    common int i; // every frame will have access to the same common instance of that variable
}

myframe t1;
myframe t2;
t1.i = 10;
t2.i = 15;
println(t1.i, t2.i); // display for both variables: 15 15
```

## Private Functions and Members

Certain functions or variables can be declared as private in a frame. A private function or a private variable can only be accessed from within the frame.

Example:

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

## Sub-framing or Enriching a Frame

Tamgu enables the programmer to enrich or sub-frame an existing frame. A frame description can be implemented in a few steps. For instance, one can start with a first description and then decide to enrich it later in the program.

### Enriching

```cpp
// We start with a limited definition of a frame...
frame myframe {
    int i = 10; // every new frame will instantiate i with 10
}

// We add some code here after...

// Then we enrich this frame with some more code
// All we need is to use the same frame instruction as above, adding some new stuff
frame myframe {
    function display() {
        println(i);
    }
}
```

### Sub-frames

A sub-frame is a frame that is declared within another frame (the parent frame), from which it inherits both variables and functions. A sub-frame can then replace functions from the parent frame with new versions and add its own variables.

```cpp
// If we want to add some sub-frames...
frame myframe {
    // We can now add our sub-frame...
    frame subframe { ... }
}
```

### Using Upper Definition: frame::function

If you need to use the definition of the parent frame instead of the current thread, Tamgu provides a mechanism that is very similar to other languages such as C++ or Java. The function name must be preceded with the frame name together with "::".

Example:

```cpp
// Calling subframes...
// We define a test frame, in which we define a subtest frame
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

## System Functions

System functions are used to map the different operators of the Tamgu language onto frame methods.

### Comparison Functions

Tamgu also provides a way to help define specific comparison functions between different frame elements. These functions have a specific name, even though they will be triggered by the following operators: `>`, `<`, `==`, `!=`, `<=`, and `>=`.

Each function has one single parameter which is compared with the current element.

Below is a list of these functions:

1. `equal`: function ==(frame b);
2. `different`: function !=(frame b);
3. `inferior`: function <(frame b);
4. `superior`: function >(frame b);
5. `inferior equal`: function <=(frame b);
6. `superior equal`: function >=(frame b);

Each of these functions should return true or false according to their test.

Example:

```cpp
// Implementation of a comparison operator in a frame
frame comp {
    int k;
    
    // We implement the inferior operator
    function <(autre b) {
        if (k < b)
            return true;
        return false;
    }
}

// We create two elements
comp one;
comp two;

// one is 10 and two is 20
one.k = 10;
two.k = 20;

// one is inferior to two and the inferior method above is called
if (one < two)
    println("OK");
```

### Arithmetic Functions

Tamgu provides a mechanism to implement specific functions for the different numerical operators. These functions must have two operators, except for `++` and `--`. They must return an element of the same frame as its arguments.

1. `plus`: function +(frame a, frame b);
2. `minus`: function -(frame a, frame b);
3. `multiply`: function *(frame a, frame b);
4. `divide`: function /(frame a, frame b);
5. `power`: function ^^(frame a, frame b);
6. `shift left`: function <<(frame a, frame b);
7. `shift right`: function >>(frame a, frame b);
8. `mod`: function %(frame a, frame b);
9. `or`: function |(frame a, frame b);
10. `xor`: function ^(frame a, frame b);
11. `and`: function &(frame a, frame b);
12. `++`: function ++();
13. `--`: function --();

Example:

```cpp
frame test {
    int k;
    
    function ++() {
        k++;
    }
    
    // It is important to create a new value, which is returned by the function
    function +(test a, test b) {
        test res;
        res.k = a.k + b.k;
        return res;
    }
}

test a, b, c;
c = a + b; // We will then call our plus implementation above.
```

### Number and String Functions

You can also interpret a frame object as a string or a number in an arithmetic expression, for instance. In that case, you can implement functions with the name of the type of the object you want your frame instance to be interpreted as.

Example:

```cpp
frame test {
    int k;
    string s;
    
    // We return a "test" instance as a string...
    function string() {
        return s;
    }
    
    // We return a "test" instance as a float...
    function float() {
        return k;
    }
    
    // We return a "test" instance as an int...
    function int() {
        return k;
    }
}
```

### Interval and Index

It is also possible to use a frame object as a vector or a map. It is then possible to access elements through an interval or set a value through an index. To use an object in this way, the developer must implement the following function:

1. function [](self idx, self value): This function inserts an element in a vector at position idx.
2. function [](self idx): This function returns the value at position idx.
3. function [:](self left, self right): This function returns the values between the positions left and right.

Example:

```cpp
frame myvect {
    vector kj;
    
    // This function inserts a value in the vector at position idx
    function [](int idx, self value) {
        kj[idx] = value;
    }
    
    // This function returns the value at position idx
    function [](int idx) {
        return kj[idx];
    }
    
    // This function returns the value between l and r.
    function [:](int l, int r) {
        return kj[l:r];
    }
}

myvect test;
test[0] = 10; // We call function [](...)
test[1] = 5; // We call function [](...)
// We call function [:](...)
println(test[0], test[1], test[0:]); // Display: 10 5 [10, 5]
```

## Specialized containers for `frames`

It is possible to constrain containers to only accept a certain type of `frame`. You just need to declare the vector or dictionary as follows:

```Java
frame Test {
    int a;
}

vector<Test> v;
map<Test> m;
```

Note that in the case of a dictionary (map), it is the values and not the keys that are of type `Test`.

Any attempt to add a value that is not compatible with the `frame` will result in an allocation error.
