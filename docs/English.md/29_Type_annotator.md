# Annotator

## Description

An `annotator` is a rule-based text annotation tool. It allows you to define rules to label specific patterns in a text. Annotator rules are divided into four groups: lexicon rules, annotation rules, global rules, and delete rules.

## Rule Syntax

A rule in the `annotator` tool is a regular expression that combines tokens and semantic categories. It returns a label that is associated with the span of the matched tokens in the original text.

The body of a rule is written after the `<-` or `←` symbol. The available operators in a rule are:

- `{t1,t2,t3...}`: Matches any of the listed tokens.
- `?`: Matches any token.
- `#label`: Matches a semantic label produced by a rule or a lexicon label.
- `#{l1,l2...}`: Matches if the label is one of the listed labels (disjunction).
- `#[l1,l2...]`: Matches if the label matches all the listed labels (conjunction).
- `*`, `+ (:c)`: Kleene operators that can be bounded with a counter.
- `~`: Negation (only of atomic elements).
- `(...)`: Optional sequence of tokens.
- `[...]`: Sequence of tokens.
- `>..<`: Skips the tokens in this sequence in the final annotation.
- `<call p1 p2>`: Calls a function that returns true or false.
- `token`: Matches a single token.
- `'rgx'`: Matches a regular expression based on POSIX regular expression syntax.
- `"metas"`: Matches a Tamgu-specific regular expression based on meta-characters.

## Annotator Methods

The `annotator` type exposes several methods to investigate the annotations produced by the rules:

- `compile(string rules)`: Compiles the rules stored in the `rules` string into the `annotator` variable. Only this variable can access the rules.
- `select(uvector labels)`: Selects the rules whose labels are defined in `labels`.
- `clear()`: Clears the label selection.
- `selection()`: Returns the label selection.
- `lexicon(transducer t)`: Associates a general-purpose transducer lexicon to the `annotator`.
- `compilelexicons()`: Pre-compiles the lexicons. If not called before parsing, the first parse will compile them, which may introduce delays.
- `spans(bool)`: Returns both the annotation classes and their offsets if `true`, or only the annotation classes if `false`.
- `parse(ustring txt, bool keeplex)`: Applies the rules to a `ustring`.
- `apply(uvector tokens, bool keeplex)`: Applies the rules to a vector of tokens.
- `apply(vector morphos, bool keeplex)`: Applies the rules to a vector of morphologically analyzed tokens.
- `apply(annotator a, bool keeplex)`: Applies the rules to the structure computed by a previous `annotator` parse or apply.

## Callback Function

You can associate a callback function to an `annotator` object. This callback function is called after the parsing has taken place but before applying the rules to the texts. The callback function has the following signature:

```cpp
function catching(vector v, annotator a) {
    v = pos.tagger(v);
    return v;
}
```

The `catching` function can be used to add more analysis to the annotations. The vector `v` is a list of sub-vectors, where each sub-vector contains the word, its lemma, features, left offset, and right offset. The function should return a vector with the same structure.

## Lexicon Rules

A lexicon rule is used to describe domain vocabulary. It associates words with specific labels. Lexicon rules can be simple words or multiword expressions. Multiword expressions can only be recognized as one single token if they are written using Tamgu regular expressions.

## Annotation Rules

An annotation rule is a label associated with a body. The body can include optional elements, disjunction of elements, regular expressions, skipped words, etc. It returns a list of labels with their spans in the original text as offsets.

## Global Rules

Global rules start with a `#` or `~` and are similar to annotation rules. However, they are not applied along the text but after the text has been fully processed to handle annotations. Global rules can add new annotations or remove existing ones.

## Example

```cpp
// Lexicon
@positive  ← great.
@positive  ← better.
@food  ← sushi.

// Annotation rules
food ← (#positive), #food +.

// Create annotator
annotator r;

// Compile rules
r.compile("@positive  ← great.\n@positive  ← better.\n@food  ← sushi.\nfood ← (#positive), #food +.");

// Apply rules to text
ustring txt = "The sushi was great.";
r.parse(txt, true);
```

In the example above, the `annotator` is created, the rules are compiled, and then applied to the text "The sushi was great.". The resulting annotations will be stored in the `annotator` object.