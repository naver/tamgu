# Annotator Methods

The "annotator" type provides several methods to investigate the extracted information from the rules. These methods are organized into different sections.

## Compiling

There are two ways to add rules to a program. You can either type the rules directly into the code, which will create a single repository for all the rules. Alternatively, you can store the rules in a string and compile that string using an annotator variable.

1. `compile(string rules)`: Compiles the rules stored in the "rules" string into the annotator variable. Only this variable can access these rules. You can also pass the string to an annotator variable during declaration, which is equivalent to calling the `compile` method.
   Example: `annotator r(ruleCode);`

## Rule Selection

It is possible to have multiple annotators running in parallel, each corresponding to a specific set of labels. For example, you can associate all rules with labels "lab1" and "lab2" to an annotator variable, which will only apply the rules with these labels.

2. `select(uvector labels)`: Selects the rules with labels defined in the "labels" vector.
3. `clear()`: Clears the label selection.
4. `selection()`: Returns the label selection.

## Lexicon

This method associates a general-purpose transducer lexicon to the annotator. A general-purpose lexicon is a lexicon for a specific natural language, such as English, French, or Korean. You can have multiple lexicons at a time, but the first lexicon provided will be used for tokenizing strings.

5. `lexicon(transducer t)`: Sets the initial language dictionary, if available.

## Applying Rules

The following methods are used to apply rules to either a string or a vector of strings.

6. `compilelexicons()`: Pre-compiles the lexicons. If not called before parsing, the first parse will compile them, which may introduce delays.
7. `spans(bool)`: Returns both the annotation classes and their offsets if set to true, or only the annotation classes if set to false.
8. `parse(ustring txt, bool keeplex)`: Applies rules to a ustring.
9. `apply(uvector tokens, bool keeplex)`: Applies rules to a vector of tokens.
10. `apply(vector morphos, bool keeplex)`: Applies rules to a vector of morphologically analyzed tokens. Each element in the vector should contain at least two elements: [word, lemma, feat1, feat2..], where "word" is the surface form, "lemma" is its lemma form, and "feat1, feat2.." is a list of features.
    Example: For the input "the dogs": `[[“the”, “the”,”det” ,”definite”], [“dogs”,”dog”,”noun”,”plural"]]`
11. `apply(annotator, bool keeplex)`: Applies rules to the structure computed by a previous annotator parse or apply. This allows for a single tokenization and multiple passes of rules.

## Parsing

While parsing, it is possible to access the internal structures that have been extracted by the annotator so far. The following methods can be used in a callback function, for instance, to enhance the analysis.

### Label Access

12. `checklabel(ustring label)`: Checks if a label belongs to the annotated text.
13. `labels()`: Returns the list of all extracted labels.

### Token Access

14. `words()`: Returns the list of all words found in the text.
15. `tokens()`: Returns the sequence of tokens extracted from the text.
16. `tokenize(ustring txt)`: Applies lexicons to "txt" and returns its tokenization. Also applies the associated function.

### Single Token Access

17. `checkword(ustring wrd)`: Checks if a word belongs to the annotated text and returns the list of annotations.
18. `word(ustring w)`: Returns true if the word (or the lemma) has been detected in the text.
19. `token()`: Returns the current word or its index, depending on whether the recipient variable is a string or an integer.
20. `token(long idx)`: Returns the token at the position "idx" itself.

## Dependencies

21. `dependencies(bool clear)`: Stores all the annotations as dependencies in the knowledge base (see section 46 for more information on dependencies). Each token is transformed into a synode, and each annotation is transformed into a dependency with the synodes as parameters. The "clear" parameter is optional, and when set to true, the knowledge base is emptied before receiving new entries. If an annotator is shared with another, its synodes are also shared, meaning that new dependencies will share the same synodes.

What is the difference between compiling and direct declaration?

Let's take an example:
a) First, we will declare the following rules directly in the code:

In the above example, "r1" will contain the rules in "rule1", and "r2" will contain the rules in "rule2". In this case, the two annotators will not share the same rule repository; they will have their own.