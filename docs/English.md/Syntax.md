## Syntax

In Tamgu, a rule is defined as a regular expression that combines tokens and semantic categories from the lexicons. A rule returns a label that is associated with the span of the token sequence that was recognized. The body of the rule is written after the "< -" symbol, although the character "←" can also be used.

For example, the rule "goodfood < - #food ,?+,delicious" states that if something is a food and is followed by the phrase "was delicious", then the label "goodfood" is produced for that sequence of words. The symbol "?+" indicates a sequence of at least one token.

### Operators

The following operators are available in Tamgu:

- {t1,t2,t3..}: The token must match one of the elements in the list of tokens.
- ?: Any token.
- #label: A semantic label produced by a rule or the lexicon.
- #{l1,l2..}: If the label is one of l1, l2, etc. (disjunction).
- #[l1,l2..]: The label should match l1, l2, etc. (conjunction).
- *,+ (:c): Kleene operators, which can be bounded with a counter.
- ~: Negation (only of atomic elements).
- (..): Optional sequence of tokens.
- [..]: Sequence of tokens.
- \\>..<: The tokens in this sequence will be skipped in the final annotation.
- \\<call p1 p2 \\>: A call to a function that returns true or false.
- token: A simple token that must match the current word from the text.
- 'rgx': A regular expression based on POSIX regular expression.
- "metas": A Tamgu-specific regular expression based on the following meta-characters (see below).

The following meta-characters can be used in Tamgu regular expressions:

- %p: A punctuation.
- %d: A digit.
- %C: An uppercase character.
- %c: A lowercase character.
- %a: An alphabetical character.
- %e: An emoji character.
- %H: A Hangul character.
- %s: A space character.
- %r: A carriage return.
- %S: Both space and carriage return.
- %x: Stands for a hexadecimal digit (0-9a-fA-F).
- %X: Escapes the character X, where X is any character.
- ~X: Negation.
- \\x: Escape character.
- \\ddd: Character code across 3 digits exactly.
- \\xFFFF: Character code across 4 hexadecimals exactly.