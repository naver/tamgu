## Elements of Language

A Tamgu program consists of variable declarations, function declarations, and frame (or class) declarations. Variables can be declared anywhere, as can functions, with the exception of loops.

### Comments

Comments for a single line are introduced with `//`.

```javascript
// This is a comment
```

Comments for a block of lines are enclosed between `/@` and `@/`.

```javascript
/@ 
This is a block of comments.
@/
```

### Functions

A function is declared using the keyword `function`, followed by a name and parameters.

### Frames

A frame is declared using the keyword `frame`, followed by a name. A sub-frame is simply declared as a frame within a frame.

### Function and Frame Pre-declarations

The pre-declaration of functions and frames is not necessary in Tamgu. The interpreter first loops through the code to detect all functions and frames and declares them beforehand.

Therefore, the following code is valid:

```javascript
// We call call2 from within call1
function call1(int x, int y) { 
    call2(x, y);  
} 

// call2 is declared after call1  
function call2(int x, int y) { 
    println(x, y);  
}
```