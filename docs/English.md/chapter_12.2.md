# Methods

The following methods are available in this library. Please refer to the dedicated chapter on Tamgu regular expression formalism or TREG for more information.

1. `base(int b, bool toconvert)`: Returns the numerical value corresponding to the string's numeric content in base b. The `toconvert` parameter is optional. When set to false, the number to be converted is assumed to be in base 10 and will be converted to base b.

2. `base(vector chrs)`: Sets the encoding for each digit in a given base. The default set is 64 characters: 0-9, A-Z, a-z, #, @. The maximum representation is base 64. You can replace this default character set with your own. If you supply an empty vector, the system will reset to the default set of characters.

3. `bytes()`: Returns a vector of bytes matching the string.

4. `charposition(int pos)`: Converts a byte position into a character position, especially useful in UTF8 strings.

5. `deaccentuate()`: Removes the accents from accented characters.

6. `doublemetaphone()`: Returns a vector containing the double-metaphone encoding of the string.

7. `dos()`: Converts a string to DOS encoding.

8. `dostoutf8()`: Converts a DOS string to UTF8 encoding.

9. `emoji()`: Returns the textual description (in English) of an emoji.

10. `evaluate()`: Evaluates the meta-characters within a string and returns the evaluated string.

11. `extract(int pos, string from, string up1, string up2, ..., up1..upn)`: Returns a vector containing all substrings from the current string, starting at position `pos`, which are composed of `from` up to one of the next strings `up1`, `up2`, ..., `up1..upn`.

12. `fill(int nb, string char)`: Creates a string of `nb` characters.

13. `find(string sub, int pos)`: Returns the position of substring `sub` starting at position `pos`.

14. `format(p1, p2, p3)`: Creates a new string from the current string in which each `%x` is associated with one of the parameters, where `x` is the position of that parameter in the argument list. `x` starts at 1.

15. `geterr()`: Catches the current error output. `Printerr` and `printlnerr` will be stored in this string variable.

16. `getstd()`: Catches the current standard output. `Print` and `println` will be stored in this string variable.

17. `html()`: Returns the string as an HTML compatible string or as a vector of strings.

18. `insert(i, s)`: Inserts the string `s` at position `i`. If `i` is -1, then `s` is inserted between each character in the input string.

19. `isalpha()`: Tests if a string contains only alphabetical characters.

20. `isconsonant()`: Tests if a string contains only consonants.

21. `isdigit()`: Tests if a string contains only digits.

22. `isemoji()`: Tests if a string contains only emojis.

23. `islower()`: Tests if a string contains only lowercase characters.

24. `ispunctuation()`: Tests if a string is composed of punctuation signs.

25. `isupper()`: Tests if a string contains only uppercase characters.

26. `isutf8()`: Tests if a string contains UTF8 characters.

27. `isvowel()`: Tests if a string contains only vowels.

28. `last()`: Returns the last character of the string.

29. `latin()`: Converts an UTF8 string to LATIN.

30. `left(int nb)`: Returns the first `nb` characters of a string.

31. `levenshtein(string s)`: Returns the edit distance with string `s` according to the Levenshtein algorithm.

32. `parenthetic()`: Converts a parenthetic expression into a vector.

