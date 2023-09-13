# Tamgu Contextual

## 12.1 Introduction
Tamgu is a contextual programming language. The way variables are handled depends on their context of utilization. When two variables are used together through an operator, the result of the operation depends on the type of the variable on the left, the one that introduces the operation. In the case of an assignment, the type of the receiving variable determines the type of the whole group.

### Example
If we declare two variables, one string and one integer, then the "+" operator will act as either a concatenation or an arithmetic operation.

a) In this case, the receiving variable is `i`, making the whole instruction an arithmetic operation.
```
int i = 10;
string s = "12";
i = s + i;       // the variable `s` is automatically converted into a number.
print("I = " + i + "\n");
```
Output:
```
I = 22
```

b) In this other case, the receiving variable is `s`. The operation is now a concatenation.
```
int i = 10;
string s = "12";
s = s + i;       // the variable `i` is automatically converted into a string.
print("S = " + s + "\n");
```
Output:
```
S = 1210
```

## 12.2 Implicit Conversion
The notion of context is very important as it defines how each variable should be interpreted. Implicit conversions are processed automatically for a certain number of types. For instance, an integer is automatically transformed into a string, with its own digits as the value. In the case of a string, the content is transformed into a number if the string only contains digits; otherwise, it is considered as 0.

For more specific cases, such as a vector or a map, the implicit conversions are sometimes a bit more complex. For instance, a vector as an integer will return its size, and as a string, it will return a representation of the vector. A file as a string returns its filename, and as an integer, it returns its size in bytes.