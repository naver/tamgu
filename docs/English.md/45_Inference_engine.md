# Inference Engine

Tamgu embeds an inference engine, which can be freely mixed with regular Tamgu instructions. This inference engine is very similar to Prolog but with some particularities:

a) Predicates do not need to be declared beforehand in order for Tamgu to distinguish predicates from normal functions. However, if you need to use a predicate that will be implemented later in the code, you need to declare it beforehand.

b) You do not need to declare inference variables. However, their names are very different from traditional Prolog names: they must be preceded with a "?".

c) Each inference clause finishes with a "." and not a ";".

d) Terms can be declared beforehand (as term variables). However, if you do not want to declare them, you must precede their name with a "?" as for inference variables.

e) Probabilities might be attached to predicates, which are used to choose as a first path the one with the highest probabilities.

**Note**: For an adequate description of the Prolog language, please consult the appropriate documentation.

## Types

Tamgu exposes three specific types for inference objects:

### Predicate

This type is used to declare predicates, which will be used in inference clauses. This type exposes the following methods:

1. `name()`: return the predicate label
2. `size()`: return the number of arguments
3. `_trace(bool)`: activate or deactivate the trace for this predicate when it is the calling predicate.

### Term

This type is used to declare terms, which will be used in inference clauses (see the NLP example below).

### Other Inference Types: List and Associative Map

Tamgu also provides the traditional lists à la Prolog, which can be used with the "|" operator to handle list decomposition (see the NLP example below for a demonstration of this operator).

**Example:**

