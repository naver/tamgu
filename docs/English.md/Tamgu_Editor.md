# Tamgu Editor

Tamgu ltk provides a specific type to handle text editing. The editor allows users to modify text and provides a callback function that is triggered whenever the text is modified.

## Editor Callback Function

The editor callback function has the following signature:

```cpp
void editorcallback(editor e, int pos, int ninserted, int ndeleted, int restyled, string del, myobj obj);
```

The arguments of the callback function are as follows:

- `editor e`: the editor object itself
- `pos`: the current cursor position in the document
- `ninserted`: the number of characters that have been inserted
- `ndeleted`: the number of characters that have been deleted
- `restyled`: the number of characters whose style has been modified
- `del`: the characters that have been deleted
- `obj`: the object associated with the editor through the `with` instruction

The editor callback function is associated with an editor object using the `with` instruction:

```cpp
editor e(obj) with editorcallback;
```

## Editor Methods

The editor object exposes the following methods:

1. `addstyle(map styles)`: Initialize the styles for text chunks. The `styles` argument is a map that describes the available styles within the editor.
2. `annotate(string s, string keystyle, bool matchcase)`: Assign a specific style (`keystyle`) to each occurrence of a string (`s`) in the text. The `matchcase` parameter is optional. If `s` is a regular expression, the `matchcase` parameter is ignored.
3. `append(string s)`: Append a string at the end of the editor text.
4. `byteposition(int pos)`: Convert a character position into a byte position, especially useful for UTF8 strings.
5. `charposition(int pos)`: Convert a byte position into a character position, especially useful for UTF8 strings.
6. `color(int c)`: Set or return the text color.
7. `copy()`: Copy the selected text to the clipboard.
8. `copy(string s)`: Copy a string `s` to the clipboard.
9. `count(string s, int bg)`: Count the number of occurrences of a string `s` starting at position `bg`.
10. `create(int x, int y, int w, int h, string label)`: Create an editor.
11. `cursor()`: Return the current position of the cursor in byte increments.
12. `cursor(int i)`: Move the cursor to the `i`th byte.
13. `cursorchar()`: Return the current position of the cursor in character increments.
14. `onhscroll(function f, object o)`: Set the callback when scrolling horizontally.
15. `onkey(int action, function f, object o)`: Set the callback when scrolling vertically.
16. `onmouse(int action, function f, object o)`: Set the callback when handling the mouse.
17. `onvscroll(function f, object o)`: Set the callback when scrolling vertically.
18. `paste()`: Paste the selected text from the clipboard.
19. `rfind(string s, int i)`: Find a string in the editor text, starting at position `i` backward.
20. `save(string filename)`: Save the content of the editor into a file.
21. `selection()`: Return the selected text in the editor.
22. `setstyle(int start, int end, string keystyle)`: Set a text chunk with a given style from the style table instantiated with `addstyle`.
23. `unhighlight()`: Remove highlighting.
24. `value(string v)`: Return the text in the editor or initialize the editor with a given string.
25. `word(int pos)`: Return the word at position `pos`.
26. `wrap(bool w)`: Wrap the text within the editor window if `w` is `true`.

## Cursor Styles

Tamgu provides different cursor styles that can be set using the `cursorstyle` method:

- `FL_NORMAL_CURSOR`
- `FL_CARET_CURSOR`
- `FL_DIM_CURSOR`
- `FL_BLOCK_CURSOR`
- `FL_HEAVY_CURSOR`

## Adding Styles

In order to display specific sections of text with different fonts, colors, and sizes in the editor, FLTK requires the description of these styles beforehand. Each style is represented by a vector of three elements: `[color, font, size]`, associated with a key that is used to refer to that style item.

Here is an example of a map that describes the available styles within the editor:

```cpp
map m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'A': [FL_BLUE, FL_COURIER_BOLD, FL_NORMAL_SIZE],
    'B': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'C': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'D': [FL_BLUE, FL_COURIER, FL_NORMAL_SIZE],
    'E': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE],
    'F': [FL_DARK_RED, FL_COURIER_BOLD, FL_NORMAL_SIZE]
};
```

Please refer to the FLTK documentation for more details on how to use these methods and styles in your Tamgu application.