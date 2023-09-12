## Lexicon Rules

Lexicon rules are used to describe domain-specific vocabulary. Each lexical rule consists of only one element. However, you can declare multiword expressions using Tamgu regular expressions.

**Important**: Only Tamgu regular expressions can be used for multiword expressions. If you want your expression to be recognized as a single token during the tokenization process, Tamgu regular expressions must be used.

### Examples

```tamgu
// Simple words associated with a "lexicon label"
@positive ← great.
@positive ← better.

// Another category
@food ← sushi.

// This is a regular expression, the "s" is optional
// ".." is a TAMGU regular expression
@food ← "food(s)".
```