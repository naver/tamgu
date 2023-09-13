## Window

The `Window` class is the entry point of this graphical library. It provides various methods that can be used to display boxes, buttons, sliders, and more.

### Methods

1. `alert(string msg)`: Displays a pop-up window with an alert message.
2. `arc(float x, float y, float rad, float a1, float a2)`: Draws an arc.
3. `arc(int x, int y, int x1, int y1, float a1, float a2)`: Draws an arc.
4. `ask(string msg, string buttonmsg2, string buttonmsg1, ...)`: Displays a pop-up window with a question and returns a value based on which button was pressed (up to 4 buttons).
5. `begin(int x, int y, int w, int h, string title)`: Creates a window and starts the initialization. `w` and `h` are optional.
6. `bitmap(bitmap image, int color, int x, int y)`: Displays a bitmap at the specified position.
7. `bitmap(bitmap image, int color, int x, int y, int w, int h)`: Displays a bitmap inside a specified box.
8. `border(bool b)`: Adds or removes borders from the window. If called without a parameter, it returns whether the window has borders.
9. `circle(int x, int y, int r, int color)`: Draws a circle. The `color` parameter is optional and determines the fill color of the circle.
10. `close()`: Closes the window.
11. `create(int x, int y, int w, int h, string title)`: Creates a window without widgets. `w` and `h` are optional.
12. `cursor(int cursortype, int color1, int color2)`: Sets the cursor shape. See below for a list of cursor shapes.
13. `drawcolor(int c)`: Sets the color for the next drawings.
14. `drawtext(string l, int x, int y)`: Puts text at the specified position.
15. `end()`: Ends the creation of the window.
16. `flush()`: Forces a redraw of all windows.
17. `resizable(object)`: Makes the object resizable.
18. `rgbcolor(int color)`: Returns a vector of the RGB components of the specified color.
19. `rgbcolor(int r, int g, int b)`: Returns the color corresponding to the combination of the RGB components.
20. `rgbcolor(vector rgb)`: Returns the color corresponding to the combination of the RGB components stored in a vector.
21. `rotation(float x, float y, float distance, float angle, bool draw)`: Computes the coordinates of a rotated point from the given parameters. Returns a vector with the new coordinates. If `draw` is true, the line is actually drawn.
22. `run()`: Launches the GUI.
23. `scrollbar(int x, int y, int wscroll, int hscroll, int vwscroll, int vhscroll)`: Creates a scrollbar zone with the specified dimensions. Requires a window callback function to draw within this zone.
24. `size()`: Returns a vector with the window size.
25. `size(int x, int y, int w, int h)`: Resizes the window.
26. `sizerange(int minw, int minh, int maxw, int maxh)`: Defines the range in which the window size can evolve.
27. `textsize(string l)`: Returns a map with the width and height of the string in pixels.
28. `unlock()`: Unlocks the FLTK.
29. `onclose()`: Intercepts the closing of a window with a special callback function. The function should return true if the action of closing the window is to be processed.
30. `ontime()`: Defines a function that is called every tth second. This function must have the following parameters: `timeout_callback(window w, object o)`.

### Drawing Complex Shapes

The following instructions extend the previous instructions. The complex drawing functions allow you to draw arbitrary shapes with 2-D linear transformations. The functionality is similar to that found in the Adobe® PostScript™ language. The exact pixels that are filled are less defined than for the fast drawing functions, allowing FLTK to take advantage of drawing hardware. On both X and MS Windows, the transformed vertices are rounded on close.

### Example

```cpp
use("gui"); 

function closing(window w, bool close) { 
    if (close == false) { 
        println("We cannot close this window"); 
        return false; 
    } 
    return true; 
} 

window w; 
bool closed = false; 

w.begin(300, 200, 1300, 150, "Modification"); 
w.onclose(closing, closed);  
```

### Colors

The Tamgu LTK library implements a few simple ways to select colors. Colors are represented as integers.

The predefined colors are:

- FL_GRAY0
- FL_DARK3
- FL_DARK2
- FL_DARK1
- FL_LIGHT1
- FL_LIGHT2
- FL_LIGHT3
- FL_BLACK
- FL_RED
- FL_GREEN
- FL_YELLOW
- FL_BLUE
- FL_MAGENTA
- FL_CYAN
- FL_DARK_RED
- FL_DARK_GREEN
- FL_DARK_YELLOW
- FL_DARK_BLUE
- FL_DARK_MAGENTA
- FL_DARK_CYAN
- FL_WHITE

To define your own colors, you can use the `rgbcolor` method provided by Tamgu. It has the following forms:

- `vector rgb = rgbcolor(int c)`: Returns a vector containing the RGB components of the specified color.
- `int c = rgbcolor(vector rgb)`: Takes a vector containing the RGB components and returns the equivalent color.
- `int c = rgbcolor(int r, int g, int b)`: Same as above, but takes the three components individually.