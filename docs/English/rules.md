# Rules

A rule is a label associated with a body ending with ".":

* (@)label <- e1,e2,e3.

Rules are divided into two groups:
* lexicons rules, which starts with a "@"
* annotation rules.

## Syntax
A rule in Tamgu is a regular expression, which combines tokens and semantic categories as defined in the lexicons. A rule returns a label, which is associated with the span of the token sequence that was recognized.

The body is written after the "<-". However the character "←" can also be used.

```Java
goodfood <- #food,?+,delicious.
```

For instance, this rule says that if something is a _food_ and is followed by “was delicious” then the label **goodfood** is produced for this sequence of words. "?+" means a sequence of at least one token.

### Operators

The operators are the following:

*    {t1,t2,t3..}:    The token must match one of the elements in the list of token.
*    ?:        Any token.
*    #label:        A semantic label either produced with a rule or the lexicon
*    *,+ (:c):    Kleene operators, which can be bounded with a counter…
*    ~:        negation (only of atomic elements)
*    (..):        Optional sequence of tokens
*    [..]:        Sequence of tokens
*    \>..<:        The tokens in this sequence will be skipped in the final annotation.
*    \<call p1 p2\>:    A call to a function that returns true or false…
*    token:        A simple token, which must match our current word from the text
*    ‘rgx’:        A regular expression based on posix regular expression
*    “metas”:    A Tamgu specific regular expression based on the following meta-characters (see below).

```
%p:        A punctuation
%d:        A digit
%C:        An uppercase character
%c:        A lowercase character
%a:        An alphabetical character
%H:        A Hangul character
%s:        A space character
%r:        A carriage return
%S:        both space and carriage return
%x:        stands for a hexadecimal digit (abcdef0123456789ABCDEF)
%X:        escape the character X, where X is any character…
```

### Tamgu Regular Expression vs. Posix Regular expressions
Tamgu regular expressions provide a very simple schema to handle character expressions. A Tamgu regular expression can handle more than one word at a time. 

Tamgu exposes a list of meta-characters (see above for a list of them) than can be combined with characters and Kleene operators. A Tamgu regular expression is always encapsulated with "" (double quotes).

#### Example

* "%p+" : a string composed of only punctuations.
* "%H+" : a string composed of only Hangul characters.
* "test(s)": a string with an optional 's'.
* "Programming Language": a multiword expressions.

Note that the first example: "%p+" is equivalent to %p+. You can omit the quotes in this case. The same applies to %H+. However, if you have a combination of characters and multiple meta characters, you must use the double quotes.


#### Posix Regular expressions

Posix regular expressions are also available, but they cannot be used for the tokenisation process.
These expressions are embedded within '' (single quotes).

Example:

'(\d{1,3}):(\d{1,3}):(\d{1,3}):(\d{1,3})' this expression can recognised: 1:22:33:444 

## Lexicon rules
A lexicon rule is used to describe domain vocabulary. The body of a lexical rule is one element only. However, you can declare multiword expressions as Tamgu regular expressions.

**Important**: Only Tamgu regular expressions can be used for multiword expressions.

However, if you want your expression to be part of the tokenisation process (i.e. so that mwe can be recognised as one single token), only Tamgu regular expressions can be used.

## Examples
```Java
//Simply words associated with a "lexicon label"

@positive ← great.

@positive ← better.

//Another category

@food ← sushi. 

//This is a regular expression, the "s" is optional

//".." is a TAMGU regular expression

@food ← "food(s)".
```
## Annotation Rules

An annotation rule is a label associated with a body. The body can integrate different elements such as optional elements, disjunction of elements, regular expressions, skipped words etc. It returns a list of labels with their spans in the original text as offsets.

### Output

When a rule applies to a sequence or tokens, or to a token, each **token** is then associated with the _label_.
In other words, we can see _labels_ as classes. When a rule applies, the tokens from the initial sequence that are spanned by this rule are distributed to the _label_ or _class_ of this rule.

```

//Inital sequence is: the chicken was delicious. The chicken was very soft.

Rules:

@food <- chicken.

good <- #food, ?, delicious.

//aspect can now benefit from the application of the above rule
//note that we restrict the number of tokens between good and soft
//to a maximum of three...
aspect <- #good, ?+:3, soft.

When the rule "good" applies, the token "chicken"
receives the "good" label and can then be identified
through this new label, in subsequent rules.

```

## Examples

```Java
// Optional elements, #food matches a rule label or a lexicon label

food ← (#positive), #food+.

// We can skip words: >..<

// We can also specify that a word can match different labels.

service ← #personnel, >?*:5<, #[positive,service]. 
```

## Function Calls

A rule can call a function, which will investigate the current token, through more complex kinds of matching. For instance, a function might compare the current token against specific lexicons or against word embeddings.

The function must have at least two parameters. The fist one is the current token, the second one is the current annotator variable. However, it is possible to provide more variables if necessary.

```Java

function distance(string currenttoken, annotator a) {
if (currentoken.editdistance('is')<=2)
return(true);
return false;
}


label1 <- this, <distance>, a, thing.

// This rule will yield label1 on utterances:

- this is a thing
- this ist a thing
- this sis a thing
```

We can also implement a more general version of this function:

```Java

//We replace the "is", which was hard-coded with a variable
function distance(string currenttoken, annotator a, string s) {
if (currentoken.editdistance(s)<=2)
return(true);
return false;
}

//we call the function with one more parameter...
label1 <- this, <distance "is">, a, thing.

```

