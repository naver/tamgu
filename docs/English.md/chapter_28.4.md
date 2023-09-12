## Sub-grammars

Sub-grammars are introduced within brackets. Within these brackets, you can define a disjunction of character regular expression strings. These expressions are especially useful when applying a grammar to a vector of strings, as the strings can be matched at the character level against the expressions. Each expression should be separated from the following expression with a "|".

You cannot call a rule from within brackets, so a string such as "dog" would be equivalent to "$dog".

Example:

```python
string dico = @"
test := %a, wrd, %a.
wrd := [%C, (" - "), %c+ | test | be | dog | cat].
"@;

grammar g(dico);
ustring s = "The C -at drinks";
uvector v = s.tokenize();
vector res = g.apply(v);
println(res);
```

## Vector vs. Map

If we replace the recipient variable with a vector, the output is different. The head rule name is inserted into the final structure as the first element. Therefore, if we apply the above grammar to the same string, but with a vector as the output, we obtain:

```python
['base', ['word', 'Test'], ' ', ['number', '123'], ' ', ['code', 'T234e']]
```