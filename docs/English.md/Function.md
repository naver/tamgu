# Function

It is also possible to associate a function with a grammar. The signature of the function is as follows:  

```python
function grammarcall(string head, self structure, int pos)
```

This function is called for each new structure computed for a given head. If this function returns false, then the analysis of that rule fails. `pos` is the last position in the string up to which parsing took place.

## Example

```python
# This grammar recognizes a word or a number
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"@;

# This function is called for each new rule that succeeds
function callgrm(string head, self v, int ps) {
    println(head, v, ps);
    return true;
}

# We load our grammar
grammar g(r) with callgrm;

# We split a string into a character vector
string s = "Test 123 T234e";

# We apply the grammar to the character vector
map m = g.apply(s);
println(m);
```

Result:

```
word ['Test']
_bloc [{'word':['Test']}]
_bloc [' ']
number ['123']
_bloc [{'number':['123']}]
_bloc [' ']
code ['T234e']
_bloc [{'code':['T234e']}]
{'base':[{'word':['Test']},' ',{'number':['123']},' ',{'code':['T234e']}]}
```

## Modification of the structure

You can also modify the structure in this function, but you should be careful with your modifications.

```python
function callgrm(string head, self v, int ps) {
    # If the head is a word, we modify the inner string
    if (head == "word") {
        println(head, v);
        v[0] += "_aword";
    }
    return true;
}
```

Then in this case, the output is:

```
word ['Test']
{'base':[{'word':['Test_aword']},' ',{'number':['123']},' ',{'code':['T234e']}]}
```

## From within a rule

A function can also be called from within a rule. The signature is as follows:

```python
function rulecall(self structure, int pos)
```

This function is called from within the code rule. If it returns false, then the code rule fails.

```python
function callcode(self v, int ps) {
    println(head, v);
    return true;
}

# This grammar recognizes a word or a number
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c,callcode.
"@;

# We load our grammar
grammar g(r);

# We split a string into a character vector
string s = "Test 123 T234e";

# We apply the grammar to the character vector
map m = g.apply(s);
println(m);
```

## Example: Parsing HTML

`evaluate` is a basic method to replace every HTML entity with its UTF8 counterpart.