```C++
predicate alist;
// in this clause, C is the rest of the list...
alist([?A,?B|?C],[?A,?B],?C) :- true.
v=alist([1,2,3,4,5], ?X,?Y);
println(v); // [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

Tamgu also provides an associative map, which is implemented as a Tamgu map, but in which the argument order is significant.

**Example:**

```C++
predicate assign,avalue;
avalue(1,1) :- true.
avalue(10,2) :- true.
avalue(100,3) :- true.
avalue("fin",4) :- true.
assign({?X:?Y,?Z:?V}) :- avalue(?X,1), avalue(?Y,2), avalue(?Z,3), avalue(?V,4).
vector v=assign(?X);
println(v); // [assign({'100':'fin','1':10})]
```

As you can see in this example, both keys and values can depend on inference variables. However, the order in which these association key:value are declared is important. Thus, {?X:?Y,?Z:?V} is different from {?Z:?V,?X:?Y}.

### PredicateVar

This type is used to handle predicates to explore their names and values. A PredicateVar can be seen as a function, whose parameters are accessible through their position in the argument list as a vector. This type exposes the following methods:

1. `map()`: return the predicate as a map: [name:name,'0':arg0,'1':arg1,...]
2. `name()`: return the predicate name
3. `query(predicate|name,v1,v2,v3)`: build and evaluate a predicate on the fly
4. `remove()`: remove the predicate from memory
5. `remove(db)`: remove the predicate from the database db
6. `size()`: return the number of arguments
7. `store()`: store the predicate in memory
8. `store(db)`: store the predicate value into the database db
9. `vector()`: return the predicate as a vector: [name,arg0,arg1,...]

It should be noted that the method "predicate", which exists both for a map and a vector, transforms the content of a vector or a map back into a predicate as long as their content mimics the predicate output of vector() and map().

**Example:**

```C++
vector v=['female','mary'];
predicatevar fem;
fem=v.predicate(); // we transform our vector into a predicate
fem.store(); // we store it in the fact base
v=fem.query(female,?X); // We build a predicate query on the fly
v=fem.query(female,'mary'); // We build a predicate query with a string
```

## Clauses

A clause is defined as follows:

```C++
predicate(arg1,arg2,...,argn) :- pred(arg...),pred(arg,...), etc. ;
```

Fact in a knowledge base. A fact can be declared in a program with the following instruction:

```C++
predicate(val1,val2,...).
```

or

```C++
predicate(val1,val2,...) :- true.
```

is actually equivalent to the above syntax.

**loadfacts(pathname)**: loading a large knowledge base

If you have a file storing a very large knowledge base, containing only facts, it is more efficient to use `loadfacts` with the file pathname to speed up loading. This instruction should be placed at the very beginning of your file. It will start loading at compile time.

**Disjunction**

Tamgu also accepts disjunctions in clauses with the operator ";", which can be used in lieu of "," between predicates.

**Example:**

```C++
predicate mere,pere;
mere("jeanne","marie").
mere("jeanne","rolande").
pere("bertrand","marie").
pere("bertrand","rolande").
predicate parent;
// We then declare our rule as a disjunction...
parent(?X,?Y) :- mere(?X,?Y);pere(?X,?Y).
parent._trace(true);
vector v=parent(?X,?Y);
println(v);
```

**Result:**

```
r:0=parent(?X,?Y) --> parent(?X6,?Y7)
e:0=parent(?X8,?Y9) --> mere(?X8,?Y9)
k:1=mere('jeanne','marie').
success:2=parent('jeanne','marie')
k:1=mere('jeanne','rolande').
success:2=parent('jeanne','rolande')
[parent('jeanne','marie'),parent('jeanne','rolande')]
```

**Cut, Fail, and Stop**

Tamgu also provides a cut, which is expressed with the traditional "!". It also provides the keyword fail, which can be used to force the failure of a clause. Stop stops the whole evaluation.

**Functions**

Tamgu also provides some regular functions from the Prolog language such as:

**Function asserta(pred(...))**

This function asserts (inserts) a predicate at the beginning of the knowledge base. Note that this function can only be used within a clause declaration.

**assertz(pred(...))**

This function asserts (inserts) a predicate at the end of the knowledge base. Note that this function can only be used within a clause declaration.

**retract(pred(...))**

This function removes a predicate from the knowledge base. Note that this function can only be used within a clause declaration.

**retractall(pred)**

This function removes all instances of predicate "pred" from the knowledge base. If retractall is used without any parameters, then it cleans the whole knowledge base. Note that this function can only be used within a clause declaration.

**Function: predicatedump(pred) or findall(pred)**

This function, when used without any parameters, returns all predicates stored in memory as a vector. If you provide the name of a predicate as a string, then it dumps as a vector all the predicates with the specified name.

**Example:**

```C++
// Note that you need to declare "parent" if you want to use it in an assert
predicate parent;
adding(?X,?Y) :- asserta(parent(?X,?Y)).
adding("Pierre","Roland");
println(predicatedump(parent));
```

**Universal Predicate Name**

When looking for facts in a knowledge base, it is possible to query without a specific predicate name. In that case, one can use: "_" in lieu of the predicate name.

**Example:**

```C++
// Our knowledge base
father("george","sam").
father("george","andy").
mother("andy","mary").
mother("sam","christine").
// Our rule, it will match any facts above
parent(?A,?B) :- _(?A,?B).
```

We can also use some specific variables: _0.._9, which can return what was the predicate name it did match against.

**Example:**

```C++
// Our rule, it will match any facts above
parent(?A,?B,?P) :- _1(?A,?B), ?P is _1.
```

This code returns:

```
parent("andy","mary","mother")
parent("sam","christine","mother")
parent("george","sam","father")
parent("george","andy","father")
```

**Tail Recursion**

Tail recursion is a mechanism that transforms a recursion into an iterative process. To activate the tail recursion mechanism, you need to modify the name of the last element of the rule by adding a "#". The tail recursion is then activated if this last element corresponds to the current rule.

Tail recursion transforms the traversal of a vector for instance into an iterative process. However, at the end of the process, the initial value of the vector is lost.

**Example:**

```C++
vector v = [1..10];
// End of recursion
traverse([],0).
// The last element is marked for tail recursion
traverse([?X|?Y],?A) :- println(?X,?Y), traverse#(?Y,?A).
vector vv = traverse(v,?A);
println(vv);
```

**Result:**

```
1 [2,3,4,5,6,7,8,9,10]
2 [3,4,5,6,7,8,9,10]
3 [4,5,6,7,8,9,10]
4 [5,6,7,8,9,10]
5 [6,7,8,9,10]
6 [7,8,9,10]
7 [8,9,10]
8 [9,10]
9 [10]
10 [] // The first argument is now []
[traverse([],0)]
```

**Callback Function**

A predicate can be declared with a callback function, whose signature is the following:

```C++
function OnSuccess(predicatevar p, string s) {
    println(s,p);
    return true;
}
```

The second argument in the function corresponds to the parameter given to parent in the declaration. If the function returns true, then the inference engine tries other solutions; otherwise, it stops.

It also possible to unify a variable in a call. In that case, the variable type should be: `?_`.

```C++

