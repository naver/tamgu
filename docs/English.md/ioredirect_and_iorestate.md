## ioredirect and iorestate

These two functions are used to capture the output from stderr or stdout and save it into a file.

### ioredirect

```cpp
int ioredirect(string filename, bool err);
```

This function redirects either stderr (if `err` is true) or stdout (if `err` is false) to the specified `filename`. It returns an ID, which will be used to set the output back to normal.

### iorestate

```cpp
void iorestate(int id, bool err);
```

This function brings the output back to normal. The first parameter is the ID that was returned by `ioredirect`. The file is then closed.

### Example

```cpp
int id = ioredirect("C:\\Tamgu\\test\\test.txt", true);
printlnerr("This string is now stored in file: test.txt");
iorestate(id, true); // back to normal
```

In the example above, the output of `printlnerr` is redirected to the file "test.txt" and then restored back to normal using `iorestate`.