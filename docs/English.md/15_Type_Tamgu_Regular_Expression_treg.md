# Tamgu Regular Expression

Tamgu provides its own regular expression formalism called `treg`. `treg` is both a type and a specific way to write down regular expressions. As a type, it takes a string as an argument, which follows the description below. But it can also be provided directly as a string, in which case the string is of the form: `r"..."`, where the 'r' stands for regular expressions.

## Methods

The `treg` type exposes two methods:

1. `compile(string rgx)`: compiles a string into a `treg` object.
2. `match(string s)`: checks if the string `s` matches the Tamgu regular expression.

Example:

```cpp
treg tt("%C+");
if (tt.match("ABCDE"))
    println("Yes");
if (tt == "aABCDE")
    println("Yes");
else
    println("NO");
```

## r expression

You can also use `r` strings to perform the same type of operation:

```cpp
if (r"%C+" == "ABCDE")
    println("Yes");
```

An `r` expression can be written with `""` or `''` (double quotes or single quotes).

## Language Description

A Tamgu regular expression is a string where meta-characters are used to introduce a certain freedom in the description of a word. These meta-characters are:

- `%d`: stands for any digit
- `%x`: stands for a hexadecimal digit (abcdef0123456789ABCDEF)
- `%p`: stands for any punctuation belonging to the following set: `< > { } [ ] ) , ; : . & | ! / \ = ~ # @ ^ ? + - * $ % ' _ ¬ £ €` “
- `%c`: stands for any lowercase letter
- `%C`: stands for any uppercase letter
- `%a`: stands for any letter
- `?`: stands for any character
- `%?`: stands for the character “?” itself
- `%%`: stands for the character “%” itself
- `%s`: stands for any space character, including the non-breaking space
- `%r`: stands for a carriage return
- `%n`: stands for a non-breaking space
- `~`: negation
- `\x`: escape character
- `\ddd`: character code across 3 digits exactly
- `\xFFFF`: character code across 4 hexadecimals exactly
- `{a-z}`: between a and z (inclusive)
- `[a-z]`: sequence of characters
- `^`: the expression should start at the beginning of the string
- `$`: the expression should match up to the end of the string

Example:

- `dog%c` matches `dogs` or `dogg`
- `m%d` matches `m0`, `m1`, ..., `m9`

### Operators: `*`, `+`, `()`, `([])`

A regular expression can use the Kleene-star convention to define characters that occur more than once.

- `x*`: the character can be repeated 0 or more times
- `x+`: the character must be present at least once
- `(x)`: the character is optional
- `[xyz](+*)`: a sequence of characters
- `{xyz}(+*)`: a disjunction of characters

Here, `x` can be a character or a meta-character. There is one special case with the `*` and the `+`. If the character that is to be repeated can be any character, then one should use `%+` or `%*`.

Important: These two rules are also equivalent to `?*` or `?+`.

Example:

1. `a*ed` matches `aed`, `aaed`, `aaaed`, etc. (the `a` can be present 0 or more times)
2. `a%*ed` matches `aed`, `aued`, `auaed`, `aubased`, etc. (any characters can occur between `a` and `ed`)
3. `a%d*` matches `a`, `a1`, `a23`, `a45`, `a765735`, etc.
4. `a{%d %p}` matches `a1`, `a/`, `a`, etc.
5. `a{bef}` matches `ab`, `ae`, or `af`
6. `a{%dbef}` matches `a1`, `ab`, `ae`, `af`, `a0`, `a9`, etc.
7. `a{be}+` matches `ab`, `ae`, `abb`, `abe`, `abbbe`, `aeeeb`, etc.
8. `a[be]+` matches `abe`, `abebe`, etc.

## Macros: `grammar_macros(key, pattern)`

Some expressions might be a bit complex to write down. Tamgu provides the procedure `grammar_macros`, which creates a new meta-character that can be used in expressions. The first argument is a character that will be used as an index, while the second is a regular expression pattern that will be associated with this key.

This function provides patterns for all calls to grammar regular expressions.

Example:

```cpp
grammar_macros("X", "({%+ -})0x%x+(.%x+)(p({%+ -})%d+)");
string s = "ceci est: 0x1.0d0e44bfeec9p -3 dans la chaine.";
// We use the macro
string res = s.scan("%X");
println(res); // 0x1.0d0e44bfeec9p -3
```

IMPORTANT: `grammar_macros` is a system function, which means that it is executed while parsing the code. It won't execute with the rest of the code once the compiling has been done.

## Using `treg` in strings

It is also possible to use `treg` directly in strings to extract or modify their content.

```cpp
string uu = "That was: 0x1.0d0e44bfeec9p -3, 0x3.456aebp -1 in here.";
print("Tst:<", uu[-r"%d":" "], ">\n"); // Tst:<1>
println("Tst2:", uu["x":r"%d %c"]); // Tst2: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

Note that it is also possible to replace a `treg` expression with a variable of type `treg`:

```cpp
treg subt(r"%d %c");
println("Tst3:", uu["x":subt]); // Tst3: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

## Taskell

You can also use these `treg` expressions with Taskell functions:

```cpp
<mytreg(r"%C+") = "uppercase">
<mytreg([r"%d+":v]) = "ok">
<mytreg(v) = "missed">
println(mytreg("ABC")); // uppercase
println(mytreg(["3a4", 1, 4, 5])); // ok
```

## Prolog

You can also use them in Prolog:

```cpp
tst(r"%d+"). // the `treg` expression is here...
tst("machin").
truc("Ok", ?P) :- tst(?P).
vector vpred;
vpred = truc(?P, "machin");
println(vpred); // [truc("Ok",machin)]
vpred = truc(?P, "12");
println(vpred); // [truc("Ok",12)]
vpred = truc(?P, "nope");
println(vpred); // []
```

## Posix Regular Expressions: `preg`

The posix regular expressions are also available in Tamgu, in the same way as Tamgu Regular expressions. However, there are two minor differences. First, to use these expressions, you have to prefix your expression with `p` instead of `r`. Second, the object type associated is `preg`.

These regular expressions are based on the posix regular expression scheme. They can be used instead of `treg` everywhere, except for the `scan` method, in strings and files.

Example:

```cpp
string str = "this subject has a submarine as a subsequence";
svector vs = p'\b(sub)([^ ]*)' in str;
println(vs); // ['subject', 'submarine', 'subsequence']
ivector iv = p'\b(sub)([^ ]*)' in str;
println(iv); // [5, 12, 19, 28, 34, 45]
string s = p'\b(sub)([^ ]*)' in str;
println(s); // subject
s = str[p"\b(sub)([^ ]*)"];
println("S=" + s); // S=subject
s = str[-p"\b(sub)([^ ]*)"];
println("S=" + s); // S=subsequence
```