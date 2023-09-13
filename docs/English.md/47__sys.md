# Tamgu System Functionalities

Tamgu provides system functionalities such as reading the content of a directory into a vector or executing a system command. It exposes the variable `_sys`, which should be used to access the following methods:

## Methods

1. `command(string s, string outputfile)`: Execute the system command `s`. `outputfile` is optional and is used to redirect the command output (stdout). If `outputfile` is supplied, `command` also returns the content of this file as a string.

2. `createdirectory(string path)`: Create a directory for the given `path`. Return false if the directory already exists or cannot be created.

3. `env(string var)`: Return the value of the environment variable `var`.

4. `env(string var, string value)`: Set the value of the environment variable `var`.

5. `listdirectory(string path)`: Return the files in a directory as a vector.

6. `ls(string path)`: Return the files in a directory as a vector.

7. `mkdir(string path)`: Create a directory for the given `path`. Return false if the directory already exists or cannot be created.

8. `fileinfo(string path)`: Return a map with the following information for a given file:
   - `info["size"]`: size of the file
   - `info["date"]`: date of the file
   - `info["change"]`: date of the last change
   - `info["access"]`: date of the last access
   - `info["directory"]`: true if the path is a directory
   - `info["pathname"]`: the real pathname

9. `pipe(string cmd)`: Execute the command `cmd` and return a vector as a result containing the output of that command.

10. `realpath(string path)`: Return the actual path for a given relative path.

11. `getchar()`: Return the last character keyed in. `getchar` does not echo the characters on screen.

12. `colors(int style, int code1, int code2, bool disp)`: Set text color and return the color string.

13. `foregroundcolor(int color)`: Set foreground text color.

14. `backgroundcolor(int color)`: Set background text color.

15. `rgbforegroundcolor(int red, int green, int blue)`: Set RGB foreground text color.

16. `rgbbackgroundcolor(int red, int green, int blue)`: Set RGB background text color.

17. `scrollmargin(int top, int bottom)`: Define scroll margin area.

18. `deletechar(int nb)`: Delete `nb` characters.

19. `up(int nb)`: Move up `nb` lines.

20. `down(int nb)`: Move down `nb` lines.

21. `right(int nb)`: Move right `nb` characters.

22. `left(int nb)`: Move left `nb` characters.

23. `next_line(int nb)`: Move to the next line down `nb` times.

24. `previous_line(int nb)`: Move to the previous line up `nb` times.

25. `column(int nb)`: Move to column `nb`.

26. `row_column(int row, int column)`: Move to row/column.

27. `home()`: Move cursor to home.

28. `cls()`: Clear screen and return to home position.

29. `hor_vert(int hor, int vert)`: Move to hor/vert.

30. `clearscreen(int nb)`: Clear the screen. `nb` can be 0, 1, 2, or 3 for partial or full screen clearing.

31. `clear()`: Clear the screen.

32. `eraseline(int nb)`: Erase `nb` lines.

33. `scroll_up(int nb)`: Scroll up `nb` characters.

34. `scroll_down(int nb)`: Scroll down `nb` characters.

35. `screensizes()`: Return the screen size for rows and columns.

36. `hasresized()`: Return 'true' if the screen size has changed.

37. `cursor()`: Return the cursor position.

38. `inmouse()`: Enable mouse tracking.

39. `outmouse()`: Disable mouse tracking.

40. `ismouseaction(string key)`: Return true if it is a mouse action.

41. `mousexy(string key)`: Return mouse position.

42. `mousescrollup(string key)`: Return mouse position when scrolling up.

43. `mousescrolldown(string key)`: Return mouse position when scrolling down.

44. `mousedown1(string key)`: Return mouse position when primary click.

45. `mousedown2(string key)`: Return mouse position when secondary click.

46. `mouseup(string key)`: Return mouse position when button up.

47. `mousetrack(string key)`: Return mouse position when the mouse is moving while pressed.

48. `reset()`: Reset mouse mode and return to initial termios values. To be used on Unix machines when `getchar` has been used.

49. `isescapesequence(string key)`: Return true if `key` is an escape sequence.

50. `showcursor(bool show)`: Show/hide cursor.

51. `resizecallback(function f)`: Set the callback function that is called when the terminal window is resized. `f` is a function with two parameters, which are the new row and column sizes.

## Example

```javascript
// This function copies all the files from a given directory to another, if they are more recent than a given date
function cp(string thepath, string topath) {
    // We read the content of the source directory
    vector v = _sys.listdirectory(thepath);
    iterator it;
    string path;
    string cmd;
    map m;
    date t;
    // We set today's date starting at 9 A.M.
    t.setdate(t.year(), t.month(), t.day(), 9, 0, 0);
    it = v;
    for (it.begin(); it.nend(); it.next()) {
        path = thepath + '/' + it.value();
        // If the file is of the right type
        if (".cxx" in path || ".h" in path || ".c" in path) {
            m = _sys.fileinfo(path);
            // If the date is more recent than our current date
            if (m["date"] > t) {
                // We copy it
                cmd = "copy " + path + ' ' + topath;
                println(cmd);
                // We execute our command
                _sys.command(cmd);
            }
        }
    }
}

// We call this function to copy from one directory to another
cp('C:\src', 'W:\src');
```

## Variables

Some variables are also exposed by this library:

- `_sys_keyright`: right arrow
- `_sys_keyleft`: left arrow
- `_sys_keydown`: down arrow
- `_sys_keyup`: up arrow
- `_sys_keydel`: del key
- `_sys_keyhomekey`: home key
- `_sys_keyendkey`: end key
- `_sys_keyc_up`: control+up arrow
- `_sys_keyc_down`: control+down arrow
- `_sys_keyc_right`: control+right arrow
- `_sys_keyc_left`: control+left arrow
- `_sys_keybackspace`: backspace
- `_sys_keyescape`: escape key

For Windows, the following variables have been added:

- `_sys_keyc_homekey`: control+home key
- `_sys_keyc_endkey`: control+end key

These variables can be compared against `getchar`, which returns these values when a key is pressed.

## Example

```javascript
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

```javascript
// We initialize the mouse tracking
_sys.inmouse();
string u;
u = _sys.getchar();
ivector iv;
while (u.ord() != 17) {
    if (_sys.ismouseaction(u)) {
        iv = _sys.mousedown1(u);
        if (iv != [])
            println("Mouse button 1 down at position:", iv);
        iv = _sys.mousexy(u);
        if (iv != [])
            println("Mouse is at position:", iv);
        iv = _sys.mousescrollup(u);
        if (iv != [])
            println("Mouse is scrolling up at position:", iv);
        iv = _sys.mousescrolldown(u);
        if (iv != [])
            println("Mouse is scrolling down at position:", iv);
    }
    u = _sys.getchar();
}
// We deactivate mouse tracking and reset the terminal
_sys.reset();
```