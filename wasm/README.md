# TAMGU webAssembly

This is the code to compile a WebAssembly version of Tamgu.

### Some Documentations
1. [C to WASM](https://developer.mozilla.org/en-US/docs/WebAssembly/C_to_wasm)
1. See [Pragmatic compiling C++ to WASM](https://medium.com/@tdeniffel/pragmatic-compiling-from-c-to-webassembly-a-guide-a496cc5954b8) for some nice explanations on how to do it
1. [Calling a method](https://wasmbyexample.dev/examples/strings/strings.c.en-us.html)

### To install it

Do `git clone https://github.com/juj/emsdk.git` to create a new version.

```
./emsdk install latest
./emsdk activate latest
```

### To compile

The Makefile has been modified to be able to use: **emsdk**

`make all`

### To launch the server:
`emrun --port 8080 .`

## Important Files

### index.html
This file contains the minimum code to execute a LispE instruction from within a browser.
The loading and initialisation of the *LispE* interpreter is done in *lispe_run.js*

### tamgu_run.js
This is the encapsulation in JavaScript of the WASM libary loading.
It exposes:
```JavaScript
function setMaxSizeInput(v); //which set the maximum size of code sent to Tamgu 
function callEval(code); //which executes the execution of a piece of Tamgu code, it returns a string
function callResetTamgu(); which resets the current Tamgu interpreter
```

### tamgu.wasm
This is the WASM library, which is loaded with tamgu_run.js.