//We define a function, which is going to unify the variable x:
function calling(string v, ?_ x) {        
    x = v + 1;
    return true;
}

//In this call, the variable ?R will be unified within calling
test(?X,?R) :-
	calling(?X, ?R).

vector v = test("Toto", ?R);
println(v);
```


**Result:**

If we run our above example, we obtain:

```
Parent: parent('John','Mary')
Parent: parent('John','Peter')
```

**DCG**

Tamgu also accepts DCG rules (Definite Clause Grammar) with a few modifications to the original definition. First, Prolog variables should be denoted with ?V as in the other rules. Third, atoms can only be declared as strings.

**Example:**

```C++
predicate sentence,noun_phrase,verb_phrase;
term s,np,vp,d,n,v;
sentence(s(np(d("the"),n("cat")),vp(v("eats"),np(d("a"),n("bat"))))) --> [].
```

**Launching an Evaluation**

Evaluations are launched exactly in the same way as a function would. You can of course provide as many inference variables as you want, but you can only launch one predicate at a time, which imposes that your expression should first be declared as a clause if you want it to include more than one predicate.

**Important**: If the recipient variable is a vector, then all possible analyses will be provided. The evaluation tree will be fully traversed. If the recipient variable is anything else, then whenever a solution is found, the evaluation is stopped.

**Mapping Methods to Predicates**

Most object methods are mapped into predicates in a very simple way. For instance, if a string exports the method "trim", then a "p_trim" with two variables is created. Each method is mapped to a predicate in this fashion. For each method, we add a prefix: "p_" to transform this method into a predicate. The first argument of this predicate is the head object of the method, while the last parameter is the result of applying this method to that object. Hence, if s is a string, s.trim() becomes p_trim(s,?X), where ?X is the result of applying trim to s. If ?X is unified, then the predicate will check if ?X is the same as s.trim().

**Example:**

```C++
compute(?X,?Y) :- p_log(?X,?Y).
between(?X,?B,?E), succ(?X,?Y).
```

**Common Mistakes with Tamgu Variables**

If you use common variables in predicates, such as string s, integer s, or any other sorts of variables, you need to remember that these variables are used in predicates as comparison values. An example might clarify a little bit what we mean.

**Example 1**

```C++
string s="test";
string sx="other";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

Execution:

```
r:0=comp(s,3) --> comp(other,?X172) --> Fail
```

This clause has failed because s and sx have different values.

**Example 2**

