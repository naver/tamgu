# Tamgu Contextual

## Tamgu is a contextual programming language.

In Tamgu, the way a variable is handled depends on its context of utilization. When two variables are used together through an operator, the result of the operation depends on the type of the variable on the left, which introduces the operation. In the case of an assignment, the type of the receiving variable determines the type of the entire group.

### Example

If we declare two variables, one string and one integer, then the "+" operator can act as a concatenation or an arithmetic operation.

a) In this case, "i" is the receiving variable, making the whole instruction an arithmetic operation.

```
int i=10;
string s="12";
i=s+i;       // the "s" is automatically converted into a number.
print("I="+i+"\n");
```

Output:
```
I=22
```

b) In this other case, "s" is the receiving variable. The operation is now a concatenation.

```
int i=10;
string s="12";
s=s+i;       // the "i" is automatically converted into a string.
print("S="+s+"\n");
```

Output:
```
S=1210
```

## Implicit conversion

The notion of context is very important in Tamgu as it defines how each variable should be interpreted. Implicit conversions are processed automatically for certain types. For instance, an integer is automatically transformed into a string, with its own digits as the value. In the case of a string, the content is transformed into a number if the string only contains digits, otherwise it is 0.

For more specific cases, such as a vector or a map, the implicit conversions can be a bit more complex. For example, a vector as an integer will return its size, and as a string, a representation of the vector. A file as a string returns its filename, and as an integer, its size in bytes.