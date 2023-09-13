## String Manipulations Examples

Below are some examples of string manipulations:

```cpp
string s;
string x;
vector<string> v;

// Some basic string manipulations
s = "12345678a";
x = s[0];            // value = '1'
x = s.substr(2, 2);  // value = '34'
x = s.substr(2);     // value = '345678a'
x = s.substr(0, 5);  // value = '12345'
x = s.substr(2, s.find('a') - 2);  // value = '34567'
s[2] = "ef";         // value = '12ef5678a'

// The last 3 characters
x = s.substr(s.length() - 3);  // value = '78a'

// Splitting along a space
s = "a b c";
v = split(s, ' ');  // v = ['a', 'b', 'c']

// Using regular expressions to find the first match
x = regex_find(s, "\\d\\d\\w");  // value = '78a'

// Splitting the string using a pattern
s = "12a23s45e";
v = regex_split(s, "\\d\\d\\w");  // value = ['12a', '23s', '45e']

// Replacing using regular expressions
x = regex_replace(s, "\\d\\d\\w", "X");  // value = '12aX45e'
x = regex_replace(s, "\\d\\d\\w", "$1");  // value = '12a2345e'

// Regular Regular Expressions: Not available on all platforms
preg rgx("\\w+day");
string str = "Yooo Wesdenesday Saturday";
vector<string> vrgx = rgx.find_all(str);  // ['Wesdenesday', 'Saturday']
```

Please note that the code provided is written in C++. Some functions used in the examples may not be available on all platforms.