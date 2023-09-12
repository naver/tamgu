# Important: key "#"

The map should always have a "#" key which is used to define the default style. If this key is not provided, an exception will be raised.

Once this map has been designed, you should pass it to the system with the instruction: `addstyle(m)`.

To use this style on a section of text, use `setstyle` with one of the above keys as a way to select the correct style.

## Example

```python
use("gui")

# A map describing the styles available within the editor
m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'A': [FL_BLUE, FL_COURIER_BOLD, FL_NORMAL_SIZE],
    'B': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'C': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'D': [FL_BLUE, FL_COURIER, FL_NORMAL_SIZE],
    'E': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE],
    'F': [FL_DARK_RED, FL_COURIER_BOLD, FL_NORMAL_SIZE]
}

w = window()
e = editor()

w.begin(300, 200, 1300, 700, "Modification")
w.sizerange(10, 20, 0, 0)
e.create(200, 220, 1000, 200, "Editor")
e.addstyle(m)
e.value("This is an interesting style")
# We use the style of key C on "interesting"
e.setstyle(10, 22, 'C')
e.annotate("a", 'E') # each "a" is assigned the E style
w.end()
w.run()

# Modifying style

It is actually possible to redefine a style for a given editor. The function `addstyle` must be called again.

```python
# A map describing the styles available within the editor
m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'truc': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE]
}

# We modify one item in our map... We keep the same key...
# The section in the text based on 'truc' will be modified...
function test(button b, editor e) {
    m["truc"] = [FL_DARK_GREEN, FL_COURIER, FL_NORMAL_SIZE];
    e.addstyle(m);
}

w = window()
e = editor()
b = button(e, test)

w.begin(300, 200, 1300, 700, "Modification")
w.sizerange(10, 20, 0, 0)
e.create(200, 220, 1000, 200, "Editor")
e.addstyle(m)
e.value("This is an interesting style")
e.setstyle(10, 22, 'truc')
b.create(1230, 20, 30, 30, FL_Regular, FL_NORMAL_BUTTON, "Ok")
w.end()
w.run()

# Style Messages

It is also possible to associate a style with a specific message. This message will be displayed when the mouse hovers above an element having that style. The only modification necessary is to add one or two more elements to each item from the style description.

A style description is composed of: `[itemcolor, font, fontsize]`.

We can add a message to that item: `[itemcolor, font, fontsize, "Message"]`.

And even a color which will be used as a background color for that message: `[itemcolor, font, fontsize, "Message", backgroundcolor]`.

If the background color is not provided, then the defined color itemcolor from the style will be used.

## Example

```python
use("gui")

m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'truc': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE, "THIS IS A TRUC", FL_YELLOW]
}

# When the mouse hovers above a piece of text with the style truc, it will display a yellow box with the message: THIS IS A TRUC.

# Callbacks: scrolling, mouse and keyboard

The callback must have the following signature:

```python
w = window()
e = editor()
num = woutput()

w.begin(300, 200, 1300, 700, "Window")
w.sizerange(10, 20, 0, 0)
num.create(100, 100, 30, 40, "Line")
e.create(200, 220, 1000, 200, "Editor")
e.onmouse(FL_RELEASE, cmouse, num)
e.onvscroll(cvscroll, num)
e.onkey(FL_KEYUP, ckey, num)
w.end()
w.run()

# Sticky notes

The following example shows how to display a little sticky note on words with a specific style in your editor.

```python
# A map describing the styles available within the editor
m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'movement': [FL_RED, FL_COURIER, FL_NORMAL_SIZE]
}

# We define the words that we want to recognize in the text
mvt = ["move", "run", "stride", "walk", "drive"]

# Whenever the text is modified, we check for our above words
function modified(editor e, int pos, int ninserted, int ndeleted, int restyled, string del, self obj) {
    # We unmark everything first
    e.setstyle(0, e.size(), "#")
    # Then, we mark all our movement words
    e.annotate(mvt, "movement")
}

w = window()
e = editor()

e.create(200, 220, 1000, 200, "Editor")
e.addstyle(m)
# We also need a mouse callback
e.onmouse(vmouse, null)
w.end()
w.run()