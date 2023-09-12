## Rules

Rules can be implemented either as a single text (which is the easiest way) or as a vector of strings, where each string represents a rule.

### Rule Format

The format of a rule is as follows:

```
head := (~) element [,;] element .
```

Where `element` can be:
- `word := %a+`
- `number := %d+;$billion;$millions;$thousand`

### Example

If we apply this grammar to the sentence "Test millions of cows", we obtain the following structure:

```
{
  'base': [
    {'word': ['Test']},
    ' ',
    {'number': ['millions']},
    ' ',
    {'word': ['of']},
    ' ',
    {'word': ['cows']}
  ]
}
```

### Complex Structure Recognition

If we want to recognize more complex structures, such as a code that starts with an uppercase letter and is followed by digits, we can implement the following grammar:

```
string  r=@" 
base := _bloc+.  
_bloc  := code;word;number;%s.  
word := %a+.  
number := %d+.  
code := %C,%d+,%c.  
"@;
```

If we apply this grammar to the sentence "Test 123 T234e", we get the following structure:

```
{
  'base': [
    {'word': ['Test']},
    ' ',
    {'number': ['123']},
    ' ',
    {'code': ['T234e']}
  ]
}
```