33. `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: Converts a recursive expression using opening and closing characters as separators. The parameters `comma` to `rules` are all optional. Please refer to the "tokenization rules" below to understand how to handle these rules.

34. `lower()`: Returns the string in lowercase characters.

35. `mid(int pos, int nb)`: Returns the `nb` characters starting at position `pos` of a string.

36. `ngrams(int n)`: Returns a vector of all ngrams of rank `n`.

37. `ord()`: Returns the code of a string character. Send either the code of the first character or a list of codes, depending on the type of the receiving variable.

38. `parse()`: Parses a string as a piece of code and returns the evaluation in a vector.

39. `pop()`: Removes the last character of the string.

40. `pop(i)`: Removes the character at position `i`.

41. `read(string file)`: Reads a file into the string.

42. `removefirst(int nb)`: Removes the first `nb` characters of a string.

43. `removelast(int nb)`: Removes the last `nb` characters of a string.

44. `replace(sub, str)`: Replaces the substrings matching `sub` with `str`. `sub` can be a TREG.

45. `reverse()`: Reverses the string.

46. `rfind(string sub, int pos)`: Returns the position of substring `sub` backward starting at position `pos`.

47. `right(int nb)`: Returns the last `nb` characters of a string.

48. `scan(rgx, string sep, bool immediatematch, string remaining)`: Returns all substrings matching `rgx` according to the TREG formalism. The `rgx` parameter is a TREG regular expression. Depending on the recipient variable, it might return a position (int), a substring (string), a vector of positions (ivector), or a vector of substrings (svector). The `sep` parameter is optional. If `rgx` contains different sub-fields separated by `sep`, the return value will always be a svector. The `immediatematch` parameter, when set to true, means that `rgx` should match starting at the first character. When set to false (default value), it scans the whole string to find the first match. This parameter must be used with `sep`. The `remaining` parameter must be a string variable. When provided, it returns the rest of the string after the section that matches. This parameter must be combined with `immediatematch` and `sep`.

49. `size()`: Returns the length of a string.

50. `slice(int n)`: Slices a string into substrings of size `n`.

51. `split(string splitter)`: Splits a string along `splitter` and stores the results in a svector (string vector). If `splitter` is an empty string, then the string is split into a vector of characters. If `splitter` is not provided, then the string is split along space characters. `splitter` can be a TREG. If `splitter` is a preg, then the split is done according to the whole POSIX regular expression, not as a substring to detect.

52. `splite(string splitter)`: Splits a string the same way as `split` above, but keeps the empty strings in the final result. Thus, if `s='+T1++T2++T3'`, then `s.split("+")` will return `["T1", "T2", "T3"]`, while `s.splite("+")` will return `["", "T1", "", "T2", "", "T3"]`. If `splitter` is a preg, then the split is done according to the whole POSIX regular expression, not as a substring to detect.

53. `multisplit(string sp1, string sp2, ...)`: Splits a string along multiple splitter strings.

54. `stokenize(map keeps)`: Tokenizes a string into words and punctuations. `keeps` is used to keep together specific strings.

55. `tags(string o, string c, bool comma, bool separator, bool keepwithdigit, svector rules)`: Parses a string as a parenthetic expression, where the opening and closing strings are provided. The parameters `comma` to `rules` are all optional. Please refer to the "tokenization rules" below to understand how to handle these rules.

56. `tokenize(bool comma, bool separator, svector rules)`: Tokenizes a string into words and punctuations. If `comma` is true, then the decimal separator is ","; otherwise, it is ".". If `separator` is true, then "." or "," can be used as separators, as in "3,000.10". `tokenize` returns a svector. Each of these parameters is optional. When one of these parameters is omitted, its default value is false. Please refer to the "tokenization rules" below for how to use this parameter.

57. `trim()`: Removes the trailing characters.

58. `trimleft()`: Removes the trailing characters on the left.

59. `trimright()`: Removes the trailing characters on the right.

60. `upper()`: Returns the string in uppercase characters.

61. `utf8()`: Converts a LATIN string into UTF8.

62. `utf8(int part)`: Converts a Latin string, encoded into ISO 8859 part `part`, into UTF8. For instance, `s.utf8(5)` means that the string to be converted to UTF-8 is encoded in ISO 8859 part 5 (Cyrillic). Please refer to the description of each part below.

63. `write(string file)`: Writes the string content into a file.

## Tokenization Rules

The following rules apply for tokenization:

- `comma`: If true, the decimal separator is ","; otherwise, it is ".".

- `separator`: If true, "." or "," can be used as separators, as in "3,000.10".

- `rules`: A svector that contains additional rules for tokenization.

Please note that all these parameters are optional, and their default value is false.