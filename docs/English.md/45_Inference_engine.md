# Tamgu Inference Engine

Tamgu embeds a powerful inference engine that can be seamlessly integrated with regular Tamgu instructions. This engine shares similarities with Prolog but has several unique features and syntax differences.

## Key Features

1. **Flexible Predicate Declaration**: Predicates don't require prior declaration for Tamgu to distinguish them from normal functions. However, if you need to use a predicate that will be implemented later in the code, you should declare it beforehand.

2. **Unique Variable Syntax**: Inference variables are denoted by preceding their names with a "?". For example: `?X`, `?Y`, `?Z`.

3. **Clause Termination**: Each inference clause ends with a "." instead of a ";".

4. **Term Declaration**: Terms can be declared beforehand as term variables. If not declared, their names must be preceded with a "?" like inference variables.

### Loading a large knowledgebase

If you have a file containing a very large knowledge base, consisting only of facts, it is more efficient to use loadfacts with the file path to speed up loading. This instruction should be placed at the very beginning of your file. It will start loading at compile time.

- `loadfacts(string path)`: Load a large knowledge base in memory


## Types

### Predicate

Used to declare predicates for inference clauses. It offers methods like:
- `name()`: Returns the predicate label
- `size()`: Returns the number of arguments
- `_trace(bool)`: Activates or deactivates tracing for this predicate when it's the calling predicate

### Term

Used to declare terms for inference clauses.

### List and Associative Map

Tamgu provides Prolog-like lists with the "|" operator for list decomposition:

```prolog
predicate alist;
alist([?A,?B|?C],[?A,?B],?C).
v = alist([1,2,3,4,5], ?X, ?Y);
println(v); // [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

Associative maps are implemented as Tamgu maps where argument order is significant:

```prolog
avalue(1,1).
avalue(10,2).
avalue(100,3).
avalue("fin",4).

assign({?X:?Y,?Z:?V}) :- avalue(?X,1), avalue(?Y,2), avalue(?Z,3), avalue(?V,4).
vector v = assign(?X);
println(v); // [assign({'100':'fin','1':10})]
```

### predicatevar or ?:-

Used to handle predicates and explore their names and values. It offers methods like:
- `map()`: Returns the predicate as a map
- `name()`: Returns the predicate name
- `query(predicate|name,v1,v2,v3)`: Builds and evaluates a predicate on the fly
- `remove()`: Removes the predicate from memory
- `store()`: Stores the predicate in memory
- `vector()`: Returns the predicate as a vector

```prolog
test(?X,?Q) :- ancestor(?X,?Q), female(?Q).
?:- var = test(?X,?Z);
println(var);
```

## Clauses

A basic clause structure:

```prolog
predicate(arg1, arg2, ..., argn) :- pred(arg...), pred(arg...), etc.
```

Facts can be declared as:

```prolog
predicate(val1, val2, ...) :- true.
// or
predicate(val1, val2, ...).
```

### Disjunction

Tamgu supports disjunctions in clauses using the ";" operator:

```prolog
parent(?X,?Y) :- mere(?X,?Y); pere(?X,?Y).
```

### Cut, fail, and stop

- `cut`: Expressed with "!"
- `fail`: Forces the failure of a clause
- `stop`: Stops the whole evaluation

## Functions

Tamgu provides Prolog-like functions such as:

- `asserta(pred(...))`: Inserts a predicate at the beginning of the knowledge base
- `assertz(pred(...))`: Inserts a predicate at the end of the knowledge base
- `retract(pred(...))`: Removes a predicate from the knowledge base
- `retractall(pred)`: Removes all instances of a predicate from the knowledge base
- `predicatedump(pred)` or `findall(pred)`: Returns predicates stored in memory as a vector

## Universal Predicate Name

Use "_" as a wildcard for predicate names when querying the knowledge base:

```prolog
parent(?A,?B) :- _(?A,?B).
```

## Tail Recursion

To activate tail recursion, add "#" to the name of the last element in the rule:

```prolog
traverse([],0).
traverse([?X|?Y],?A) :- println(?X,?Y), traverse#(?Y,?A).
```

Tail recursion can also be implemented with the `::-` operator:

```prolog
traverse([],0).
traverse([?X|?Y],?A) ::- println(?X,?Y), traverse(?Y,?A).
```

The `#` is then no longer necessary.

## Function Calls within Predicates and External Variable Unification

One of Tamgu's most powerful features is its ability to seamlessly integrate function calls within predicate rules and unify variables in external functions.

### Calling Functions from Predicates

You can call regular functions within predicate rules:

```prolog
function calculate_price(int base_price, int nights) {
    float total = base_price * nights;
    return total;
}

hotel_stay(?nights, ?price) :- 
    ?base_total is calculate_price(50, ?nights),
    ?price is ?base_total * 1.1.  // Adding 10% tax
```

### Unifying Variables in External Functions

Tamgu allows unifying variables within external functions using the special type `?_` for parameters that should be unified:

```prolog
function calling(string v, ?_ x) {        
    x = v + "1";
    return true;
}

predicate test;
test(?X, ?R) :- calling(?X, ?R).

vector v = test("Toto", ?R);
println(v);
```

### Advantages of This Approach

