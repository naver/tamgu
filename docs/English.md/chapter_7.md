# Loading External Files: Type Tamgu vs Loadin

The `tamgu` type is used to dynamically load a specific Tamgu program. On the other hand, `loadin` is used to load the content of a file into the current program.

## 9.1 Methods

1. `tamgu var(string Tamgu pathname)`: Creates and loads a Tamgu program.

## 9.2 Executing External Functions

The functions available in the Tamgu file can be called through a `tamgu` variable.

Example:

```cpp
// In our program test.tmg, we implement the function: Read
// In test.Tamgu
function Read(string s) {
    return (s + "_toto");
}

// In call.Tamgu
// In our calling program, we first load test.Tamgu, then we execute Read
tamgu kf('c:\\test.tmg'); // We load a program implementing Read
string s = kf.Read("xxx"); // We can execute Read in our local program.
```

## Private Functions

If you do not want external programs to access specific functions, you can protect them by declaring these functions as private.

Example:

```cpp
// We implement a function that cannot be called from outside
private function CannotBeCalled(string s) { ... }
```

## `loadin(string pathname)`

Tamgu also provides another way to load files in another program. `loadin(pathname)` loads the content of a file into the current program. In other words, `loadin` works exactly as if the current file contained the code of what is loaded with `loadin`. The code is actually loaded at the very place where the call to `loadin` is written.

Example:

```cpp
// In our program test.tmg, we implement the function: Read
// In test.Tamgu
function Read(string s) {
    return (s + "_toto");
}

// In call.Tamgu
// In our calling program, we first load test.Tamgu, then we execute Read
loadin('c:\\test.tmg'); // We load the code implementing Read
string s = Read("xxx"); // We can execute Read in our local program directly.
```