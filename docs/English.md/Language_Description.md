## Language Description

A Tamgu regular expression is a string that uses meta-characters to provide flexibility in describing a word. The following meta-characters are available:

- `%d` stands for any digit.
- `%x` stands for a hexadecimal digit (abcdef0123456789ABCDEF).
- `%p` stands for any punctuation mark from the following set: `< > { } [ ] ) , ; : . & | ! / \\ = ~ # @ ^ ? + - * $ % ' _ ¬ £ €` “`.
- `%c` stands for any lowercase letter.
- `%C` stands for any uppercase letter.
- `%a` stands for any letter.
- `?` stands for any character.
- `%?` stands for the character "?" itself.
- `%%` stands for the character "%" itself.
- `%s` stands for any space character, including the non-breaking space.
- `%r` stands for a carriage return.
- `%n` stands for a non-breaking space.
- `~` represents negation.
- `\\x` is the escape character.
- `\\ddd` represents a character code with exactly 3 digits.
- `\\xFFFF` represents a character code with exactly 4 hexadecimal digits.
- `{a-z}` represents a range of characters from "a" to "z", inclusive.
- `[a-z]` represents a sequence of characters.
- `^` indicates that the expression should start at the beginning of the string.
- `$` indicates that the expression should match up to the end of the string.

Example:

- `dog%c` matches "dogs" or "dogg".
- `m%d` matches "m0", "m1", ..., "m9".

Operators:

- `*`: the preceding character can be repeated 0 or more times.
- `+`: the preceding character must be present at least once.
- `()`: groups characters together.
- `[]`: represents a set of characters.

A regular expression can use the Kleene-star convention to define characters that occur more than once.

- `x*`: the character "x" can be repeated 0 or more times.
- `x+`: the character "x" must be present at least once.
- `(x)`: the character "x" is optional.
- `[xyz](+*)`: represents a sequence of characters.
- `{xyz}(+*)`: represents a disjunction of characters.

**IMPORTANT:**

The `grammar_macros` is a system function that is executed during code parsing. It is not executed with the rest of the code after compilation.