# TAMGU (탐구): A FIL Language

TAMGU is a __FIL__ programming language: __Functional, Imperative and Logical__. 

TAMGU is a _multithreaded programming language_ that provides: 

* an _imperative_ formalism close to Python, but with a strong and powerful type system. 
* a _functional_ formalism inspired by Haskell, which can freely mix with the _imperative_ paradigm
* a _logical_ formalism inspired by Prolog, which can freely mix with the _imperative_ and _functional_ paradigms

_For each specific problem in your programming, choose the most appropriate formalism and freely mix functional, imperative and logical approaches to implement the most expressive but also the most compact code possible._

### Annotation
TAMGU provides also an annotation language implementation (see below for an example), which allows for an efficient way to detect complex patterns in text. 

The language provides many features but also many libraries, which helps design and implement annotation schemes for complex documents.

## Pre-compiled Releases
__Note__: _Releases_ for Windows, Mac OS and Linux are available at: https://github.com/naver/tamgu/releases

## Example of code

Below is an example of the kind of code that you can implement with _Tamgu_.

```C++

//An imperative language

int i=10;
string s="20";
------------------------------------------------------------------------------------
//Containers

map m = {'a':1, 'b':2}; //all sorts of containers
treemap t = {"u":10, 'v':'toto'};
vector v = [1,2,3,"a","b"];
ivector iv = [1..10]; //iv = [1,2,3,4,5,6,7,8,9,10]
iv = [1,3..10]; //iv = [1,3,5,9]
------------------------------------------------------------------------------------
//Functions, threads and frames

function mycall(int i, string s) {...} //functions
thread mythread(int i, string s) {...} //threads
frame myclass {...} //class definition
------------------------------------------------------------------------------------
//Native Korean string support

s="에버랜드 알쏭달쏭 Quiz";
if (s[0].ishangul()) println("Ok");
uvector dec = s.jamo(); //['ᄋ','ᅦ','ᄇ','ᅥ','ᄅ','ᅢ','ᆫ','ᄃ','ᅳ'...]
dec = s.romanization(); //['-/ng','e','b','eo','r/l','ae','n','d',...]
------------------------------------------------------------------------------------
//Functional approach based on Haskell

ivector iv = [1..20]; //a list of integers between 1 and 20
iv.shuffle(); //The order of the elements is now random.

<fastsort([]) = []>;  //if the list is empty, we return an empty "list"
<fastsort([fv:v]) =  minlist &&& fv &&& maxlist where //we merge the different list...
    let minlist = fastsort(<filter (<=fv) v>), //we sort the list with elements smaller than fv
    let maxlist = fastsort(<filter (>fv) v>)>; //we sort the list with elements larger than fv

//we can call a functional definition as regular code
v=fastsort(iv); 

------------------------------------------------------------------------------------
//Predicates

concat([],?X,?X).
concat([?H|?T],?Y, [?H|?Z]) :- concat(?T,?Y,?Z).

//You can mix freely your predicate definition with some regular tamgu code
v=concat(["english",'russian',"french"],['spanish'],?L);
println(v); //["english",'russian',"french","spanish"]

```


## Documentation
The documentation is in : **docs/tamgu.pdf**

## Compiling

The TAMGU project has been implemented in C++. It can be compiled on most platforms:

* Windows: see https://github.com/naver/tamgu/wiki/1.1-Compiling-on-Windows
* Mac OS: see https://github.com/naver/tamgu/wiki/1.2-Compiling-on-Mac-OS
* Linux: see https://github.com/naver/tamgu/wiki/1.3-Compiling-on-Linux

### Libraries
Tamgu also provides different libraries:

Compiling libraries: https://github.com/naver/tamgu/wiki/1.4-Compiling-libraries-(Linux-&-Mac-OS)

### Python
Furthermore, Tamgu provides a bi-directional library, which can be used to execute Tamgu code from Python (and conversely, Python code in Tamgu)

See: https://github.com/naver/tamgu/wiki/1.4-Compiling-libraries-(Linux-&-Mac-OS) for more information

## Code Example 

```Java

//We define some lexical rules (starting with a "@")
@food <- burger.
@food <- tartare.


//Our rule: if a "food" word is found in a sentence, then we return a "meal" label 
meal <- "the", #food.

//We need a specific object to scan a sentence
annotator r;

//a sentence
string sentence="Here, the burger and the tartare are delicious."
vector v = r.parse(sentence); 

//Result: v =  [['meal',[10,16]],['meal',[25,32]]]
//It reads: two 'meal' were found at position 10-16 and position 25-32...

```

# __IMPORTANT__ : 

```
Pre-compiled versions for:

    Windows
    Mac OS
    Linux: Centos, Ubuntu, Fedora 

```
are available in the *__releases__* section in this *__GitHub__* archive.

## License

```
BSD 3-Clause License

Copyright (c) 2019-present NAVER Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of Naver Corporation nor the names of its 
   contributors may be used to endorse or promote products derived from 
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