```C++
string s="test"; // now they have the same values
string sx="test";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

Execution:

```
r:0=comp(s,3) --> comp(test,?X173)
e:0=comp(test,3) --> println(s)test
success:1=comp('test',3)
```

Be careful when you design your clauses to use external variables as comparison sources and not as instantiation. If you want to pass a string value to your predicate, then the placeholder for that string should be a predicate variable.

**Example 3**

```C++
string sx="test";
predicate comp;
comp._trace(true);
comp(?s,3) :- println(?s).
comp(sx,?X);
```

Execution:

```
r:0=comp(?s,3) --> comp(test,?X176)
e:0=comp('test',3) --> println(?s177:test)test
success:1=comp('test',3)
```

**Examples**

**Hanoi Tower**

The following program solves the Hanoi tower problem for you.

```C++
predicate move;
move(1,?X,?Y,_) :- println('Move the top disk from ',?X,' to ',?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
predicatevar res;
res=move(3,"left","right","centre");
println(res);
```

**Ancestor**

With this program, you can find the common female ancestor between different people parent relationships.

```C++
predicate ancestor,parent,male,female,test;
ancestor(?X,?X) :- true.
ancestor(?X,?Z) :- parent(?X,?Y),ancestor(?Y,?Z).
parent("george","sam") :- true.
parent("george","andy") :- true.
parent("andy","mary") :- true.
male("george") :- true.
male("sam") :- true.
male("andy") :- true.
female("mary") :- true.
test(?X,?Q) :- ancestor(?X,?Q), female(?Q).
test._trace(true);
vector v=test("george",?Z);
println(v);
```

**An NLP Example**

This example corresponds to the clauses that have been generated out of the previous DCG grammar given as an example.

```C++
predicate sentence,noun_phrase,det,noun,verb_phrase,verb;
term P,SN,SV,dét,nom,verbe;
sentence(?S1,?S3,P(?A,?B)) :- noun_phrase(?S1,?S2,?A), verb_phrase(?S2,?S3,?B).
noun_phrase(?S1,?S3,SN(?A,?B)) :- det(?S1,?S2,?A), noun(?S2,?S3,?B).
verb_phrase(?S1,?S3,SV(?A,?B)) :- verb(?S1,?S2,?A), noun_phrase(?S2,?S3,?B).
det(["the"|?X],?X,dét("the")) :- true.
det(["a"|?X],?X,dét("a")) :- true.
noun(["cat"|?X],?X,nom("cat")) :- true.
noun(["dog"|?X],?X,nom("dog")) :- true.
verb(["eats"|?X],?X,verbe("eats")) :- true.
vector v;
v=sentence(?X,[],?A);
println("All the sentences that can be generated:",v);
// we analyze a sentence
v=sentence(["the","dog","eats","a","bat"],[],?A);
println("The analysis:",v);
```

**Animated Hanoi Tower**

The code below displays an animation in which disks are moved from one column to another. It merges both graphics and predicates.

```C++
predicate move;
map columns={'left':[70,50,30],'centre':[],'right':[]};
function disk(window w,int x,int y,int sz,int position) {
    int start=x+100-sz;
    int level=y-50*position;
    w.rectanglefill(start,level,sz*2+20,30,FL_BLUE);
}
function displaying(window w,self o) {
    w.drawcolor(FL_BLACK);
    w.font(FL_HELVETICA,40);
    w.drawtext("Left",180,200);
    w.drawtext("Centre",460,200);
    w.drawtext("Right",760,200);
    w.rectanglefill(200,300,20,460,FL_BLACK);
    w.rectanglefill(100,740,220,20,FL_BLACK);
    w.rectanglefill(500,300,20,460,FL_BLACK);
    w.rectanglefill(400,740,220,20,FL_BLACK);
    w.rectanglefill(800,300,20,460,FL_BLACK);
    w.rectanglefill(700,740,220,20,FL_BLACK);
    vector left=columns['left'];
    vector centre=columns['centre'];
    vector right=columns['right'];
    int i;
    for (i=0;i<left;i++)
        disk(w,100,740,left[i],i+1);
    for (i=0;i<centre;i++)
        disk(w,400,740,centre[i],i+1);
    for (i=0;i<right;i++)
        disk(w,700,740,right[i],i+1);
}
window w with displaying;
function moving(string x,string y) {
    columns[y].push(columns[x][-1]);
    columns[x].pop();
    w.redraw();
    pause(0.5);
    return true;
}
move(1,?X,?Y,_) :- moving(?X,?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
thread hanoi() {
    move(3,"left","right","centre");
}
function launch(button b,self o) {
    hanoi();
}
button b with launch;
w.begin(50,50,1000,800,"HANOI");
b.create(20,20,60,30,FL_Regular,FL_NORMAL_BUTTON,"Launch");
w.end();
w.run();
```