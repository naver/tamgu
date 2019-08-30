# Functional properties of objects

C++ is a rigid shell with a very soft body.   It is a language where the most robust choices can be ruined by a horrible cast.  C++ transforms memory management into a minefield, where the slightest error can make the code uncontrollable and cause it to crash in the most unusual places.  But C++ is also a very flexible language that can interface with most existing languages such as Java or Python.  Choosing C++ is like driving a 1930s race car, a colossal power without airbags and seat belts on bicycle tires. 
It is this flexibility and efficiency that led me to choose C++ to implement my programming language, despite the constant danger of memory leaks and other language joys.  
Tamgu is the latest in a series of experiments in C++ that I conducted over several years to build the programming language I was looking for.  I needed speed and efficiency to be able to build and modify very large text corpora and C++ seemed to be the only language that could provide me with this power.  On the other hand, before eventually implementing Tamgu, I had to struggle with C++ to isolate a mode of operation that would allow for controlled and efficient memory management.  If C++ requires checking at each step the life cycle of the data being manipulated, it also makes it possible to know very precisely the size of what has been loaded. This aspect is fundamental when managing very large amounts of data. In particular, character string management is crucial in corpus manipulation. The encoding problems but also the speed with which we can extract sub-strings or find patterns are at the heart of this problem. Again, only C or C++ are close enough to the intimate functioning of the machine to provide the necessary finesse. In the "conversion.cxx" file, I tried to concentrate all the tables, functions and other routines around the notion of manipulating character strings. For example, the search for a subchain of characters is based on Intel's "intrinsics" types and sometimes provides performance three or four times better than that of the "find" of the "std::string" type in C++. This is an aspect that higher-level languages cannot offer with the same degree of control. 

# Virtual Machine 

Most modern languages such as Java or Python work on the principle of a virtual machine designed to execute a pseudo-machine language called "opcode" or "byte code", while memory management is controlled in parallel by a "garbage collector".  This choice is very effective but it does pose some problems when you want to extend these languages with compatible external libraries.  In particular, the code of these libraries usually runs _outside the virtual machine_, while their internal data structures most often fall outside of the _garbage collector_. 

## C++ Machine 

In the case of Tamgu, if there is a virtual machine, it blends with the execution of the interpreter in C++.  In Tamgu, each element of the language is implemented as a C++ object.  All you have to do is to overload certain particular methods to specialize their behaviour. In particular, all these objects overload the "Get" method, whose application, depending on the type of the object, may lead to the execution of a loop or a function. On the other hand, a "Get" on a number object will only return that number itself.


### Evaluation 

These objects all derive from the same mother class:  _Tamgu_ which means that a sequence of instructions can be represented by a simple vector: 

```C++ 
vector<Tamgu*> instructions; 
``` 

A Tamgu program is therefore stored in memory as encapsulated objects.  A function, for example, is represented by an object _TamguFunction_ which contains precisely a declaration like the one above.  Each of the elements of this vector can in turn be a loop, an assignment, a function call or a variable declaration.  A _while_ loop will be represented by an object _TamguWhile_ which in turn will contain the same type of vector, which may contain loops, assignments, function calls etc.  

Thus, a Tamgu program is compiled as a nested tree structure of C++ objects. 

To run a Tamgu program, we simply browse through a list of _Tamgu_ objects and evaluate each of them by calling their method _Get_.  At the highest level, a Tamgu program is a simple vector of instructions.  The main loop looks like this: 

```C++ 
Tamgu* o; 
for (auto& a : instructions) { 
    o = a.Get(); 
    o->Release(); 
} 
``` 

A _Get_ method systematically returns a value to the higher level.  If this "o" value has not been saved in a variable or container, the _Release_ method  will destroy it.  The cleaning of the objects is done _en passant_ thanks to the magic of the interpreter's execution stack.  This mechanism allows precise control over the creation and destruction of data structures. 

### Stateless execution 

In addition, as shown in the example above, this execution does not use any global variables, it does not modify any pointers in a parallel execution stack.  The execution does not change any global state, it is "stateless", which ensures both reentrance and robustness.  Moreover, this approach also reduces the code required to execute instructions to very few C++ lines.  If we examine the file "codeexecute.cxx" which contains the code of most Tamgu instructions, we can see that the majority of _Get_ methods do not exceed ten lines.  The code is reduced to short methods that are easy to read and debug. 

### Functional

