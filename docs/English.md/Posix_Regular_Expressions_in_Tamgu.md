## Posix Regular Expressions in Tamgu

Posix regular expressions are available in Tamgu, just like Tamgu regular expressions. However, there are two minor differences. First, to use posix regular expressions, you need to prefix your expression with "p" instead of "r". Second, the object type associated with posix regular expressions is "preg".

These regular expressions are based on the posix regular expression scheme. They can be used instead of treg everywhere, except for the scan method, in strings and files.

### Example

```cpp
string str = "this subject has a submarine as a subsequence";
svector vs = p'\\b(sub)([^ ]*)' in str;
println(vs); // ['subject','submarine','subsequence']

ivector iv = p'\\b(sub)([^ ]*)' in str;
println(iv); // [5,12,19,28,34,45]

string s = p'\\b(sub)([^ ]*)' in str;
println(s); // subject

s = str[p"\\b(sub)([^ ]*)"];
println("S=" + s); // S=subject

s = str[-p"\\b(sub)([^ ]*)"];
println("S=" + s); // S=subsequence
```