## Basic Types

Tamgu requires all items to be declared with a specific type. Types can be either predefined or user-defined as a frame.

### Predefined types

Tamgu proposes some very basic types:

#### Basic Objects:
- self
- string
- int
- decimal
- float
- long
- bit
- short
- fraction
- bool
- date
- time
- call

#### Containers:
- vector
- map
- imatrix
- fmatrix
- file
- iterator

#### Alias
An alias is a function in which arguments are declared without types. An alias can be used to access a structure or to execute some code.

Note: All variables are subject to modification in an alias.

#### Function
A function is declared anywhere in the code using the keyword `function`.

#### Frame
A frame is a user-defined type which is very similar to a class in other languages. A frame contains as many variables or function definitions as necessary.

### Variable Declaration
A variable is declared in a similar way to many other languages. First, the type of the variable is given, followed by a list of variable names separated by commas and ending with a semicolon.

Example:
```cpp
// Each variable can be individually instantiated in the list
int i, j, k = 10;
string s1 = "s1", s2 = "s2";
private type name;
```

A variable can be declared as private, which forbids its access from external sources:
```cpp
private test toto;
```