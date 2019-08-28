# Functional properties of objects

C++ is a conch, a rigid shell with a very soft body.  It is a language where the most robust choices can be ruined by a horrible cast. It is a language that transforms memory management into a minefield, where the slightest carelessness can cause the code to crash in the most unusual places. But C++ is also a very flexible language that can interface with most existing languages such as Java or Python. Choosing C++ is like driving a 1930s race car, a colossal power without airbags and seat belts on bicycle tires. It can hurt.
When I started working on Tamgu in 2010, there was little alternative, except maybe C. Tamgu's code has undergone a lot of massive rewriting over the years but during the last iteration, I made a clean slate of the existing code and started over with a totally different architecture. In particular, I have worked hard on how to reduce the risks of C++ programming to a minimum, drawing inspiration from functional programming.

## Tabula Rasa

It often takes less time to completely rewrite a system from scratch than it does to debug old code and keep it alive against all odds. Because if the current code is based on a new architecture, the accumulated experience allows you to move very quickly on the invariants. Archaeologists have been struggling to rebuild the Parthenon for more than fifty years, whereas it took the ancient Greeks only 11 years to build it. 
This last version proposes a unification of the management of objects whether they are native or from an external library. From now on, all objects handled by Tamgu are implemented in the same way. This is a fundamental difference with the most common interpreted languages.

## Virtual Machine

Indeed, most of the current programming languages Java, Python, Kotlin or Perl work on the principle of a "virtual machine" designed to execute a pseudo-machine language called "opcode". Once your code has been compiled into opcodes, the virtual machine is able to run it. In addition, memory management is done through a garbage collector that eliminates most memory leak problems and other joys of C++ programming. 
However, this approach introduces a gap between the ways basic objects of the language are handled and those from external libraries. As a result, an external function is executed _outside the virtual machine_. If this function also manipulates its own internal data structures, these will fall outside of the garbage collector.
Debugging an external library is often like walking through a maze in the dark with a blind cane.

## Tamgu Class

Tamgu operates on a radically different principle. Tamgu introduces a common class _Tamgu_ which is divided into two distinct classes: _TamguTracked_ and _TamguReference_. 

* _TamguTracked_ is the mother class of all static language objects, especially instructions such as loops or functions. Each instance is systematically registered in a global list, hence the name, so that the memory can be cleaned in a single iteration.

* _TamguReference_ is the parent class of all volatile data such as strings, numbers or containers.

### Get

All these objects overload a "Get" method whose role is to perform the evaluation of the object. A "Get" on a number instance will only return the instance itself, while a "Get" on an instruction object will cause its execution. Each "Get" necessarily returns a value.

## Compilation

Compilation consists in transforming each language instruction into a particular object. For example, a "if" is compiled into a _TamguIf_ object, a "while" into a _TamguWhile_ object and a function into a _TamguFunction_ object.

### Recursive

In addition, the construction is done in a recursive way. For example, a _TamguFunction_ object includes a list of instructions that can be written in C++ in the following form:

```C++
vector<Tamgu*> instructions;
```

The first step in compiling a program is to create a syntactic tree with each node corresponding to a Tamgu object. We then follow the tree in a recursive way to create at each step the corresponding objects and store them in their parent objects.

### LISP

This is similar to Lisp where each instruction is actually a function call.

```LISP
(if (test) then else)
```

* "if" in Lisp is a three-argument function that returns either the evaluation of "then" or that of "else" depending on the evaluation of "test".

* "TamguIf" is an object made up of a list of three elements that work on the same principle as the Lisp "if", with the difference that to evaluate the elements we will call "Get", where Lisp implicitly calls the "eval" method.

The same idea is found in the definition of a function:

```LISP

(defun toto(x y)
    (if (< x y)
        (+ x y)
        (* x y)
    )
)
```
* A Lisp function consists of a list of parameters and a list of instructions. It must also return a value.

* A "TamguFunction" function is an object composed of a list of parameters and a list of instructions. The execution of a Tamgu function consists in matching the parameters to the arguments, then going through the list of instructions by applying the "Get" method to each of them to evaluate them.

```C++
for (auto& a : instructions)
    a.Get();
```

This recursive definition of objects is very similar to the recursive definition of function calls in Lisp.

## Garbage Collector

Tamgu does not have a garbage collector mechanism.  It keeps track of some objects such as instructions or constants, but there is no mechanism for asynchronous destruction of objects. 
To better understand the memory management mechanism, we will simply rewrite the code above:

```C++
Tamgu* o;
for (auto& a : instructions) {
    o = a.Get();
    o->Release();
}
```

The destiny of a data structure is to be stored somewhere either in a variable or in a container. When a data has not been used, "Release" destroys it. Otherwise, "Release" leaves it intact.

This mechanism controls the life cycle of a data structure after it has been called in order to get rid of it at the right time.  It is therefore possible to do without a "garbage collector", the execution of which sometimes leads to untimely disruptions.

### Dynamic libraries

An external library in Tamgu is a simple derivation of the class _TamguReference_ with an overload of the appropriate methods for executing its purpose. Therefore, instances from this class run in exactly the same way as the rest of the native objects. There is therefore no gap between internal and external codes. A function will store an external object in its instruction list in the same way as an internal object. The creation and destruction of an external object will follow the same life cycle as an internal object.

## Functional properties of objects

It should be noted, however, that this approach is more memory-intensive than "opcodes" except that language instructions are not mapped to a single opcode but to a routine composed of opcodes. It should also be noted that in Tamgu objects have a much richer semantics than a single opcode.

On the other hand, this approach allows us to benefit from properties that are generally attributed to functional approaches. 

### Stateless execution

"Get" methods depend only on their local context to be processed. If we use the "Get" code from _TamguFunction_, we can see that there is no need to refer to any global variable. There is no pointer in a local execution stack, no reference to an external state of the program. Calls are recursively processed and each returned value is evaluated locally. 

### Robustness

As there is no global management of the code flow, side effects are very rare. In addition, the association object / instruction leads to very local and especially very small code. If we look at the C++ code dedicated to Tamgu instructions in the file "codeexecute.cxx", we can see that the "Get" methods rarely exceed ten lines. This parsimony of the code combined with the absence of side effects makes debugging much simpler.

### Reentrance

Finally, the absence of states also makes it possible to make the code easily reentrant. The integration of multithreading in Tamgu is less than 250 lines of code, essentially the preparation and cleaning of the execution context. All instructions can be executed without any difficulty and without any particular modification in a thread.

## Conclusion

The choice to implement Tamgu's instructions as nested objects offers multiple advantages in terms of maintenance and code understanding. This approach allows to keep the semantics of the instructions clear while keeping the code small and readable, offering great simplicity of debugging together with a homogeneity of interpretation between internal and external objects. Furthermore, the very precise monitoring of the life cycle of objects makes it possible to reduce the risks of memory leaks or pointer null exceptions to a minimum.


