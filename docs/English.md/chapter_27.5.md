# Tamgu Regular Expression vs. POSIX Regular Expressions

Tamgu regular expressions provide a simple schema for handling character expressions. A Tamgu regular expression can handle multiple words at a time.

Tamgu exposes a list of meta-characters (see above for a list of them) that can be combined with characters and Kleene operators. A Tamgu regular expression is always encapsulated with double quotes.

Example:
- `"%p+"`: a string composed of only punctuations.
- `"%H+"`: a string composed of only Hangul characters.
- `"test(s)"`: a string with an optional 's'.
- `"Programming Language"`: a multiword expression.

Note that the first example, `"%p+"`, is equivalent to `%p+`. You can omit the quotes in this case. The same applies to `%H+`. However, if you have a combination of characters and multiple meta characters, you must use double quotes.

Posix regular expressions are also available, but they cannot be used for tokenization processes. These expressions are embedded within single quotes.

Example:
- `'(\\d{1,3}):( \\d{1,3}):( \\d{1,3}):( \\d{1,3 })'`: this expression can recognize 1:22:33:444.