This function can implement any kind of behaviour. For instance, the token could be compared against word embedding or against a cosine distance between two words.

## annotator methods

The type "annotator" exposes many methods to investigate what was extracted by the rules so far.

We have organised them is different sections.

### Rule Selection

It is possible to have different annotators in parallel each corresponding to a certain set of labels.

For instance, you can associate to an annotator all rules, whose labels are lab1 and lab2.
The annotator variable, which would have been associated with these labels, will only apply the rules whose head belongs to this selection.

* select(uvector labels): select the rules, whose label is defined in labels.
* clear(): clear the label selection.
* selection(): return label selection.

### Lexicon

This method associates a general purpose lexicon to the annotator. A general purpose lexicon is a lexicon of a given natural language, for instance: English, French or Korean.
You can have more than one lexicon at a time. The fist lexicon, which is provided will be used to tokenise strings.

* lexicon(transducer t): Add a transducer to the list of lexicons.

### Parsing

The two following methods are used to apply rules to either a string or a string vectors.

* parse(ustring tokens,bool keeplex): apply rules to a ustring.
* apply(uvector tokens,bool keeplex): apply rules to a vector of tokens.

### While parsing

While parsing, it is possible to have access to the internal structures, which have extracted by the annotator so far. The following methods can be used in a callback function for instance to add more flesh to your analysis.

#### Label access
* checklabel(ustring label): Check if a label belongs to the annotated text.
* labels(): Return the list of all labels that were extracted.

#### Word access
* checkword(ustring wrd): Check if a word belongs to the annotated text and returns the list of annotations     
* words(): Return the list of all words found in the text.

#### Current token access
* current(): Return the current word or its index depending if the recipient variable is a string or an integer.
* word(long idx): Return the word at the position of the index itself...


# A Code example

```Java

//The lexicon...

//Simply words associated with a "lexicon label"
@positive ← great.
@positive ← better.
@positive ← good.
@positive ← accomodating.

//We compare the words with their lemma
@negative ← $overrate.
@negative ← $disappoint.

//Another category
@food ← sushi. 

//This is a regular expression, the "s" is optional
//".." is a TAMGU regular expression
//'..' is a posix regular expression
@food ← "food(s)".

//These are multiword expressions, which will be combine with the lexicon
//For a multiword tokenization
@food ← "lemon chicken".
@food ← "honey walnut prawn(s)". 

@service ← accomodating.

@personnel ← staff.
@personnel ← personnel.
@personnel ← $waiter.

//Rules...
// Optional elements, #food matches a rule label or a lexicon label
food ← (#positive), #food+.

// We can skip words: >..<
// We can also specify that a word can match different labels.
service ← #personnel, >?*:5<, #[positive,service]. 

//A disjunction is expressed with {...}
//%p is a punctuation.
positive ← "can%pt", say, enough, {of,about}.
positive ← no, $complaint.
service ← $tend, >?+:5<, my, $need.

negnot ← "not", #positive.
neg ← #negative.

//All these rules will be access through an annotator
annotator r;
//We load our english dictionary
transducer lex(_current+"english.tra");
//Which we associate with our annotator...
r.lexicon(lex); 

ustring u=@"
No Comparison...
I can't say enough about this place.
It has great sushi and even better service.
The waiters were extremely accomodating and tended to my every need.
I've been to this restaurant over a dozen times with no complaints to date.

Overrated and not good.
– I was highly disappointed in the food at Pagoda.
The lemon chicken tasted like sticky sweet donuts and the honey walnut prawns, the few they actually give you.....were not good.
The prices are outrageous, especially since the food was actually less satisfying than most neighborhood Chinese establishments.
Nice ambience, but highly overrated place.
I will not go back.
The waiters were extremely accomodating and tended to my taste and to my need.

"@;

//We apply our rules to the text above...
vector res=r.parse(u,true);
println(res);

//The result is: 
[['positive',[20,25],[26,29],[30,36],[37,42]],['food',[62,67],[68,73]],
['service',[103,110],[126,138]],['positive',[221,223],[224,234]],
['neg',[245,254]],
['negnot',[259,262],[263,267]],['neg',[284,296]],['food',[304,308]],
['food',[324,337]],['food',[378,397]],['negnot',[439,442],[443,447]],
['food',[497,501]],['neg',[604,613]],['service',[645,652],[668,680]],
['service',[685,691],[711,713],[714,718]]]


```

# BNF description
```kotlin

# A Kleene operator can be bounded with a number
kleene := [%+^%*] (%: anumber)

# a regular expression based on meta enclosed in ""
metas := ".+"

# a regular expression is enclosed in ''
rgx := '.+'

#any character
any := %?

# a meta character %x
meta := %% .

# A multilabel is a token associated with more than one label
multilabel := %# %[ word [%, word]* %]
label := %# word

# A lemma is the default form of a word as yielded by a lexicon
lemma := %$ word

# A call to an external function...
callmatch := %< word parameters* %>

# a token is a combination of all these
token := [callmatch^multilabel^label^lemma^word^any^meta^metas^rgx^punct] (kleene)

%listoftokens := %{ annotation [%, annotation]* %} (kleene)
%sequenceoftokens :=  %[ annotation [%, annotation]* %] (kleene)
%optionaltokens := %( annotation [%, annotation]* %)
%removetokens := %> annotation [%, annotation]* %<

%annotation := [(not) listoftokens]^sequenceoftokens^optionaltokens^removetokens^[(not) token]

%annotationrule := (%@) word [%←^[%< %-]] annotation [%, annotation]* %.

```
