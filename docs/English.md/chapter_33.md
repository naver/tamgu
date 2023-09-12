35. Type "file" and "wfile" in Tamgu

This type is used to manage input and output files. The type "wfile" is specifically used for handling UTF16 (UCS-2) files.

35.1 Methods

1. eof(): Returns true when the end of the file is reached.
2. file f(string filename, string mode_read): Opens a file according to the specified mode. If the file is in read mode, the "mode_read" parameter is optional. The possible values for "mode_read" are:
   a. "a": append
   b. "r": read
   c. "w": write
   d. "w+": append
3. find(string s, bool case_insensitive): Returns all positions in the file where the string "s" is found.
4. get(): Reads one character from the file.
5. getsignature(): Returns whether the file contains a signature.
6. openappend(string filename): Opens a file in append mode.
7. openread(string filename): Opens a file in read mode.
8. openwrite(string filename): Opens a file in write mode.
9. read(): Reads the whole file into a variable, which can be:
   a. string: the whole document is stored in one string.
   b. svector: the document is split into strings along carriage returns, with each string stored in the container.
   c. bvector: the document is stored byte by byte into the container.
   d. ivector: the document is stored byte by byte into the container.

Operator
x in file: If x is a string, it receives a line from the file. If x is a vector, it pushes the line to the top of the vector. If x is an integer or a float, it gets only one character from the stream.

Example
file f;
f.openread(path);
string s;
svector words;
string w;
for (s in f) { // Using the "in" operator
    s = s.trim();
    words = s.split(" ");
    for (w in words)
        print("word:", w, endl);
}
f.close();

35.2 Standard input: stdin
Tamgu provides the variable "stdin" to handle standard input. This variable can be useful for handling data coming from a piped file, for instance.

Example
string s;
int i = 1;
for (s in stdin) {
    println(i, s);
    i++;
}

If you store these lines in a small file called "stdin.tamgu", then the content of the piped strings will be displayed with a specific number for each line:
echo "The lady is happy" | Tamgu stdin.tamgu.