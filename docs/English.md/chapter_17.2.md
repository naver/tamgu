## Methods

1. `clear()`: Clears the container.
2. `convert()`: Detects number values in the string vector `s` (only applicable to `svector` and `uvector`) and converts them into actual numbers. Returns a vector object.
3. `editdistance(v)`: Computes the edit distance with vector `v`.
4. `flatten()`: Flattens a vector structure.
5. `insert(i,v)`: Inserts `v` at position `i`.
6. `join(string sep)`: Produces a string representation for the container.
7. `json()`: Returns a JSON compatible string matching the container.
8. `last()`: Returns the last element.
9. `merge(v)`: Merges `v` into the vector.
10. `pop(i)`: Erases an element from the vector.
11. `product()`: Returns the product of elements.
12. `∏(v,i,j)`: Multiplies the elements from `i` to `j` (optional parameters).
13. `push(v)`: Pushes a value into the vector.
14. `read(string path)`: Reads a file into the vector (only applicable to `vector`, `svector`, `uvector`).
15. `remove(e)`: Removes `e` from the vector.
16. `reserve(int sz)`: Reserves a size of `sz` potential elements in the vector.
17. `reverse()`: Reverses a vector.
18. `size()`: Returns the number of elements, i.e., the size of the container.
19. `shape(d1,d2,d3)`: Determines the dimensions of a tensor on a one-dimensional vector.
20. `shuffle()`: Shuffles the values in the vector.
21. `sort(bool reverse | function | method)`: Sorts the values in the vector.
22. `sortfloat(bool reverse)`: Sorts the values in the vector as floats.
23. `sortint(bool reverse | function)`: Sorts the values in the vector as integers.
24. `sortstring(bool reverse)`: Sorts the values in the vector as strings.
25. `sortustring(bool reverse | function)`: Sorts the values in the vector as Unicode strings.
26. `sum()`: Returns the sum of elements.
27. `∑(v,i,j)`: Sums the elements from `i` to `j` (optional parameters).
28. `unique()`: Removes duplicate elements.
29. `write(string file)`: Writes the vector content to a file.