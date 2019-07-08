# Introduction

The following document describes how the Tamgu code (탐구) was implemented. This document is intended for anyone wishing to discover the inner working principle of the interpreter. While reading this document may shed light on Tamgu's implementation choices, it is not mandatory to read it in order to use the language.


# Code organization

The interpreter of a computer language is generally divided into three parts:

* Building the syntactic tree from the code. This operation is also called "parsing".
* Compiling the code that transforms the syntactic tree into an executable structure
* Code execution

## The "parser": _codeparse_

The construction of the syntactic tree is done using the class: _bnf_tamgu_ whose code is stored in the files: _codeparse.cxx_ and _codeparse.h_.

Warning: These two files were not written by hand but were automatically generated out of the _tamgu_ file in the directory: _bnf_. 
The _bnf_ directory also contains scripts in Python to regenerate new versions of _codeparse_ if necessary.

The description of the _BNF_ language used is recorded in the _tamgu_ file itself.

#### Nodes: x_node.h

This file contains the classes to save the syntactic tree: _x_node_. 

A _x_node_ contains its name (_token_), value (_value_), position in the initial text (_start_, _end_) and child nodes.

The _x_nodes_ are produced by _codeparse_.

#### Tokenization

On the other hand, the input structure of _codeparse_ is the result of a _tokenization_. The file contains the class _x_rules_ and its derivations _x_reading_ and _x_wreading_. The class _x_rules_ allows the recording and use of tokenization rules.

These tokenization rules divide the input code into _tokens_ while maintaining both their position in the file and their identity as defined by a rule such as:

```
rules.push_back("%d+(.%d+)(e([- +])%d+)=3");
```

This rule allows to recognize among other things a  number with exponent. When such a _token_ is identified, the code "3" is returned, which identifies the nature of this token as a number. We can then refer to this code in the BNF grammar to generate the corresponding correct node:

```
# Numbers as parsed by the tokenizer: "3" is the code yielded by the tokenizer.
^3 anumber := .
```

By default, these rules directly recognize strings between quotes or separators such as";",":".

```
# punctuation as parsed by the tokenizer
^0 punct := .

# String definitions as parsed by the tokenizer
^1 astringdouble : = .
^2 astringsimple : = .
^5 afullstring : = .

# Numbers as parsed by the tokenizer
^3 anumber := .

#token as parsed by the tokenizer
^4 word := = .
^4 typename := .
^4 astring := .

#regular expressions. In some cases, the code is a simple character.
^9 atreg := .
^a astreg : = .
^b apreg : = .
^c aspreg : = .
```


## The compiler: _codecompile_

Once the syntactic tree is correctly constructed, it is passed to the compiler. The compiler is implemented as a class: TamguCode which contains everything you need to traverse the syntactic tree and build the internal representations.

#### Choice of a compiling method

The TamguGlobal class contains a special dictionary that associates a token name with a TamguCode compiling method: _parseFunctions_. The method _RecordCompileFunctions_ records all these methods in this dictionary.

The main method when the syntactic tree is compiled is: _Traverse_.

This method includes two parameters: the current node in the syntactic tree and the object being built.

This method examines the current node and looks at whether its _token_ is associated with a compiling method. If this is not the case, it examines the child nodes: A _token_ may not be associated with a method.

When a compiling method is chosen, it will enrich the current object with the necessary information by analyzing the current node and its sub-nodes.

#### TamguGlobal

The Tamgu interpreter contains almost no global variables with one exception: _globalTamgu_ of type _TamguGlobal_.

The _TamguGlobal_ class allows you to centralize the creation of objects to perform their destruction. Tamgu does not include a  Garbage Collector as such, but keeps track of the number of objects that the interpreter may need during execution. For example, all instructions are stored in the _tracked_ atomic vector.

This class also keeps track of _threads_ and manages their creation or deletion. _TamguGlobal_ also manages the variable stack.

#### Object

Each data structure is an independent class derived from _TamguReference_, recorded in their own file. Each object exposes at least the following methods:

* The following two methods are used to record methods associated with an object. In particular, _InitializationModule_ is called at startup to register all the basic objects and their methods. The method _AddMethod_ allows to associate a method name on the _Tamgu_ side with a method of the corresponding class.

** static void AddMethod(TamguGlobal* g, string name, objectMethod func, unsigned long arity, string infos);
** static bool InitializationModule(TamguGlobal* g, string version);

* The following two methods are used to return or modify the value of an object 
** Tamgu* Get(Tamgu* context, Tamgu* value, short idthread); 
** Tamgu* Put(Tamgu* context, Tamgu* value, short idthread); 

* The following methods are used to return atomic values:
** long Integer();
** string String();
** etc.

* The following methods are used to compare values with each other
** Tamgu* less(Tamgu*)
** Tamgu* lessequal(Tamgu*)
** Tamgu* more(Tamgu*)
** Tamgu* moreequal(Tamgu*)
** Tamgu* same(Tamgu*)
** Tamgu* different(Tamgu*)

*The following methods are used to perform operations with other values
** Tamgu* minus(Tamgu*)
** Tamgu* plus(Tamgu*)
** Tamgu* multiply(Tamgu*)
** Tamgu* divide(Tamgu*)
** etc.

#### Template

The _template_ directory allows you to build your own objects and libraries. Just give the name of your object to the script and it automatically generates a directory in which it places the source and include files as well as the Makefiles. All you have to do is fill in these templates with your own code.
