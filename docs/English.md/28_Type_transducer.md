# Type Transducer

This type is focused on storing and handling lexicons in a very compact and efficient way. It exposes the following methods:

## Methods

1. `add(container, bool norm, int encoding)`: Transforms a container (vector or map) into a transducer lexicon. If the container is a vector, it should have an even number of values.
2. `build(string input, string output, bool norm, int encoding)`: Builds a transducer file from a text file containing the surface form on the first line and the lemma+features on the next line.
3. `compilergx(string rgx, svector features, string filename)`: Builds a transducer file from regular expressions. The filename is optional, and the resulting automaton is stored in a file.
4. `load(string file)`: Loads a transducer file.
5. `lookdown(string lemFeat, byte lemma)`: Searches for a surface form that matches a lemma plus features. Lemma is optional. When it is 1 or 2, the string to compare against can be reduced to only a lemma. If lemma is 2, features are also returned. Note: The lemma should be separated from the features with a tab.
6. `lookup(string wrd, int threshold, int flags)`: Looks up a word using a transducer and a set of potential actions combined with a threshold. The last two arguments can be omitted. The available actions are:
   - `a_first`: The automaton can apply actions to the first character.
   - `a_change`: The automaton can change a character to another.
   - `a_delete`: The automaton can delete a character.
   - `a_insert`: The automaton can insert a character.
   - `a_switch`: The automaton transposes two characters.
   - `a_nocase`: The automaton checks if the two characters can match independently of their case.
   - `a_repetition`: The automaton accepts that a character is repeated a few times.
   - `a_vowel`: The automaton compares de-accentuated vowels together. For instance, "e" will match "é" or "è" but not "a".
   - `a_surface`: The automaton only returns the surface form.
7. `parse(string sentence, int option, int threshold, int flags)`: Analyzes a sequence of words using a transducer. The option can take the following values:
   - `0`: Returns only the surface forms that were recognized within the initial string.
   - `1`: Same as `0`, but with their offsets.
   - `2`: Returns the surface forms and the lemmas with their features that were recognized in the initial input.
   - `3`: Same as `2`, but with their offsets. The threshold and flags are optional and follow the same convention as for `lookup`.
8. `store(string output, bool norm, int encoding)`: Stores a transducer into a file. The last two parameters are optional.

## Format

The format of files compiled into lexicons either through `build` or `add` has a similar structure. In the case of a file, the first line should be a surface form, while the next line should be a lemma with some features, separated with a tab.

Example:
```
classes
class +Plural+Noun
class
class +Singular+Noun
```

The `build` function takes such a file as input and generates a file that contains the corresponding transducer based on these lines. The two other parameters are used when processing a word or text. 

- Normalization means that the lexicon can match words without being case sensitive. Hence, this lexicon will recognize "CLASS" as a word.
- The system has been implemented to recognize words in UTF8 encoding (actually the transducers are stored in Unicode). However, it is possible to tell the system how to take into account Latin encodings. For instance, you can provide the system with 5 as an encoding, which in this case refers to Latin 5, used to encode Cyrillic characters. The default value is Latin 1.

## Vector

In the case of a vector as input to `add`, the structure will be a little different. The even positions in the vector will be the surface form, while the odd positions will be the lemmas plus their features, again separated with a tab.

## Map

For a map, the key will be the surface form, and the value will be the lemmas with their features. A map might actually pose a problem to store ambiguous words.

## Processing Strings

There are different ways of processing strings with a transducer.

### `lookup`

`lookup` is used to detect if a word belongs to the transducer, and in this case, it returns its lemma and features. The transducer can return more than one solution. The recipient variable should be a vector if you want to retrieve all possible solutions.

Example:
```cpp
t.lookup("class") returns: class +Singular+Noun
```

You can constrain the processing of a string with edit distance threshold and actions.

```cpp
t.lookup("cliss", 1, a_change) returns: class +Singular+Noun
```

### `lookdown`

`lookdown` is used to retrieve the correct surface form of a word using its lemma and features.

Example:
```cpp
t.lookdown("class +Plural+Noun") returns: classes
```

### `parse`

`parse` splits a string into tokens and returns, for each token, its lemma+features as a vector of all possibilities.

Example:
```cpp
transducer t(_current + "english.tra");
string sentence = "The lady drinks a glass of milk.";
vector v = t.parse(sentence);
printjln(v);
```

Output:
```
['The', 'The +0+3+0+3+Prop+WordParticle+Sg+NOUN', 'the +0+3+0+3+Det+Def+SP+DET']
['lady', 'lady +4+8+4+8+Noun+Sg+NOUN']
['drinks', 'drink +9+15+9+15+Verb+Trans+Pres+3sg+VERB', 'drink +9+15+9+15+Noun+Pl+NOUN']
['a', 'a +16+17+16+17+Det+Indef+Sg+DET']
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Verb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

Note: `parse` also returns the position of each word in the initial sentence.

## Regular Expressions

The regular expressions processed by the transducer are very limited:

1. `%c`: Defines a character, where `c` is a UTF8 character.
2. `$..`: Defines a string.
3. `u-u`: Defines an interval between two Unicode characters.
4. `[..]`: Defines a sequence of characters.
5. `{...}`: Defines a disjunction of strings.
6. `.+`: The structure should occur at least once.
7. `(..)`: Defines an optional structure.
8. `!n`: Inserts a features structure along its number in the feature vector (n>=1).

Examples:
```cpp
transducer t;
// This expression recognizes Roman Numbers
t.compilergx("{DMCLXVI}+!1", ["\t+Rom"]);
// This expression recognizes any kind of numbers, including the decimal separator and exponential expressions.
t.compilergx("({-+}){0-9}+!1(%.{0-9}+!2({eE}({-+}){0-9}+!3))", ["+Card", "+Dec", "+Exp+Dec"]);
// To recognize ordinal numbers
t.compilergx("{[1st][2nd][3rd]}!1", ["+Ord"]);
t.compilergx("[3-9]([0-9]+)$th!1", ["+Ord"]);
// We want to recognize any strings made of the Greek alphabet
t.compilergx("{α-ω0-9}+!1", ["+Greek"]);

int i;
string s;
for (i in <945,970,1>) s += i.chr();
println(t.lookup("MMMDDD")); // MMMDDD +Rom
println(t.lookup("1234")); // 1234 +Card
println(t.lookup("1.234")); // 1.234 +Dec
println(t.lookup("1.234e-8")); // 1.234e-8 +Exp+Dec
println(t.lookup("1st")); // 1st +Ord
println(t.lookup("2nd")); // 2nd +Ord
println(t.lookup("3rd")); // 3rd +Ord
println(t.lookup("4th")); // 4th +Ord
println(t.lookup(s)); // αβγδεζηθικλμνξοπρςστυφχψ +Greek
```