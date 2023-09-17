# String Manipulation in Tamgu

Tamgu provides several types to handle strings: `string`, `ustring`, and `rawstring`. 

- The `string` type is used to handle any sort of string. It provides many different methods to extract substrings, characters, or apply pattern recognition on top of it.
- The `ustring` type is used to offer faster access to very large strings, as the system assumes only one single encoding for the whole string. It is based on the `wstring` implementation in C++.
- The `rawstring` type handles strings at the byte level. It accepts strings but handles them at the byte level. It does not require specific protection in threads and can be accessed and modified freely. However, you cannot resize a `rawstring` if threads are running in the background.

## Methods

### Base Conversion

- `base(int b, bool toconvert)`: Returns the numerical value corresponding to the string numeric content in base `b`. The `toconvert` parameter is optional. When it is `false`, the number to be converted is in base 10 and is converted to base `b`.
- `base(vector chrs)`: Sets the encoding for each digit in a given base. The default set is 64 characters: 0-9, A-Z, a-z, #, @. You can replace this default character set with your own. If you supply an empty vector, then the system resets to the default set of characters.

### String Extraction and Manipulation

- `bytes()`: Returns an ivector of bytes matching the string.
- `charposition(int pos)`: Converts a byte position into a character position (especially useful in UTF8 strings).
- `deaccentuate()`: Removes the accents from accented characters.
- `doublemetaphone()`: Returns an svector containing the double-metaphone encoding of the string.
- `dos()`: Converts a string to DOS encoding.
- `dostoutf8()`: Converts a DOS string into UTF8 encoding.
- `emoji()`: Returns the textual description (in English) of an emoji.
- `evaluate()`: Evaluates the meta-characters within a string and returns the evaluated string.
- `extract(int pos, string from, string up1, string up2...)`: Returns an svector containing all substrings from the current string, starting at position `pos`, which are composed of `from` up to one of the next strings `up1`, `up2`, etc.
- `fill(int nb, string char)`: Creates a string of `nb` chars.
- `find(string sub, int pos)`: Returns the position of substring `sub` starting at position `pos`.
- `format(p1, p2, p3)`: Creates a new string from the current string in which each '%x' is associated with one of the parameters, 'x' being the position of that parameter in the argument list. 'x' starts at 1.
- `geterr()`: Catches the current error output.
- `getstd()`: Catches the current standard output.
- `html()`: Returns the string into an HTML compatible string or as a vector of strings.
- `insert(i, s)`: Inserts the string `s` at position `i`. If `i` is -1, then insert `s` between each character in the input string.
- `isalpha()`: Tests if a string only contains alphabetical characters.
- `isconsonant()`: Tests if a string only contains consonants.
- `isdigit()`: Tests if a string only contains digits.
- `isemoji()`: Tests if a string only contains emojis.
- `islower()`: Tests if a string only contains lowercase characters.
- `ispunctuation()`: Tests if the string is composed of punctuation signs.
- `isupper()`: Tests if a string only contains uppercase characters.
- `isutf8()`: Tests if a string contains UTF8 characters.
- `isvowel()`: Tests if a string only contains vowels.
- `last()`: Returns the last character.
- `latin()`: Converts an UTF8 string to LATIN.
- `left(int nb)`: Returns the first `nb` characters of a string.
- `levenshtein(string s)`: Returns the edit distance with `s` according to the Levenshtein algorithm.
- `parenthetic()`: Converts a parenthetic expression into a vector.
- `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: Converts a recursive expression using opening and closing characters as separators.
- `lower()`: Returns the string in lowercase characters.
- `mid(int pos, int nb)`: Returns the `nb` characters starting at position `pos` of a string.
- `ngrams(int n)`: Returns a vector of all ngrams of rank `n`.
- `ord()`: Returns the code of a string character.
- `parse()`: Parses a string as a piece of code and returns the evaluation in a vector.
- `pop()`: Removes the last character.
- `pop(i)`: Removes the character at position `i`.
- `read(string file)`: Reads a file into the string.
- `removefirst(int nb)`: Removes the first `nb` characters of a string.
- `removelast(int nb)`: Removes the last `nb` characters of a string.
- `replace(sub, str)`: Replaces the substrings matching `sub` with `str`.
- `reverse()`: Reverses the string.
- `rfind(string sub, int pos)`: Returns the position of substring `sub` backward starting at position `pos`.
- `right(int nb)`: Returns the last `nb` characters of a string.
- `scan(rgx, string sep, bool immediatematch, string remaining)`: Returns all substrings matching `rgx` according to the Tamgu regular expression formalism.
- `size()`: Returns the length of a string.
- `slice(int n)`: Slices a string into substrings of size `n`.
- `split(string splitter)`: Splits a string along `splitter` and stores the results in a svector.
- `splite(string splitter)`: Splits a string the same way as `split` above, but keeps the empty strings in the final result.
- `multisplit(string sp1, string sp2...)`: Splits a string along multiple splitter strings.
- `stokenize(map keeps)`: Tokenizes a string into words and punctuations.
- `tags(string o, string c, bool comma, bool separator, bool keepwithdigit, svector rules)`: Parses a string as a parenthetic expression, where the opening and closing strings are provided.
- `tokenize(bool comma, bool separator, svector rules)`: Tokenizes a string into words and punctuations.
- `trim()`: Removes the trailing characters.
- `trimleft()`: Removes the trailing characters on the left.
- `trimright()`: Removes the trailing characters on the right.
- `upper()`: Returns the string in uppercase characters.
- `utf8()`: Converts a LATIN string into UTF8.
- `utf8(int part)`: Converts a Latin string encoded into ISO 8859 part `part` into UTF8.
- `write(string file)`: Writes the string content into a file.

## String Handling

Tamgu also provides specific methods for Korean strings and Latin tables. These methods are used with the `string` and `ustring` types.

### Korean Specific Methods

- `ishangul()`: Returns `true` if it is a Hangul character.
- `isjamo()`: Returns `true` if it is a Hangul jamo.
- `jamo(bool combine)`: If `combine` is `false` or absent, splits a Korean jamo into its main consonant and vowel components. If `combine` is `true`, combines content into a jamo.
- `normalizehangul()`: Normalizes different UTF8 encoding of Hangul characters.
- `romanization()`: Romanization of Hangul characters.

### Latin Table

Tamgu provides encoding names and constants to access these encodings. The encoding names are:

- `e_latin_we`: Western European
- `e_latin_ce`: Central European
- `e_latin_se`: South European
- `e_latin_ne`: North European
- `e_cyrillic`: Cyrillic
- `e_arabic`: Arabic
- `e_greek`: Greek
- `e_hebrew`: Hebrew
- `e_turkish`: Turkish
- `e_nordic`: Nordic
- `e_thai`: Thai
- `e_baltic`: Baltic Rim
- `e_celtic`: Celtic
- `e_latin_ffe`: Extended (French, Finnish, Estonian)
- `e_latin_see`: South East European
- `e_windows`: Windows encoding

## Operators

Tamgu supports several operators for string manipulation:

- `sub in s`: Tests if `sub` is a substring of `s`.
- `for (c in s) {…}`: Loops among all characters in `s`.
- `+`: Concatenates two strings.
- `"…"`: Defines a string, where meta-characters such as `\n`, `\t`, `\r`, `\"` are interpreted.
- `'…'`: Defines a string, where meta-characters are not interpreted.

