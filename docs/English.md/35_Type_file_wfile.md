# File Management

The `file` type is used to manage input and output files. The `wfile` type is specifically used to handle UTF16 (UCS-2) files.

## Methods

1. `eof()`: Returns true when the end of the file is reached.
2. `file f(string filename, string mode_read)`: Opens a file according to the specified `mode_read`. If the file is in read mode, `mode_read` is optional. Possible values for `mode_read` are:
   - "a": append
   - "r": read
   - "w": write
   - "w+": append
3. `find(string s, bool case_insensitive)`: Returns all positions in the file where the string `s` is found.
4. `get()`: Reads one character from the file.
5. `get_signature()`: Returns whether the file contains a signature.
6. `open_append(string filename)`: Opens a file in append mode.
7. `open_read(string filename)`: Opens a file in read mode.
8. `open_write(string filename)`: Opens a file in write mode.
9. `read()`: Reads the whole file into a variable, which can be:
   - string: the whole document is stored in one string
   - svector: the document is split into strings along carriage returns, with each string stored in the container
   - bvector: the document is stored byte by byte in the container
   - ivector: the document is stored byte by byte in the container
10. `read(int nb)`: Similar to `read()`, but extracts only "nb" characters or bytes from the file.
11. `readln()`: Reads a line from the file.
12. `seek(int p)`: Positions the file cursor at position `p`.
13. `set_signature(bool s)`: Sets the UTF8 or UTF16 signature accordingly.
14. `tell()`: Returns the position of the file cursor.
15. `unget()`: Returns one character to the stream.
16. `unget(nb)`: Returns `nb` characters to the stream.
17. `scan(string grammar, string separator)`: Reads a file with a Tangu Regular Expression and returns a vector of substrings. The separator is a character that separates one field from another (default is a space).
18. `write(string s1, string s2, ...)`: Writes string `s` to the file.
19. `writelen(string s1, string s2, ...)`: Writes strings to the file, separating each string with a space and adding a carriage return at the end of the line.
20. `writebin(int s1, int s2, ...)`: Writes bytes to the file. If the value is a container, it writes the list of bytes from that container.
21. writejson(auto v)`: Stores `v` as a JSON structure in the file.

## Signature

UTF-8 and UTF-16 files may have a signature at the beginning. A UTF-8 file has three octets for the signature, while a UTF-16 file has two octets. 

- When using the `file` type, the signature must be set beforehand in order to read it. This type can only be used to read UTF-8 or binary files.
- In the case of `wfile`, the signature is automatically set when found at the beginning of the file. This type can only be used to read UTF-16 (UCS-2) files.

## Operator

- `x in file`: If `x` is a string, it receives a line from the file. If `x` is a vector, it pushes the line to the top of the vector. If `x` is an integer or a float, it retrieves only one character from the stream.

## Example

```cpp
file f;
f.open_read(path);
string s;
svector words;
string w;

for (s in f) { // Using the in operator
    s = s.trim();
    words = s.split(" ");
    for (w in words)
        print("word:", w, endl);
}

f.close();
```

## Standard Input: stdin

Tamgu provides the variable `stdin` to handle standard input. This variable is useful for handling data coming from a piped file, for example.

### Example

```cpp
string s;
int i = 1;

for (s in stdin) {
    println(i, s);
    i++;
}
```

If you store these lines in a small file, say `stdin.Tamgu`, then the content of the piped strings will be displayed with a specific number for each line:

```bash
echo "The lady is happy" | Tamgu stdin.Tamgu
```

### Example (scan)

The text contains lines such as:
```
456 -0x1.16bca4f9165dep -3 0x1.0d0e44bfeec9p -3
```

```cpp
file f(_current + "tst.txt");
// Define a macro to read a complex hexadecimal string
grammar_macros("X", "({%+ -})0x%x+(%.%x+)(p({%+ -})%d+)");
// Read the first two fields. The space is the default separator.
uvector v = f.scan("%d+ %X ");
println(v); // ['456', ' -0x1.16bca4f9165dep -3']
// Read the next field.
v = f.scan("%X ");
println(v); // ['0x1.0d0e44bfeec9p -3']
f.close();
```
