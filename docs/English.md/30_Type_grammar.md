# Type Grammar

The `grammar` type is designed to provide coders with a powerful way to describe complex string structures. It is particularly useful for detecting specific substrings in a text that involve digits, uppercase letters, or punctuations in a strict order.

## Methods

There are two functions that are exposed by this type:

1. `apply(string|vector)`: Applies a grammar to a text, transforming it into a vector of characters or a vector of tokens.
2. `load(rule, int skipblanks)`: Loads rules as a string or a vector of rules. The `skipblanks` parameter is optional and can have the values:
   - 0: All characters are taken into account in the grammar. This is the default value when `skipblanks` is omitted.
   - 1: White spaces and tabs are automatically skipped before applying a target to substrings. Trailing characters at the end of the strings are also skipped.
   - 2: All spaces, including carriage returns, are skipped.

The "in" operator can also be used with a grammar to detect if a string is compatible with the grammar.

## Rules

Rules can be implemented either as a single text or as a vector of strings, where each string represents a rule.

### Rule Format

The format of a rule is as follows:

```
head := (~) element [,;] element .
```

where `element` can be:
- A string: Enclosed in quotes or single quotes.
- ? : Any character.
- %a : Any alphabetic character.
- %c : Any lowercase character.
- %C : Any uppercase character.
- %d : A digit.
- %e : An emoji character.
- %H : A Hangul character.
- %r : A carriage return.
- %s : A space character.
- %S : A separator character (space or carriage return).
- %p : A punctuation.
- %? : The "?" character.
- %% : The "%" character.
- 0, 1, 2, ..., 9 : Any digit, represented by its character code.
- $string : A string of any length (same as "string").
- head : The head of another rule.

Negation: All elements can be negated with "~" except for heads.

Disjunction: Use ";" when you need a disjunction between two elements, "," otherwise.

Kleene star: Use "+" or "*" to loop for each of these elements.

Longest match: If you use "++" or "**", the loop will consume the string up to the most reachable element.

Optional: Use "(element)" for optional characters or heads.

All rules should end with a ".".

When a head name starts with an "_", the string is extracted but its label is not stored.

Specific cases:
- ?_ : Any character, but not stored.
- %a_ : Any alphabetic character, but not stored.
- %c_ : Any lowercase character, but not stored.
- %C_ : Any uppercase character, but not stored.
- %d_ : A digit, but not stored.
- %e_ : An emoji, but not stored.
- %H_ : A Hangul character, but not stored.
- %r_ : A carriage return, but not stored.
- %s_ : A space character, but not stored.
- %S_ : A separator character, but not stored.
- %p_ : A punctuation, but not stored.
- label_ : A call to a rule without storage.

The adjunction of a "_" at the end of these options allows for the recognition of a character or a group of characters that is not stored in the final result.

### Example

```python
# This grammar recognizes a word or a number, only for one string...
string r = @"
bloc := word;number.
word := %a+.
number := %d+.
"@;

# We load our grammar
grammar g(r);

# We apply it to the string "the"
map m = g.apply("the")
# Output: {'bloc':[{'word':['the']}]}

m = g.apply("123")
# Output: {'bloc':[{'number':['123']}]}

# However, if we apply this grammar to "Test 123", it will fail. We need to add
# two things to the grammar:
# a) Take into account spaces
# b) Loop to recognize every token in the string
string r = @"
base := bloc+.
bloc := word;number;%s.
word := %a+.
number := %d+.
"@;

# We have added a new disjunction with %s to take into account spaces. Then
# we have added a "base" rule that loops on bloc.
m = g.apply("Test 123")
# Output: {'base':[{'bloc':[{'word':['Test']}]},{'bloc':[' ']},{'bloc':[{'number':['123']}]}]}

# We can use the "_" operator to remove unnecessary information from the output, such as "bloc".
string r = @"
base := _bloc+.
_bloc := word;number;%s.
word := %a+.
number := %d+.
"@;

m = g.apply("Test 123")
# Output: {'base':[{'word':['Test']},' ',{'number':['123']}]}
```

## Sub-grammars

Sub-grammars are introduced within "[...]". In these brackets, it is possible to define a disjunction of character regular expression strings. These expressions are especially useful when applying a grammar to a vector of strings, where strings can be matched at the character level against the expression itself. Each expression should be separated from the following with a "|". You cannot call a rule from within brackets, so a string such as "dog" will be equivalent to "$dog".

### Example

```python
string dico = @"
test := %a, wrd,%a.
wrd := [%C,(" -"),%c+|test|be|dog|cat].
"@;

grammar g(dico);
ustring s = "The C -at drinks";
uvector v = s.tokenize();
vector res = g.apply(v);
println(res);
```

## Vector vs Map

If the recipient variable is replaced with a vector, the output structure is different. The head rule name is inserted into the final structure as the first element. For example, if we apply the grammar to the same string but with a vector as output, we obtain: `['base',['word','Test'],' ',['number','123'],' ',['code','T234e']]`.

## Input is a String or a Vector

If the input is a string, each detected character is appended to the output string. However, if the input is a vector of characters, the output result is kept as a vector of characters.

### Example

```python
# This grammar recognizes a word or a number
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"@;

# We load our grammar
grammar g(r);

# We split a string into a character vector
string s = "Test 123 T234e";
svector vs = s.split("");

# We apply the grammar to the character vector
vector v = g.apply(vs);
println(v);
```

## Function

It is possible to associate a function with a grammar. The signature of the function is as follows: `function grammarcall(string head, self structure, int pos)`. This function is called for each new structure computed for a given head. If this function returns false, then the analysis of that rule fails. `pos` is the last position in the string up to which parsing took place.

### Example

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
    println(head,v,ps);
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

## Modification of the Structure

The structure can also be modified in the function associated with the grammar, but caution should be exercised when making modifications.

### Example

```python
function callgrm(string head, self v, int ps) {
    # If the head is a word, we modify the inner string
    if (head == "word") {
        println(head,v);
        v[0] += "_aword";
    }
    return true;
}
```

## From Within a Rule

A function can also be called from within a rule. The signature is as follows: `function rulecall(self structure, int pos)`.

### Example

```python
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

## Parsing HTML

```python
# evaluate is a basic method to replace every HTML entity with its UTF8 counterpart.
function evalue(self s, int p) {
    s[1] = s[1].evaluate();
    return true;
}

# This is our HTML grammar
# We do not keep space characters between tags, hence: %s_ in object
string htmlgrm = @"
html := _object+.
_object := tag;%s_;text.
tag := "<",?+,">".
text := _characters,evalue.
_characters := ~"<"+.
"@;

# We compile our grammar
grammar ghtml(htmlgrm);

# We apply the grammar to an HTML text
vector rgram = ghtml.apply(html_text);
```