1. **Flexibility**: Leverage strengths of both logical and imperative programming paradigms.
2. **Extensibility**: Easy integration with existing Tamgu libraries and functions.
3. **Performance**: Offload computationally intensive tasks to optimized functions.
4. **Clarity**: Encapsulate complex operations in functions, keeping logical rules clean.

### Example: Database Integration

```prolog
function query_database(string user_id, ?_ user_data) {
    // Simulating a database query
    if (user_id == "12345") {
        user_data = {"name": "John Doe", "age": 30, "balance": 1000};
        return true;
    }
    return false;
}

function calculate_discount(?_ discount) {
    // Simulating some complex calculation
    discount = 0.15;  // 15% discount
    return true;
}

predicate eligible_for_loan;
eligible_for_loan(?user_id, ?loan_amount) :-
    query_database(?user_id, ?user_data),
    ?user_data["age"] >= 18,
    ?user_data["balance"] >= 500,
    calculate_discount(?discount),
    ?loan_amount is ?user_data["balance"] * (1 + ?discount).

vector v = eligible_for_loan("12345", ?loan);
println(v);
```

## Thread Execution within Predicates

Tamgu allows for the execution of threads from within predicate rules, enabling concurrent execution of tasks within the logical framework of predicates.

### Calling Threads in Predicates

You can define and call threads within predicate rules, but you must use the `waitonjoined()` instruction after the thread calls to ensure proper synchronization:

```prolog
thread thread_name(parameters, ?_ result) {
    // Thread operations
    return true;
}

predicate predicate_name;
predicate_name(args) :-
    thread_name(args1, ?result1),
    thread_name(args2, ?result2),
    // More thread calls if needed
    waitonjoined(),
    // Further processing with results.
```

### Example: Parallel API Calls

```prolog
thread t_prompt(string model, string p, ?_ res) {
    res = ollama.promptnostream(uri, model, p);
    return true;
}

string p1 = @"Return a Python function that iterates from 1 to 100 and displays each value on screen."@;
string p2 = @"Return a Python function that computes the square of all odd numbers between 1 and 100 and stores them in a list."@;
string p3 = @"Return a Python function that returns the last 10 characters of a string given as input."@;

predicate appel;
appel(?P1, ?P2, ?P3, [?Y1, ?Y2, ?Y3]) :-
    t_prompt("phi3", ?P1, ?Y1),
    t_prompt("phi3", ?P2, ?Y2),
    t_prompt("phi3", ?P3, ?Y3),
    waitonjoined().

vector v = appel(p1, p2, p3, ?V);
println(v);
```

### Important Considerations

1. **Use of `waitonjoined()`**: Crucial for ensuring all threads complete before the predicate continues.
2. **Concurrency**: Allows for concurrent execution of multiple tasks.
3. **Result Handling**: Results of thread executions can be captured in variables for further processing.
4. **Error Handling**: Proper error handling in threads is important.
5. **Resource Management**: Be mindful of resource usage when spawning multiple threads.

### Advantages of Threading in Predicates

1. **Parallelism**: Perform multiple operations concurrently.
2. **Integration**: Seamless integration of concurrent programming with logical programming.
3. **Flexibility**: Handle time-consuming operations without blocking entire predicate execution.
4. **Scalability**: Efficiently handle multiple similar tasks.

## Examples

### Ancestor Relationship

```prolog
ancestor(?X,?X).
ancestor(?X,?Z) :- parent(?X,?Y), ancestor(?Y,?Z).
parent("george","sam").
parent("george","andy").
parent("andy","mary").
parent("sam","christine").
male("george").
male("sam").
male("andy").
female("mary").
female("christine").
test(?X,?Q) :- ancestor(?X,?Q), female(?Q), ?X is not ?Q.
vector v = test(?X,?Z);
println(v);
```

### Natural Language Processing

```prolog
sentence(s(?NP,?VP)) --> noun_phrase(?NP), verb_phrase(?VP).
noun_phrase(np(?D,?N)) --> det(?D), noun(?N).
verb_phrase(vp(?V,?NP)) --> verb(?V), noun_phrase(?NP).
det(d("the")) --> ["the"].
det(d("a")) --> ["a"].
noun(n("bat")) --> ["bat"].
noun(n("cat")) --> ["cat"].
verb(v("eats")) --> ["eats"].

vector vr = sentence(["the", "bat", ?E, ?Y, ?N], [], ?X);
printjln(vr);
```

### List Concatenation

```prolog
predicate concat;
concat([], ?X, ?X).
concat([?H|?T], ?Y, [?H|?Z]) :- concat(?T, ?Y, ?Z).

concat.trace(true);
vector v = concat(["english", "russian", "french", "portuguese", "italian", "german"], ['spanish'], ?L);
println(v);
```

### Simple Hanoi Tower Solver

```prolog
predicate move;
move(1,?X,?Y,_) :- println('Move the top disk from ',?X,' to ',?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
predicatevar res;
res=move(3,"left","right","centre");
println(res);
```

### Animated Hanoi Tower

```prolog
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

This comprehensive documentation covers all aspects of Tamgu's inference engine, including its basic features, advanced capabilities like function and thread integration, and various examples demonstrating its versatility in different problem domains.
