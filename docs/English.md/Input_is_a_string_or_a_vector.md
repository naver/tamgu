## Input is a string or a vector

If the input is a string, each detected character is appended to the output string. However, if the input is a vector of characters, the output result is kept as a vector of characters.

### Example

```python
# This grammar recognizes a word or a number
grammar = """
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"""

# Load the grammar
g = Grammar(grammar)

# Split a string into a character vector
s = "Test 123 T234e"
vs = list(s)

# Apply the grammar to the character vector
v = g.apply(vs)

print(v)
```

The output in this case is:

```python
['base', ['word', 'T', 'e', 's', 't'], ' ', ['number', '1', '2', '3'], ' ', ['code', 'T', '2', '3', '4', 'e']]
```