## Indexes

Tamgu allows indexing and modification of strings using indexes:

- `str[i]`: Returns the `i`th character of a string.
- `str[i:j]`: Returns the substring between `i` and `j`.
- `str[s:]`: Returns the substring starting at string `s`.
- `str[-s:]`: Returns the substring starting at string `s` from the end of the string.

If an index is out of bounds, an exception is raised unless the flag `erroronkey` has been set to `false`.

## Examples

Here are some examples of string manipulations in Tamgu:

```tamgu
string s;
string x;
vector v;

// Some basic string manipulations
s = "12345678a";
x = s[0];  // value=1
x = s[2:3];   // value=3
x = s[2:-2];  // value=34567
x = s[3:];  // value=45678a
x = s[:"56"];  // value=123456
x = s["2":"a"];  // value=2345678a
s[2] = "ef";  // value=empty

// The 3 last characters
x = s.right(3);  // value=78a

// A split along a space
s = "a b c";
v = s.split(" ");  // v=["a", "b", "c"]

// Regex, x is a string, we look for the first match of the regular expression
x = s.scan("%d%d%c");  // value=78a

// We have a pattern, we split our string along that pattern
s = "12a23s45e";
v = s.scan("%d%d%c");  // value=['12a', '23s', '45e']

x = s.replace(r"%d%ds", "X"); // value=12aX45e

// Replace also accepts %x variables as in Tamgu regular expressions
x = s.replace(r"%d%1s", "%1"); // value=12a2345e

// Regular expressions: Not available on all platforms
preg rgx(p"\w+day");
string str = "Yooo Wesdenesday Saturday";
vector vrgx = rgx in str; // ['Wesdenesday', 'Saturday']
string s = rgx in str; // Wesdenesday
int i = rgx in str; // position is 5

// We use (…) to isolate specific tokens that will be stored in the vector
rgx = p"(\d{1,3}):( \d{1,3}):( \d{1,3}):( \d{1,3})";
str = '1:22:33:444';
vrgx = str.split(rgx);  // [1, 22, 33, 444], rgx is a split expression

str = '1:22:33:4444';
vrgx = str.split(rgx);  // [] (4444 contains 4 digits)

str = "A_bcde";
// Full match required
if (p'[a-zA-Z]_.+' == str)
    println("Yooo"); // Yooo

// This is also equivalent to:
rgx = p'[a-zA-Z]_.+';
if (rgx.match(str))
    println("Yooo bis");

str = "ab(Tamgu 12,Tamgu 14,Tamgu 15,Tamgu 16)";
v = str.extract(0, "Tamgu ", ",", ")");
// Result: ['12', '14', '15', '16']

string frm = "this %1 is a %2 of %1 with %3";
str = frm.format("tst", 12, 14);
println(str); // Result: this tst is a 12 of tst with 14
```
