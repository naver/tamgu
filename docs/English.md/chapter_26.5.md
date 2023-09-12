## Regular Expressions

The regular expressions processed by the transducer are very limited:

1. `%c`: defines a character, where `c` is a UTF8 character.
2. `$..`: defines a string.
3. `u-u`: defines an interval between two Unicode characters.
4. `[..]`: defines a sequence of characters.
5. `{...}`: defines a disjunction of strings.
6. `.+`: structure should occur at least once.
7. `(..)`: defines an optional structure.
8. `!n`: inserts a features structure along with its number in the feature vector (n >= 1).

Examples:

```cpp
transducer t;
// This expression recognizes Roman Numbers
t.compilergx("{DMCLXVI}+!1", ["\\t+Rom"]);
// This expression recognizes any kind of numbers including the decimal separator and exponential expressions.
t.compilergx("({-+}){0-9}+!1(%.{0-9}+!2({eE}({-+}){0-9}+!3))", ["+Card", "+Dec", "+Exp+Dec"]);
// To recognize ordinal numbers
t.compilergx("{[1st][2nd][3rd]}!1", ["+Ord"]);
t.compilergx("[3-9]([0-9]+)$th!1", ["+Ord"]);
```