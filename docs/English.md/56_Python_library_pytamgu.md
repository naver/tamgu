# pytamgu

The `pytamgu` library is a dual library that can be used both as a Python library and as a Tamgu library. It allows you to execute Python code from within a Tamgu program or execute a Tamgu program from within Python code.

## As a Tamgu library

When used as a Tamgu library, `pytamgu` exposes a new type called `python`. The base Tamgu types (boolean, integer, long, float, fraction, string, vector containers, and map containers) are automatically mapped onto the corresponding Python types.

The `python` type exposes the following methods:

1. `close()`: Close the current Python session.
2. `execute(string funcname, p1, p2...)`: Execute a Python function with `p1`, `p2`, etc. as parameters.
3. `import(string python)`: Import a Python file.
4. `run(string code)`: Execute Python code.
5. `setpath(string path1, string path2, etc...)`: Add system paths to Python.

The `setpath` method is crucial for using the `import` method, which works the same way as the `import` keyword in Python. If you want to import a Python program at a specific location that has not been referenced through `PYTHONPATH`, you need to add it with `setpath` first.

### Example

First, we implement a small Python program called `testpy.py`:

```python
val = "here"
# The input variables are automatically translated from Tamgu into Python variables
def Iteste(s, v):
    v.append(s)
    v.append(val)
    return v
```

Then, we implement our own Tamgu program, which will call this file (assuming it is in the same directory as our Tamgu program):

```tamgu
// We need to use Pytamgu for our own sake
use("pytamgu");

// We need a variable to handle the Python handling
python p;

// We suppose that our Python program is in the same directory as our Tamgu program
p.setpath(_paths[1]);

// We then import our program
p.import("testpy");

vector v;
string s = "kkk";

// We execute the Python function Itest, which takes as input a string and a vector,
// which will be converted into Python objects on the fly.
// The output is automatically re-converted into a Tamgu vector (from the Python vector)
vector vv = p.execute("Itest", s, v);

println(vv); // Output is: ['kkk', 'here']

p.close(); // We close the session
```

## As a Python library

When used as a Python library, you can import the `pytamgu` library, which exposes two methods.

1. `load(file, arguments, mapping)`
   - `file` is the filename of the Tamgu file to load.
   - `arguments` is a string that provides arguments to the Tamgu file separated by a space.
   - If `mapping` is set to 1, a Python method is created for each function in the Tamgu file with the same name as the Tamgu functions.
   - This method returns a handle.

2. `execute(handle, function_name, [arg1, arg2, ..., argn])`
   - `handle` is the handle of the file that contains the function we want to execute.
   - `function_name` is the name of the function in the Tamgu file.
   - `[arg1, arg2, ..., argn]` is the list of arguments that will be provided to the Tamgu program as a vector of strings.

If you use the mapping option, the `execute` method is optional. The values returned by the Tamgu program are automatically translated into Python objects, and the same applies to the arguments transmitted to the Tamgu program.

**Note:** Tamgu always returns Python Unicode strings.

### Example

Tamgu Program:

```tamgu
vector v = [1..10];

function rappel(string s, int j) {
    j += 10;
    v.push(j);
    v.push(s);
    return v;
}
```

Python Program:

```python
import pytamgu

h0 = pytamgu.load("rappel.kif", "", 1)
# We use the mapping to a Python function
v = rappel("Test", 10)
for i in v:
    print(i)
# This is equivalent to
v = pytamgu.execute(h0, "rappel", ["Test", 10])
```