## Passing Arguments to a Tamgu Program

When a Tamgu program is called with a list of arguments, each of these arguments becomes available to the Tamgu program through three specific system variables: `_args`, `_current`, and `_paths`.

### Example:
```
tamgu myprogram c:\\test\\mytext.txt
```

#### `_args`: Argument Vector
Tamgu provides a specific variable called `_args`, which is actually a string vector that stores each argument according to its position in the declaration list.

##### Example (from the above call):
```
file f;
f.openread(_args[0]);
```

#### `_paths` and `_current`: Path Management
Tamgu provides a second vector variable called `_paths`, which stores the pathnames of the different Tamgu programs that have been loaded.

##### Displaying all paths loaded in memory:
```
string n;
for (n in _paths)
    print("Loaded: ", n, "\n");
```

**Important**: The first element of `_paths` (`_paths[0]`) stores the current directory pathname, while `_paths[1]` stores the path of the current program file.

`_current` is another interesting variable that stores the path of the program file that is currently being run. The path stored in `_current` always ends with a final separator. In fact, `_current` points to the same path as `_paths[1]`.

`_endl`: Carriage Return
Windows and Unix do not use the same carriage return. On Windows, a carriage return is usually two characters long (`\r\n`). `_endl` returns the appropriate carriage return based on the platform value.

`_sep`: Separator in Pathnames
Unix-based systems and Windows use different separators between directory names in pathnames. Unix requires `/`, while Windows requires `\`. Tamgu provides a specific variable called `_sep`, which returns the correct separator based on your system.