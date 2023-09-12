# System Functionalities in Tamgu

Tamgu provides various system functionalities such as reading the content of a directory into a vector or executing a system command. It exposes the variable `_sys`, which should be used to access the following methods:

## Methods

1. `command(string s, string outputfile)`: Executes the system command `s`. `outputfile` is optional and is used to redirect the command output (stdout). If `outputfile` is supplied, `command` also returns the content of this file as a string.

2. `createdirectory(string path)`: Creates a directory for the given path. Returns false if the directory already exists or cannot be created.

3. `env(string var)`: Returns the value of the environment variable `var`.

4. `env(string var, string value)`: Sets the value of the environment variable `var`.

5. `listdirectory(string path)`: Returns the files in a directory as a vector.

6. `ls(string path)`: Returns the files in a directory as a vector.

7. `mkdir(string path)`: Creates a directory for the given path. Returns false if the directory already exists or cannot be created.

8. `fileinfo(string path)`: Returns a map with the following information for a given file:
   - `info["size"]`: Size of the file
   - `info["date"]`: Date of the file
   - `info["change"]`: Date of the last change
   - `info["access"]`: Date of the last access
   - `info["directory"]`: True if the path is a directory
   - `info["pathname"]`: The real pathname

9. `pipe(string cmd)`: Executes the command `cmd` and returns a vector as a result containing the output of that command.

10. `realpath(string path)`: Returns the actual path for a given relative path.

11. `getchar()`: Returns the last character keyed in. `getchar` does not echo the characters on the screen.

12. `colors(int style, int code1, int code2, bool disp)`: Sets the text color and returns the color string.

13. `foregroundcolor(int color)`: Sets the foreground text color.

14. `backgroundcolor(int color)`: Sets the background text color.

15. `rgbforegroundcolor(int red, int green, int blue)`: Sets the RGB foreground text color.

## Variables

Some variables are also exposed by this library:
- `_sys_keyright`: Right arrow
- `_sys_keyleft`: Left arrow
- `_sys_keydown`: Down arrow
- `_sys_keyup`: Up arrow
- `_sys_keydel`: Del key
- `_sys_keyhomekey`: Home key
- `_sys_keyendkey`: End key
- `_sys_keyc_up`: Control + Up arrow
- `_sys_keyc_down`: Control + Down arrow
- `_sys_keyc_right`: Control + Right arrow
- `_sys_keyc_left`: Control + Left arrow
- `_sys_keybackspace`: Backspace
- `_sys_keyescape`: Escape key

For Windows, the following variables have been added:
- `_sys_keyc_homekey`: Control + Home key
- `_sys_keyc_endkey`: Control + End key

These variables can be compared against `getchar`, which returns these values when a key is pressed.

## Example

```cpp
// We check if a character was hit
string c = _sys.getchar();
println(c.bytes()); // We display its content

// If the user presses the up arrow, we display all possible colors
if (c == _sys_keyup) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 99; j++) {
            for (int k = 0; k < 99; k++) {
                _sys.colors(i, j, k);
                println("test:", i, j, k);
            }
        }
    }
}
```

## Mouse Tracking

You can also track the mouse within your terminal. Here is an example: