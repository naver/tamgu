## Format

The files that are compiled into lexicons, either through build or add, have a similar structure. 

In the case of a file, the first line should be the surface form, while the next line should be the lemma with its features, separated by a tab. This pattern continues for each entry in the file:

```
classes
class  +Plural+Noun
class
class  +Singular+Noun
etc.
```

The `build` function takes such a file as input and generates a file that contains the corresponding transducer based on these lines. The two other parameters are used when processing a word or a text.

a) Normalization means that the lexicon can match words without being case sensitive. Hence, this lexicon will recognize "CLASS" as a word.

b) The system has been implemented to recognize words in UTF8 encoding (actually the transducers are stored in Unicode). However, it is possible to specify how the system should handle Latin encodings. For instance, you can provide the system with "5" as an encoding, which in this case refers to Latin 5, used to encode Cyrillic characters. The default value is Latin 1.

## Vector

In the case of a vector as input to `add`, the structure will be a little different. The even positions in the vector will be the surface form, while the odd positions will be the lemmas plus their features, separated by a tab.

## Map

For a map, the key will be the surface form and the value will be the lemmas with their features. Storing ambiguous words in a map might prove to be a problem.