Stateless execution, reentrance, robustness, these good properties are in fact due to an implementation choice that is inspired by functional programming. If we take the example of a language like Lisp, the slightest instruction in this language is view as a function.

Let's take compare how the "if" is handled in both languages:

```LISP
(if (test) 
    then_exp
    else_exp
)
```

And in C++:

```C++
TamguIf Class: Tamgu public {
    Tamgu* instructions[3];

    //The "Get" method.
    Tamgu* Get() {
        Tamgu* test = instructions[0]->Get();
        if (test == aTrue)
            return instructions[1]->Get();
        else
            return instructions[2]->Get();
    }
};
```

* "if" in Lisp is a three-argument function that returns either the evaluation of "then_exp" or that of "else_exp" depending on the evaluation of "test".

* "TamguIf" is an object made up of a list of three elements that work on the same principle as the Lisp "if", with the difference that to evaluate the elements we will call "Get", where Lisp implicitly calls the "eval" method.

By recursively evaluating a nested list of C++ objects, it is easy to reproduce the behavior of a nested list of functions.

This approach also makes the language easily extensible and easily optimized. You only need to add additional instruction objects to enrich the language. It is also sometimes possible to replace some recurring sequences of instructions with a single, more compact and faster object. But there is one last important aspect of this approach, the fact that internal and external objects can be managed in a similar way by the execution engine.

## Homogeneity

As we have just seen, Tamgu encapsulates all its elements in the form of C++ objects, whose methods are overloaded to specialize their behavior. This is also the case when a user wants to create a Tamgu-compatible library. In this case, the programmer must build her/his own derivation of the class _Tamgu_ to encapsulate her/his personal code. However, for obvious reasons of robustness, only the virtual machine can have the right to life or death on an object. Therefore, it is necessary to hide from the programmer everything related to the life cycle of an object, both for the class it develops and for the objects that might appear in its code at runtime.

Ensuring that the virtual machine alone has access to the creation and destruction of an object requires some adjustments.

### Factory

First of all, we must ask ourselves how objects are created. In Python, when you create an external _toto_ library, you must often provide a _totonew_ method whose purpose is to create objects whose life cycle will be compatible with the Python virtual machine. In Tamgu's case, an object is created by providing its type and a variable:

```C++
toto myvar;
```
This declaration is sufficient to create an object, whether it is internal or from a compatible external library.

Tamgu manages the creation of objects as a _factory_. A _factory_ is a table where we store an instance of all objects corresponding to all types of Tamgu. Each type is associated with a numerical identifier that corresponds to the box where an instance of the corresponding C++ object is stored. For example, in index 23 in this table is an instance of the class _Tamgustring_. This identifier 23 is also associated with a Tamgu type: _string_. In addition, each object overloads a _Newinstance_ method whose role is to create a new instance of the object in question.

```C++
string s; //string has as identifier 23
```

When the declaration of _s_ is required, the _string_ index (here 23) allows access to an instance of _Tamgustring_ in the _factory_ table. All that remains is to call the _Newinstance_ method to have a new object that will be associated with _s_.

### Declaration

The _factory_ table is empty at initialization. It is the objects that record an instance of themselves and are assigned a unique numerical identifier. Just take a look at the files _objectrecording.cxx_ and _containerrecording.cxx_ to check it. Each object has a class method: InitializationModule which records in the _factory_ table this first instance. Each time the engine is started, the _factory_ table is reset completely with the basic objects that are an integral part of the interpreter.

In the case of a compatible external library, the mechanism is hardly more elaborate. First of all, each library implements a derivation of the class _Tamgu_. The programmer then overloads the different methods in his own class to specialize it. It also adds a "C" function whose name is "libname_InitializationModule". At the very end of loading, Tamgu then executes this function whose role is to _call the InitializationModule_ class method so that an instance of the implemented object is registered in the _factory_.

At the end of loading, the origin of the object becomes indistinguishable from those of other objects. For the engine, creating a variable of type _string_ or type _toto_ obeys exactly to the same rules. While in Python or Java, external code usually runs outside the virtual machine, here it is dynamically integrated as a natural extension of the virtual machine.

## Conclusion

The choice to implement Tamgu's instructions as nested objects offers multiple advantages in terms of maintenance and code understanding. This approach allows to keep the semantics of the instructions while keeping the code small and readable, offering great simplicity of reading and debugging. It also allows to keep a homogeneity of interpretation between internal and external objects, thus avoiding the danger of an execution taking place outside the virtual machine.

