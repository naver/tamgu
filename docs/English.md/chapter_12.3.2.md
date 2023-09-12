## Unicode Characters

- The corresponding Unicode character for `\\x30` is the character `0`.
- The Unicode code `\\uhhhh` is also translated into the corresponding Unicode character `\\u0030`, which is the character `0`.
- The coding `&#d(d)(d)(d);` is used in XML and HTML texts to represent the corresponding Unicode character `&#30;`, which is the character `0`.
- In XML and HTML, the `&namecode;` syntax is used to represent characters. For example, `&eaccute;` represents the character `é`.

## Emojis

Tamgu keeps track of emojis (Unicode 2017) and provides the following methods:
- `emojis()` returns a treem apls object containing the emoji Unicode and its textual description in English.
- `isemoji(string)` checks if a string is composed of emojis.
- `emoji(string)` returns the textual description of an emoji.

## Operators

- `sub in s` tests if `sub` is a substring of `s`.
- `for (c in s) {…}` loops through all characters in `s`, with `c` containing a character from `s` at each iteration.
- `+` is used to concatenate two strings.
- `"…”` defines a string where meta-characters such as `\\n`, `\\t`, `\\r`, `\\` are interpreted.
- `'…'` defines a string where meta-characters are not interpreted. This string cannot contain the character `'`.

## Indexes

- `str[i]` returns the `i`th character of a string.
- `str[i:j]` returns the substring between `i` and `j` in `str`.
- `str[s..]` returns the substring starting at `s` in `str`.
- `str[-s..]` returns the substring starting at `s` in `str`, searching from the end of the string.
- When `i` and `j` are positive integers, they are treated as absolute positions within the string. If they are negative, they are considered as offsets counted from each string's extremities. If the first element of the interval is a substring and the second one is a positive integer, the second index will be treated as an offset from the rightmost position of where the substring was found.
- A character range can also be modified.

## Examples

```python
string s = "This is a cliché, which contains a 'é'";
s[10:16] // Returns "cliché"
s["cliché":7] // Returns "cliché, which"
s["cliché":-4] // Returns "cliché, which contains a"
s[-"a":] // Returns "a 'é'"
s[-"a":] = "#" // Replaces the substring with "#": "This is a cliché, which contains #"

# If an index is out of bounds and the flag `erroronkey` is set to false, Tamgu will return an empty string.
```

## Conversion to Integer or Float

If a string contains digits, it can be converted into the equivalent number. Otherwise, its conversion is 0.

## Formatting

A format string contains specific variables that can be replaced with content. For example:

```python
string frm = "this %1 is a %2 of %1 with %3";
string str = frm.format("test", 12, 14);
println(str); // Result: "this test is a 12 of test with 14"
```

## Scanning

There are two versions of the `scan` method:
- The first version takes only one argument and applies the regular expression (TREG) across the whole string, extracting every single target that matches the TREG. Each element corresponds to the whole regular expression.
- The second version takes a separator and considers the regular expression as extracting different fields separated with the separator.

For example, `scan("%d+, %d+", ',')` considers expressions in which there are two integers separated with a comma. It will return two elements for "12,34,45,56" and only two for "12,34".

Note that the second version does not return positions for `ivector` as the other versions do.

## Macro

A macro can be used to read complex hexadecimal structures. For example:

```python
grammar_macros("X", "({%+ -})0x%x+(.%x+)(p({%+ -})%d+)");
string s = "This: 0x1.0d0e44bfeec9p -3 0x2.09p3 0x3.456aebp -1 in here.";
string res = s.scan("%X");
```