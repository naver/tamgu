## Common Methods

Most of the objects described in the following section share the following methods, which are used to handle the label associated with a window, a box, an input, etc.

These methods, when used without any parameters, return their current value.

### Methods

1. `align(int a)`: Defines the label alignment (see below).
2. `backgroundcolor(int color)`: Sets or returns the background color.
3. `coords()`: Returns a vector of the widget coordinates.
4. `coords(int x, int y, int w, int h)`: Sets the widget coordinates. It also accepts a vector instead of the four values.
5. `created()`: Returns true if the object has been correctly created.
6. `hide()`: Hides a widget.
7. `label(string txt)`: Sets the label with a new text.
8. `labelcolor(int c)`: Sets or returns the font color of the label.
9. `labelfont(int f)`: Sets or returns the font of the label.
10. `labelsize(int i)`: Sets or returns the font size of the label.
11. `labeltype(int i)`: Sets or returns the font type of the label (see below for a description of the different types).
12. `selectioncolor(int color)`: Sets or returns the widget selected color.
13. `show()`: Shows a widget.
14. `timeout(float f)`: Sets the timeout of an object within a thread.
15. `tooltip(string txt)`: Associates a widget with a tooltip.

### Label Types

- `FL_NORMAL_LABEL`
- `FL_NO_LABEL`
- `FL_SHADOW_LABEL`
- `FL_ENGRAVED_LABEL`
- `FL_EMBOSSED_LABEL`

### Alignment

- `FL_ALIGN_CENTER`
- `FL_ALIGN_TOP`
- `FL_ALIGN_BOTTOM`
- `FL_ALIGN_LEFT`
- `FL_ALIGN_RIGHT`
- `FL_ALIGN_INSIDE`
- `FL_ALIGN_TEXT_OVER_IMAGE`
- `FL_ALIGN_IMAGE_OVER_TEXT`
- `FL_ALIGN_CLIP`
- `FL_ALIGN_WRAP`
- `FL_ALIGN_IMAGE_NEXT_TO_TEXT`
- `FL_ALIGN_TEXT_NEXT_TO_IMAGE`
- `FL_ALIGN_IMAGE_BACKDROP`
- `FL_ALIGN_TOP_LEFT`
- `FL_ALIGN_TOP_RIGHT`
- `FL_ALIGN_BOTTOM_LEFT`
- `FL_ALIGN_BOTTOM_RIGHT`
- `FL_ALIGN_LEFT_TOP`
- `FL_ALIGN_RIGHT_TOP`
- `FL_ALIGN_LEFT_BOTTOM`
- `FL_ALIGN_RIGHT_BOTTOM`
- `FL_ALIGN_NOWRAP`
- `FL_ALIGN_POSITION_MASK`
- `FL_ALIGN_IMAGE_MASK`