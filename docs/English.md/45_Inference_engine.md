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

## Different Specific Types

### predicate

Used to declare predicates for inference clauses. It offers methods like:
- `name()`: Returns the predicate label
- `size()`: Returns the number of arguments
- `_trace(bool)`: Activates or deactivates tracing for this predicate when it's the calling predicate

```prolog
predicate solve;

solve(?X, ?Y) :-
    ...

vector v = solve(?X,?Y);
```

### term

Use to declare terms for inference clauses.

```prolog
term one, second;

pred(one(?X,?Y), ?L) :-
    ...

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

## Recipient Types

When calling a predicate in Tamgu, the type of recipient variable used affects how the inference engine processes the query. If a vector is used as the recipient, the engine explores all possible paths in the inference graph, returning multiple solutions if they exist. When a predicatevar is employed, the engine searches for just one solution, stopping after finding the first valid result. In cases where no variables need to be unified in the call (i.e., all arguments are ground terms), a Boolean recipient can be used to simply force the execution of the predicate without capturing any specific results. This flexibility allows users to tailor their queries to their specific needs, whether they require exhaustive search, single-solution efficiency, or mere execution of facts.


### vector: Exploring All Possible Solutions

When the recipient of a predicate call is a vector, Tamgu's inference engine explores all possible paths in the inference graph, returning multiple solutions if they exist. This is useful when you want to find all matching results for a given query.

Example:
```prolog
predicate parent;
parent("John", "Alice").
parent("John", "Bob").
parent("Mary", "Alice").

vector v = parent("John", ?Child);
println(v);  // Output: [parent("John", "Alice"), parent("John", "Bob")]
```

### predicatevar or ?:-: Finding One Solution

Used to handle predicates and explore their names and values. It offers methods like:
- `map()`: Returns the predicate as a map
- `name()`: Returns the predicate name
- `vector()`: Returns the predicate as a vector

When a predicatevar is used as the recipient, the engine searches for just one solution, stopping after finding the first valid result. This is efficient when you only need a single matching solution.

Example:
```prolog
predicate sibling;
sibling(?X, ?Y) :- parent(?Z, ?X), parent(?Z, ?Y), ?X != ?Y.

?:- result = sibling("Alice", ?Sibling);
println(result);  // Output: sibling("Alice", "Bob")
println(result.map());  // Output: {'0':'Alice', '1':'Bob', 'name':'sibling'}
```

### Boolean Recipient: Executing Without Variable Unification

In cases where no variables need to be unified in the call (i.e., all arguments are ground terms), a Boolean recipient can be used to simply force the execution of the predicate without capturing any specific results. This is useful for predicates that perform actions or check conditions without needing to return a value.

Example:
```prolog
predicate add_fact;
add_fact(?X, ?Y) :- assertz(parent(?X, ?Y)).

bool success = add_fact("Emma", "Olivia");
println(success);  // Output: true

// Now we can query the newly added fact
vector check = parent("Emma", ?Child);
println(check);  // Output: [parent("Emma", "Olivia")]
```

These different recipient types provide flexibility in how predicates are queried and how results are handled, allowing users to tailor their logic programming approach to their specific needs.


### Disjunction

Tamgu supports disjunctions in clauses using the ";" operator:

```prolog
parent(?X,?Y) :- mere(?X,?Y); pere(?X,?Y).
```

### Cut, fail, and stop

- `cut`: Expressed with "!"
- `fail`: Forces the failure of a clause
- `stop`: Stops the whole evaluation

### Assert

Tamgu provides specific predicates to handle the knowledgebase:

- `asserta(pred(...))`: Inserts a predicate at the beginning of the knowledge base
- `assertz(pred(...))`: Inserts a predicate at the end of the knowledge base
- `retract(pred(...))`: Removes a predicate from the knowledge base
- `retractall(pred)`: Removes all instances of a predicate from the knowledge base
- `findall(template, pred(..), ?L)`: Returns a list of facts that matches `pred`.

```Prolog
// We return the object itself
test(?L) :-
    findall(obj(?X,?Y), obj(?X,?Y), ?L).

// We only return ?X
test(?L) :-
    findall(?X, obj(?X,?Y), ?L).

```

### Knowledgebase Functions

Tamgu also provides different ways to extract this database by name:

- `predicatedump()`: Returns all knowledgebase predicates in memory
- `predicatedump(string p1, string p2, ...)`: Returns all knowledgebase predicates in memory corresponding to one of these names. 
- `predicatedump(svector predicatenames)`: The predicates can also be provided as a vector.
- `predicatestore(string filename,...)` : Works in the same way as `predicatedump`, but stores the database in a file.

### Universal Predicate Name

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
# Integrating Frames with Predicates in Tamgu

Tamgu offers a powerful feature that allows seamless integration of object-oriented programming (using frames) with logic programming (using predicates). This section explores how to leverage this integration for complex data structures and operations within the inference engine.

## Frames in Tamgu

Frames in Tamgu are equivalent to classes in other object-oriented languages. They allow you to define complex data structures with associated methods.

### Example: The `Vectordb` Frame

```prolog
frame Vectordb {
    string key;
    fvector embedding;
    
    function _initial(string k) {
        key = k;
        //We suppose the existence of a get_embedding method
        //that transforms a string into a fvector
        embedding = get_embedding(key);
    }
    
    function string() {
        return key;
    }
    
    function ==(fvector e) {
        return (cosine(embedding, e) > 0.9);
    }
    
    function ==(Vectordb e) {
        return (cosine(embedding, e.embedding) > 0.9);
    }
}
```

This frame, `Vectordb`, demonstrates several key features:

1. Data members: `key` (a string) and `embedding` (a float vector).
2. An initializer method `_initial`.
3. A string conversion method.
4. Custom equality comparison methods using cosine similarity.

## Integrating Frames with Predicates

Frames can be used within predicates, allowing for complex operations and queries on structured data.

### Automatic Unification with Custom Equality

```prolog
adding([]).
adding([?K | ?R]) :-
    ?X is Vectordb(?K),
    assertz(embed(?X, ?K)),
    adding(?R).

