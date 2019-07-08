# Data programming

Today, thanks to Machine Learning algorithms, speech recognition or machine translation has entered the lives of millions of people. Unfortunately, for these algorithms to work optimally, they need annotated data, a lot of annotated and structured data. 

This problem is at the heart of most Artificial Intelligence teams around the world. How to have enough data to train efficient and reliable Machine Learning models?

## Tamgu (탐구)

In Korean, _탐구_ (tamgu) means _quest, research, exploration_.  

The language has been designed to simplify the creation of new corpora.

First, Tamgu is a modern language that combines imperative, functional and logical programming in a single formalism. 

* Imperative programming: multithreaded, close to Python in spirit but with explicit typing of variables, Tamgu offers you the full range of a rich and concise programming language.

* Functional programming: inspired by Haskell, Tamgu allows you to build extremely compact lambda-functions.

* Logical programming: inspired by Prolog, Tamgu allows you to build logical routines that you can integrate into your code.

There is full interoperability. A Tamgu vector remains the same vector whether it is used in functional or logical programming.

## Libraries

Tamgu is written in C++. The language is designed in such a way that it is very easy to create new libraries. 

Moreover, Tamgu already comes with many libraries, such as cURL (to manipulate web pages), SQLite (to manipulate databases), FLTK (to manage graphical interfaces) or AO and SNDFILE (to play audio files).

Tamgu also provides a tool to allow you to quickly create new libraries, so you can enrich the language with your own algorithms.

## Rules and Glossaries

Tamgu also offers the possibility of creating lexicons in the form of a transducer that can contain millions of words for both Western and Asian languages.

These lexicons can be combined with quasi-syntactic rules that can be added to the code in a transparent way. Once again, interoperability is maintained. Thanks to these rules you can easily isolate recurring patterns in your texts, in order to annotate them automatically.

#### Machine Learning

Tamgu also offers some Machine Learning libraries to manipulate embeddings or to apply classifiers or CRFs to your data. Thus, the above rules can for example include capsules based on embeddings to isolate close words.

## Noisy Corpus

Tamgu allows you to create custom corpuses for your learning models. For example, as part of the machine translation evaluation campaign: WMT 2019, we used Tamgu to build a noisy corpus. First, we automatically identified poorly written words in a corpus of comments, using a French lexicon. Then we took a clean corpus and automatically replaced some words with their incorrect versions that we had previously isolated.

# Examples

Basic programming in Tamgu is not difficult. We have tried to keep the formalism close to what people are used to. However, it should be noted that the language requires explicit typing as in Java or C, while offering many implicit conversions between types. 

```C++

int i=10;
string s='20';

s+=i; //s is now 2010
i+=s; //i is now 30 (automatic conversion of a type into another)

vector v=[1,2,3,3,4,5,5,6];

s = v; //s is now"[1,2,3,4,5,6]"

vector vv = s; // s is converted back into a vector which is stored in vv.

```
The language offers many types of containers, dictionaries, vectors. It also offers the possibility of creating classes and their derivations. 

## Functions, Frames, Threads

Tamgu also offers the means to write functions and classes (called 'frame' in Tamgu). 

```C++

function callme(int i, string s) {...}

frame test {
    int i;
    string s;

    //creator
    function _initial(int e, string u) {
        i = e;
        s = u;
    }

    function test() {
        return i;
    }
}

//we create a frame element
test toto(10, "value");
```

Tamgu is also a multi-threaded language. Each object in Tamgu is _systematically_ protected for potential use in threads.
Tamgu provides _lock_, the notion of _join_. 

```C++

//A thread is simply a thread....
joined thread callme(int i, string s) {
    println(i,s);
}

//We call a thread as a normal function....
callme(1, "t1");
callme(2, "t2");
callme(3, "t3");
callme(4, "t4");
callme(5, "t5");
callme(6, "t6");

//We wait for all threads to end....
waitonjoined();

```

You can still transform any variable into a _thread_ variable:

```C++

long i=1000;

//a simple thread that wait on i to be 0...
thread callme() {
    waitonfalse(i);
    println("Ok");
}

callme();

//We then decrement i... When i is 0, the thread is unlocked....
for (int e = 0; e <2000; e++)
    i--;

```

## Character string management

Tamgu provides an impressive arsenal for manipulating your strings. First of all, it dynamically recognizes the encoding of a string, even if it accidentally mixes different encodings. 

There are many ways to access the content of a string in Tamgu. 

* With indexes:                        _str[1], str[2:4], str[-2 :]_
* With sub-strings:               _str["beg" : "end"]_
* With regular expressions: _str[r "ab%d+"]_

You can also link descriptions: _str["a": "e"][1:-1]_

But most importantly, you can modify the content of a string in the following way:

```C++
string s="The dog is <eating> a bone.";

s["<":">"][1:-1] = "biting"; //the string is now: The dog is <biting> a bone.

The first part isolates the substring between' <...>', 
then removes them to focus on the content of
this substring and replaces it with a new value.

```

