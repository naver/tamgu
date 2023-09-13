# Tamgu LTK

Tamgu LTK is a library that provides various components and functionalities for creating graphical user interfaces. This README.md file provides an overview of the different components and their usage.

## Components

### Fonts

Tamgu LTK provides the following font codes:

- FL_HELVETICA
- FL_HELVETICA_BOLD
- FL_HELVETICA_ITALIC
- FL_HELVETICA_BOLD_ITALIC
- FL_COURIER
- FL_COURIER_BOLD
- FL_COURIER_ITALIC
- FL_COURIER_BOLD_ITALIC
- FL_TIMES
- FL_TIMES_BOLD
- FL_TIMES_ITALIC
- FL_TIMES_BOLD_ITALIC
- FL_SYMBOL
- FL_SCREEN
- FL_SCREEN_BOLD

To use a font, you can specify it when creating a widget or style.

### Line Shapes

Tamgu LTK provides the following line shapes:

- FL_SOLID
- FL_DASH
- FL_DOT
- FL_DASHDOT
- FL_DASHDOTDOT
- FL_CAP_FLAT
- FL_CAP_ROUND
- FL_CAP_SQUARE
- FL_JOIN_MITER
- FL_JOIN_ROUND
- FL_JOIN_BEVEL

These line shapes can be used when drawing lines or shapes.

### Cursor Shapes

Tamgu LTK provides various cursor shapes that can be used in your applications:

- FL_CURSOR_DEFAULT: the default cursor, usually an arrow.
- FL_CURSOR_ARROW: an arrow pointer.
- FL_CURSOR_CROSS: crosshair.
- FL_CURSOR_WAIT: watch or hourglass.
- FL_CURSOR_INSERT: I-beam.
- FL_CURSOR_HAND: hand (up arrow on MSWindows).
- FL_CURSOR_HELP: question mark.
- FL_CURSOR_MOVE: 4-pointed arrow.
- FL_CURSOR_NS: up/down arrow.
- FL_CURSOR_WE: left/right arrow.
- FL_CURSOR_NWSE: diagonal arrow.
- FL_CURSOR_NESW: diagonal arrow.
- FL_CURSOR_NONE: invisible.
- FL_CURSOR_N: for back compatibility.
- FL_CURSOR_NE: for back compatibility.
- FL_CURSOR_E: for back compatibility.
- FL_CURSOR_SE: for back compatibility.
- FL_CURSOR_S: for back compatibility.
- FL_CURSOR_SW: for back compatibility.
- FL_CURSOR_W: for back compatibility.
- FL_CURSOR_NW: for back compatibility.

These cursor shapes can be used to change the appearance of the cursor in your application.

### Simple Window

In Tamgu LTK, you can create a simple window by following these steps:

1. Declare a window object.
2. Begin the window instantiation.
3. Set the window size range.
4. Create widgets within the window.
5. End the window session.
6. Run the window to launch it.

Here's an example code snippet:

```cpp
use("gui");

// Declare the window object
window w;

// Begin the window instantiation
w.begin(300, 200, 1300, 150, "Modification");

// Set the window size range
w.sizerange(10, 20, 0, 0);

// Create a winput widget within the window
txt.create(200, 20, 1000, 50, true, "Selection");

// End the window session
w.end();

// Run the window
w.run();
```

If you don't want to store any widgets in your window, you can replace the `begin` call with a final `end` with `create`.

### Drawing Window

If you need to draw things like lines or circles, you can provide a new drawing function to the window. In Tamgu LTK, this can be done using the `with` keyword.

Here's an example code snippet:

```cpp
window wnd(object) with callback_window;

function callback_window(window w, type o) {
    // Drawing methods here
}

wnd.run();
```

In the above code, `object` is the variable that will be passed to the associated function when it is called. The associated function should be a sequence of drawing methods.

### Mouse Actions

Tamgu LTK provides a way to track different mouse actions through a callback function. The `onmouse` method associates a mouse action with a specific callback function.

Here's an example code snippet:

```cpp
onmouse(action, callback, myobject);
```

The `action` parameter must be one of the following values: `FL_PUSH`, `FL_RELEASE`, `FL_MOVE`, `FL_DRAG`, or `FL_MOUSEWHEEL`.

The `callback` function must have the following signature:

```cpp
function callback_mouse(window w, map coords, type myobject) {
    // Callback function code
}
```

The `coords` parameter is a map that contains information about the mouse action, such as the button that was pushed, the mouse coordinates, and the mouse wheel increments.

## Conclusion

Tamgu LTK provides a wide range of components and functionalities for creating graphical user interfaces. With its easy-to-use syntax and powerful features, it is a great choice for developing GUI applications.