## Methods

There are only two functions that are exposed by this type:

1. `apply(string|vector)`: You can apply a grammar to a text, which will be transformed into a vector of characters, or to a vector of tokens.

2. `load(rule, int skipblanks)`: You can either load rules as a string or as a vector of rules. You can also load rules when building the grammar object itself. `skipblanks` is optional and can have the following values:

   - `0`: All characters should be taken into account in the grammar. This is the default value when `skipblanks` is omitted.
   
   - `1`: White spaces and tabs are automatically skipped before applying a target to a substring. Trailing characters at the end of the strings are also skipped.
   
   - `2`: All spaces, including carriage returns, are skipped.

Note: The "in" operator can also be used with a grammar. It is used as a way to detect if a string is compatible with the grammar.