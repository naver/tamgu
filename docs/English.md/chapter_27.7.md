## Annotation Rules

An annotation rule is a label associated with a body. The body can integrate different elements such as optional elements, disjunction of elements, regular expressions, skipped words, etc. It returns a list of labels with their spans in the original text as offsets.

### Output

When a rule applies to a sequence of tokens, or to a token, each token is then associated with the label.

In other words, we can see labels as classes. When a rule applies, the tokens from the initial sequence that are spanned by this rule are distributed along the label or class of this rule.

#### Initial sequence:
The chicken was delicious. The chicken was very soft.

#### Rules:
@food <- chicken.
good <- #food, ?, delicious.

#### aspect can now benefit from the application of the above rule
Note that we restrict the number of tokens between good and soft to a maximum of three...

aspect <- #good, ?+:3, soft.

When the rule "good" applies, the token "chicken" receives the "good" label and can then be identified through this new label in subsequent rules.

### Example

Optional elements, #food matches a rule label or a lexicon label.

food ← (#positive), #food +.

We can skip words: >..<

We can also specify that a word can match different labels.

service ← #personnel, >?*:5<, #[positive,service].

### Function Callbacks

A rule can call a function, which will investigate the current token through more complex kinds of matching. For instance, a function might compare the current token against specific lexicons or against word embeddings.

The function must have at least two parameters. The first one is the current token, and the second one is the current annotator variable. However, it is possible to provide more variables if necessary.

```python
function distance(string currenttoken, annotator a) {
    if (currenttoken.editdistance('is') <= 2)
        return true;
    return false;
}
```

label1 <- this, <distance>, a, thing.

This rule will yield label1 for the following utterances:
- this is a thing
- this ist a thing
- this sis a thing

We can also implement a more general version of this function:

```python
function distance(string currenttoken, annotator a, string s) {
    if (currenttoken.editdistance(s) <= 2)
        return true;
    return false;
}
```

We call the function with one more parameter...

label1 <- this, <distance "is">, a, thing.

This function can implement any kind of behavior. For instance, the token could be compared against word embeddings or against a cosine distance between two words.

### Global Rules

Global rules that start with a "#" or a "~" are similar to annotation rules. However, they are not applied along the text but once the text has been fully processed to handle annotations in order to add new ones or remove existing ones.

```python
function test(annotator a) {
    if (a.word("bread"))
        return true;
    return false;
}

#label ← #lab1, #lab2, <test>.
```

For instance, the above rule will create the annotation label if lab1 and lab2 have been created and the call returns true.

Note that the call in this case has only one parameter since the rule applies independently from the text.

### Code Example

The lexicon...

Simply words associated with a "lexicon label"

@positive ← great.
@positive ← better.
@positive ← good.
@positive ← accommodating.

We compare the words with their lemma.

@negative ← $overrate.
@negative ← $disappoint.

Another category.

@food ← sushi.

This is a regular expression, the "s" is optional.
".." is a TAMGU regular expression.
'..' is a POSIX regular expression.

@food ← "food(s)".

These are multiword expressions, which will be combined with the lexicon for a multiword tokenization.

@food ← "lemon chicken".
@food ← "honey walnut prawn(s)".

@service ← accommodating.

@personnel ← staff.
@personnel ← personnel.
@personnel ← $waiter.