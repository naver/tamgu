# Tamgu String Types

In Tamgu, there are three types for handling strings: `rawstring`, `string`, and `ustring`.

## String Type

The `string` type is used to handle any kind of string. It provides various methods to extract substrings, characters, or apply pattern recognition on the string.

## Ustring Type

The `ustring` type is used for faster access to very large strings. It assumes that the entire string has a single encoding and is based on the `wstring` implementation in C++. The "u" stands for "Unicode".

## Rawstring Type

The `rawstring` type is different from the other two types. It accepts strings but handles them at the byte level. When creating a `rawstring` element, you must provide either its size or an initial string, which determines the variable's size. The string will not accept characters outside its boundaries unless it is resized. Unlike the other types, a `rawstring` does not require specific protection in threads and can be freely accessed and modified. However, resizing a `rawstring` is not possible if threads are running in the background. Since the string is handled at the byte level, accessing it is very fast as the system does not assess UTF-8 characters.

Example:
```cpp
rawstring rd(100);
rd = "toto";
println(rd[0], rd[1], rd[2], rd[3], rd[4], rd[5], rd[6]);
// Since the string is managed at the byte level, UTF-8 is not recognized: c l i c h Ã ©
```

Note: The improved text has been formatted as a GitHub README.md file.