# TAMGU (탐구)

TAMGU provides also an annotation language implementation (see below for an example), which allows for an efficient way to detect complex patterns in text. 

TAMGU is a _multithreaded programming language_ that mixes in one single formalism an _imperative language_, a _functional language_ (close to Haskel) and a _logic programming language_ (à la Prolog). 

The language provides many features but also many libraries, which helps design and implement annotation schemes for complex documents.

__Note__: _Releases_ (pre-compiled versions) are available at: https://github.com/naver/tamgu/releases

## Example of code

Below is an example of the kind of code that you can implement with _Tamgu_.

```C++

//An imperative language

int i=10;
string s="20";

s+=i; //s is now 2010
i+=s; //i is now 30 (automatic conversion of a type into another)
------------------------------------------------------------------------------------
//Containers

map m; //all sorts of containers
treemap t;
vector v;
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

* Mac OS                        (see _Xcode_ directory and _install.py_)
* Windows                       (see tamgu.sln or tamguraw.sln)
* Different flavors of Linux    (_install.py_)

## Compiling on Windows

We provide a _tamgu.sln_ makefile, which has been created for _Visual Studio 2013_. 
This makefile comprises sub-makefiles to compile:

* the different tamgu libraries (see the list below)
* the command line version (tamgucom)
* a native window MFC GUI (Tamgui). This last makefile requires a full-fledged _Visual Studio 2013_ version with _MFC_. 

* Note: The _python_ library is linked with _Python 3.7.3_.

* Note bis: The __tamguinstaller__ makefile requires the installation of a specific Visual _plugin_, which is free: _Visual Studio Installer_.
_(https://marketplace.visualstudio.com/items?itemName=UnniRavindranathan-MSFT.MicrosoftVisualStudio2013InstallerProjects)_

### tamguraw.sln

If you don't have a commercial licence for Visual 2013, then the MFC might be absent from your installation. You can then use _tamguraw.sln_ to compile a Windows version.

## Compiling on Mac OS

We provide in the directory: _Xcode_, two Makefiles (compatible with the _Xcode_ framework on Mac OS):

* tamgubase: it compiles a console version of tamgu, it allows for a simpler way to debug _tamgu_.
* tamgugui: it compiles a native Mac OS GUI for _Tamgu_

However, if you want to compile the different libraries, you need to launch: _install.py_ as shown below.

## Compiling on linux (or Mac OS)

The compiling is done in three steps
* a) launch: _python install.py_ to create the _Makefile.in_ that matches your platform.
* b) launch: _make all_
* c) launch: _make libs_ to compile the external libraries (Note: the target for compiling libraries is in _Makefile.in_)

__Note__ : You can also execute: _make all libs_

install.py comes with the following options:
* -noregex: Do not compile with posix regular expression support
* -withsound: Check if you can compile the _sound_ library, which offers a rather rich coverage for most sound files: ogg, mp3 etc.
* -pathregex path: Path to regex include files
* -pathpython path: Path to Python include file and library.
* -pathfltk path: path to GUI libraries (if you have a specific version of fltk1.3 in a different directory than /usr/lib)
* -version name: Directory names for intermediate and final files depend on 'name', which allows for compiling different versions locally 
* -pathlib path: provides a system path to check for system libraries
* -intel: use specific intel instruction to speed up seach and conversions for strings.
* -help: display this help

### Important

Add the following lines to your bash profile:

```sh
export TAMGULIBS=/home/me/tamgu
export LD_LIBRARY_PATH=$TAMGULIBS:$LD_LIBRARY_PATH
export PATH=$TAMGULIBS:$PATH
```
Of course, _/home/me/tamgu_  should be replaced with the path that points to the directory that contains _tamgu_ and its libraries.

_The TAMGULIBS variable is central to access the different Tamgu libraries_

There are two versions of tamgu: _tamgu_ and _tamguconsole_.

_tamguconsole_ only works if the FLTK1.3 library has been installed. The presence of this library also enables the Tamgu GUI features. You can install _FLTK_ via __yum__ or __apt-get__ according to your platform.

## Compiling libraries

Tamgu provides different libraries, which can be used in a Tamgu program. Each of these libraries is stored in its own directory and can be compiled once the Tamgu interpreter has been compiled.  

* _allmaps_: this library provides access to all maps.
* _pytamgu_ : this library enables tamgu to communicate with python (2 or 3 depending with which version you compile). The library can be both loaded from within Python to handle Tamgu programs or from within Tamgu to execute Python programs. 
* _word2vec_: this library provides a way to compute and exploit word embeddings. 
* _wapiti_ : this library provides a CRF implementation
* _liblinear_: this library provides a linear classifier
* _curl_ : this library provides ways to handle Web instructions, such as loading a page, download a package etc.
* _fltk_ : this library provides a graphical API that is available on all platforms
* _sound_: this library provides instructions to load and play the most common sound format (mp3, ogg etc.)
* _sqlite_: this library provides an access to SQLite databases.
* _xml_: this library based on libxml2 provides methods to handle XML encoded files

Except _allmaps_, which is a full _tamgu_ library, none of these libraries is mandatory and can be compiled beside _tamgu_.

_curl, fltk, sound, sqlite, xm, Python_ are usually pre-installed on most Linux platforms. If they are missing, you can ask your administrator to install the _development version_ for you. However, if you plan to compile with Python 3 version, note that these versions are not always pre-installed. You will need to provide the exact path the include/library files with _-pathpython_.

_word2vec, wapiti, liblinear_ are external libraries that have been embedded into _tamgu_ libraries. We provide a licence for each of them in their respective directory. These libraries have usually also been modified to communicate with _tamgu_ and might present some slight differences. 

### Compiling libs

The _install.py_ script detects, which libraries are available on your system and automatically builds a _libs_ target in _Makefile.in_.

* In the case of Windows, the libraries are available in the _tamgu.sln_ and _tamguraw.sln_ make files.
* In the case of _Mac OS_,  the libraries are already all available.
* In the case of _Linux_, the _libs_ target will depend on which libraries were found on the machine. __Note__ that the output of _install.py_ displays the list of libraries that are absent on your machine. You may ask your administrator to install them for you.

#### Compiling Python Library

_pytamgu_ is a library that is compatible both with python and tamgu. In other words, you can _import pytamgu_ in a Python program and execute _tamgu_ scripts as if it was some Python code. Containers and strings are automatically converted in both directions.

If you want to compile with a specific version of Python (for instance with Python 3),  you should use _"-pathpython"_, which should point to the python "include" directory. 

The library can be compiled either with _Python 2.7_ or _Python 3.x_...

Note that the script has been written for _python2.7_...

See the doc for information on how to use it... In fact, _pytamgu_ only exposes two methods. One to load a tamgu program (_load_), the second to execute a function exported by this tamgu program (_execute_). See below for an example.

#### Compiling the Java API
To compile the Java API, you need to call install.py with "-java".
You will need a version of "ant" installed on your machine.


## Example

### To compile a version on Ubuntu with a specific link to python3.6

_python install.py -version ubuntu -intel -pathpython /usr/include/python3.6m_

The code for _pytamgu_ is in: _libpython_  

The _-version_ option allows for the compiling of different versions in the same directory. We choose _ubuntu_ here as a name for this specific compiling, but you can choose whatever name you want. The name is not _binding_ to a specific compiler or a specific OS.

When you launch _install.py_, the system returns the list of libraries that are missing. These missing libraries does not mean that _tamgu_ will not compile, but that some of the tamgu libraries in the list above might not compile by lack of specific libraries. 


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

