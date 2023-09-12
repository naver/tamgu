## Processing Strings

There are different ways to process strings using a transducer.

### Lookup

The `lookup` function is used to determine if a word belongs to the transducer. In this case, it returns the lemma and features of the word. The transducer can return multiple solutions. If you want to retrieve all possible solutions, the recipient variable should be a vector.

Example:
```python
t.lookup("class") returns: class +Singular+Noun
```

You can also constrain the processing of a string with edit distance threshold and actions.

Example:
```python
t.lookup("cl iss", 1, a_change) returns: class +Singular+Noun
```

### Lookdown

The `lookdown` function is used to retrieve the correct surface form of a word using its lemma and features.

Example:
```python
t.lookdown("class +Plural+Noun") returns: classes
```

### Parse

The `parse` function splits a string into tokens and returns, for each token, its lemma+features as a vector of all possibilities.

Example:
```python
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
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Ve rb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

Note: The `parse` function also returns the position of each word in the initial sentence.