//Note that since `?P` is an `Vectordb` object, the `==` methods
//in Vectordb will automatically be applied when querying `embed(?P, ?K)`.
checking(?P, ?R) :-
    findall(?K, embed(?P, ?K), ?R).
```

These predicates demonstrate how to work with frame objects:

1. `adding`:
   - Takes two lists: keys and Vectordb.
   - Creates `Vectordb` objects and asserts them into the knowledge base.
   - Uses recursion to process all elements.

2. `checking`:
   - Takes an `Vectordb` object and finds all matching keys in the knowledge base.
   - Utilizes the custom equality comparison defined in the frame.

An important feature to note is how Tamgu's unification mechanism interacts with custom equality definitions in frames. When using predicates like `findall`, Tamgu automatically leverages the "==" functions defined in the frame.

In our `Vectordb` example:

```prolog
checking(?P, ?R) :-
    findall(?K, embed(?P, ?K), ?R).
```

When this predicate is called, the `findall` operation automatically uses the "==" function defined in the `Vectordb` frame to compare the input `?P` with each `Vectordb` object stored in the knowledge base. This means:

1. The cosine similarity comparison is automatically applied without explicit calls in the predicate.
2. The threshold for similarity (>0.9 in our example) is respected without additional logic in the predicate.
3. The same predicate can work with different frame types, as long as they define appropriate "==" functions.

This automatic use of custom equality in unification showcases Tamgu's seamless integration of object-oriented concepts with logic programming. It allows for sophisticated matching and querying operations while maintaining clean and concise predicate definitions.

## Usage Example

```prolog
//Note that since the call to this predicate has no variable, we use a Boolean recipient variable
//to force the execution.
bool x = adding(["This is a test", "This is a second test", "This is a third test"]);

?:- r = checking(Vectordb("This is a test"), ?R);
println(r);
```

This example shows how to:
1. Add multiple key-embedding pairs to the knowledge base.
2. Query the knowledge base for similar Vectordb.

## Key Benefits

1. **Object-Oriented Logic Programming**: Combine the strengths of OOP and logic programming paradigms.
2. **Custom Comparisons**: Define and use domain-specific comparison logic within predicates.
3. **Dynamic Knowledge Base**: Easily add structured data to the knowledge base during runtime.
4. **Complex Queries**: Perform sophisticated queries on structured data using predicate logic.


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

## DCG

Tamgu also accepts DCG rules (Definite Clause Grammar), with a few modifications with the original definition. First, Prolog variables should be denoted with ?V as in the other rules. Third, atoms can only be declared as strings.

### Example:
```prolog

term s,np,vp,d,n,v;

sentence(s(?NP,?VP)) --> noun_phrase(?NP), verb_phrase(?VP).
noun_phrase(np(?D,?N)) --> det(?D), noun(?N).
verb_phrase(vp(?V,?NP)) --> verb(?V), noun_phrase(?NP).
det(d("the")) --> ["the",?X], {?X is "big"}.
det(d("a")) --> ["a"].
noun(n("bat")) --> ["bat"].
noun(n("cat")) --> ["cat"].
verb(v("eats")) --> ["eats"].

//we generate all possible interpretations...
vector vr=sentence(?Y,[],?X);
println(vr);
```

## Mapping methods to predicates.
Most object methods are mapped into predicates, in a very simple way. For instance, if a string exports the method “trim”, then a “p_trim” with two variables is created. Each method is mapped to a predicate in this fashion. For each
method, we add a prefix: “p_” to transform this method into a predicate.

The first argument of this predicate is the head object of the method, while the last parameter is the result of applying this method to that object. Hence, if s is a string, s.trim() becomes p_trim(s,?X), where ?X is the result of applying trim to s. If ?X is unified, then the predicate will check if the ?X is the same as s.trim().

### Example 1

```prolog
compute(?X,?Y) :- p_log(?X,?Y).
```
### Methods: between(?X,?B,?E), pred(?X,?Y) and succ(?X,?Y)

- `between(?X,?B,?E)` checks if the value ?X is between ?B and ?E.
- `pred(?X,?Y)` returns the predecessor of ?X. pred can also be used as term, but in that case, it only uses one argument.
- `succ(?X,?Y)` returns the successor of ?X. succ can also be used as term, but in that case, it only uses one argument.

### Common mistakes with Tamgu variables.

If you use regular variables in predicates, such as strings, integers or any
other sorts of variables, you need to remember that these variables are used
in predicates as comparison values. An example might clarify a little bit what
we mean.

#### Example 1
```prolog
string s="test";
string sx="other";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
Execution:
r:0=comp(s,3) --> comp(other,?X172) --> Fail
This clause has failed, because s and sx have different values.
```

#### Example 2

```prolog
string s="test"; //now they have the same values
string sx="test";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
Execution:
r:0=comp(s,3) --> comp(test,?X173)
e:0=comp(test,3) --> println(s)test
success:1=comp('test',3)
```

Be careful when you design your clauses, to use external variables as
comparison sources and not as instantiation. If you want to pass a string value
to your predicate, then the placeholder for that string should be a predicate
variable.

#### Example 3

```prolog
string sx="test";
predicate comp;
comp._trace(true);
comp(?s,3) :- println(?s).
comp(sx,?X);
Execution:
r:0=comp(?s,3) --> comp(test,?X176)
e:0=comp('test',3) --> println(?s177:test)test
success:1=comp('test',3)
```


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