#### Lambdas

The Lambdas in Tamgu are written in a formalism largely borrowed from _Haskell_. They allow you to describe in a very compact way complex manipulations on containers and character strings in particular. Haskell's most common methods are all available: _map, filter, fold, scan, take, drop, takeWhile, takeWhile, dropWhile, zip, repeat and cycle_ . You can also chain several methods with the dot operator: ".". Finally, the language offers the possibility to write functions that you can call from your code. 

Haskell

vector v =[1...10]; //v contains[1,2,3,3,4,5,6,7,8,9,10]
vector vres;

vres = <x | | x <- v, (x < 5)>; //we only keep elements inferior to 5
vres = <filter (<5) v>; //Same....

vres = <map (+1) v>; //vres is[2,3,4,5,6,7,8,9,10,11]

vres = <takeWhile (<15) . map (*2) v>; 
//we can chain methods: vres is[2,4,6,6,8,10,10,12,14], we stop at 14, since 2*8 = 16 > 15

### A _Haskell_ function to sort elements

```Haskell

v.shuffle(); // we shuffle the elements in the vector...

<fastsort([]) = []>; //if the list is empty, we return an empty "list"
<fastsort([fv:v]) = mn &&&& fv &&& mx where //we merge the different list...
    let mn = fastsort(<filter (<=fv) v>), //we apply our "sort" on the list that contains the elements smaller than fv
    let mx = fastsort(<filter (>fv) v>) //we apply our "sort" on the list that contains the elements larger than fv
>;
    
//we can call a functional definition as regular code
vres=fastsort(v); 

```


#### Prolog

Tamgu finally offers the ability to execute Prolog code directly into your own code:

```Prolog

concat([],?X,?X,?X).
concat([?H|?T],?Y, [?H|?Z]) :- concat(?T,?Y,?Z).

//You can mix freely your predicate definition with some regular tamgu code
v=concat(['english','russian','french'],['spanish'],?L);
println(v); //['english','russian','french','spanish']

```

## Data programming

Tamgu was designed for data programming. For example, you can directly integrate rules into your code and detect words or word patterns in a text in a single operation.

```C++

//We define some lexical rules (starting with a "@")
@food <-- burger.
@food <- tartar.


//Our rule: if a "food" word is found in a sentence, then we return a "meal" label 
meal <- "the", #food.

//We need a specific object to scan a sentence
annotator r;

//a sentence
string sentence="Here, the burger and the tartare are delicious."
vector v = r.parse(sentence); 

//Result: v = [[['meal',[10,16]],['meal',[25,32]]]]
//It reads: two "meal' were found at position 10-16 and position 25-32....

```

We used this mechanism to generate a _noise_ input to drive our model for the _workshop_ WMT 2019 for the task _robust translation_. 

#### Generation

You can also generate your own sentences, thanks to the DCG module of our Prolog...

```Prolog

sentence(s(?NP,?VP)) --> noun_phrase(?NP), verb_phrase(?VP).
noun_phrase(np(?D,?N)) --> det(?D), noun(?N).
verb_phrase(vp(?V,?NP)) --> verb(?V), noun_phrase(?NP).
det(d("the")) --> ["the"].
det(d("a")) --> ["a"].

noun(n("bird")) --> ["bird"].
noun(n("cat")) --> ["cat"].
verb(v("catches")) --> ["catches"].

//sentence._trace(true);
//we generate all possible interpretations....
vector vr=sentence(?Y,[],?X);
printjln(vr);

```
This gives us all the following variations:

```Prolog
sentence(['the','bird','catches','the','bird'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['the','bird','catches','the','cat'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['the','bird','catches','a','bird'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['the','bird','catches','a','cat'],[],s(np(d('the'),n('bird')),vp(v('catches'),np(d('a'),n('cat')))))
sentence(['the','cat','catches','the','bird'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['the','cat','catches','the','cat'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['the','cat','catches','a','bird'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['the','cat','catches','a','cat'],[],s(np(d('the'),n('cat')),vp(v('catches'),np(d('a'),n('cat')))))
sentence(['a','bird','catches','the','bird'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['a','bird','catches','the','cat'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['a','bird','catches','a','bird'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['a','bird','catches','a','cat'],[],s(np(d('a'),n('bird')),vp(v('catches'),np(d('a'),n('cat')))))
sentence(['a','cat','catches','the','bird'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('the'),n('bird')))))
sentence(['a','cat','catches','the','cat'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('the'),n('cat')))))
sentence(['a','cat','catches','a','bird'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('a'),n('bird')))))
sentence(['a','cat','catches','a','cat'],[],s(np(d('a'),n('cat')),vp(v('catches'),np(d('a'),n('cat')))))

```

Note that we have built a syntax tree for each generated sentence.
