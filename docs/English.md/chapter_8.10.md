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
//implementation of a comparison operator in a frame
frame comp {
    int k;
    //we implement the inferior operator
    function <(frame autre) {
        if (k < autre)
            return true;
        return false;
    }
}

//we create two elements
comp one;
comp two;
//one is 10 and two is 20
one.k = 10;
two.k = 20;
//one is inferior to two and the inferior method above is called
if (one < two)
    println("OK");
```

### Arithmetic Functions

Tamgu provides also a mechanism to implement specific functions for the different numerical operators. These functions must have two operators, except for `++` and `--`. They must return an element of the same frame as its arguments.

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
    //it is important to create a new value, which is returned by the function
    function +(test a, test b) {
        test res;
        res.k = a.k + b.k;
        return res;
    }
}

test a, b, c;
c = a + b; //we will then call our plus implementation above.
```

### Number and String Functions

You can also interpret a frame object as a string or a number in an arithmetic expression for instance. In that case, you can implement functions with as a name, the type of the object you want your frame instance to be interpreted as.

Example:

```cpp
frame test {
    int k;
    string s;
    //We return a "test" instance as a string ...
    function string() {
        return s;
    }
    //We return a "test" instance as a float ...
    function float() {
        return k;
    }
    //We return a "test" instance as an int ...
    function int() {
        return k;
    }
}

Nota bene: In the case of numbers, you should note that you can define as many functions as the number of available types: short, int, long, decimal, and float. However, the first function to be defined will be the default numeric function for all types, unless you specifically add new function implementations. Hence, in our example, all numerical type interpretations will be a float by default except for int.

### Interval and Index

It is also possible to use a frame object as a vector or a map. It is then possible to access elements through an interval or set a value through an index. To use an object in this way, the developer must implement the following function: