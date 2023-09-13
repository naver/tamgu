# Button

The button object is essential in allowing users to interact with the GUI. A button should be created along with a callback function that has the following signature:

```javascript
function callback_button(button b, myobj obj) { ... }
```

The button object exposes the following methods:

## Methods

1. `align(int)`: Defines the alignment of the button label.
2. `bitmap(bitmap im, int color, string label, int labelalign)`: Uses the bitmap as a button image.
3. `color(int code)`: Sets the color of the button.
4. `create(int x, int y, int w, int h, string type, int shape, string label)`: Creates a button. Refer to the list below for available types and shapes.
5. `image(image im, string label, int labelalign)`: Uses the image as a button image.
6. `shortcut(string keycode)`: Sets a shortcut to activate the button from the keyboard. Refer to the list below for available shortcuts.
7. `value()`: Returns the value of the current button.
8. `when(int when1, int when2, ...)`: Specifies the type of event that triggers the callback. Refer to the events list below.

## Button Types

- FL_Check
- FL_Light
- FL_Repeat
- FL_Return
- FL_Round
- FL_Regular
- FL_Image

## Button Shapes

- FL_NORMAL_BUTTON
- FL_TOGGLE_BUTTON
- FL_RADIO_BUTTON
- FL_HIDDEN_BUTTON

## Events (when)

The following events can be associated with the callback function:

- FL_WHEN_NEVER
- FL_WHEN_CHANGED
- FL_WHEN_RELEASE
- FL_WHEN_RELEASE_ALWAYS
- FL_WHEN_ENTER_KEY
- FL_WHEN_ENTER_KEY_ALWAYS

## Shortcuts

The following shortcuts can be associated with a button:

- FL_Button
- FL_BackSpace
- FL_Tab
- FL_Enter
- FL_Pause
- FL_Scroll_Lock
- FL_Escape

```javascript
w.close();
}

function launch(string ph) {
    final = ph;
    // We begin our window instantiation
    w.begin(300, 200, 1300, 150, "Modification");
    // We want our window to be resizable
    w.sizerange(10, 20, 0, 0);
    // We create our winput, which is placed within the current window
    txt.create(200, 20, 1000, 50, true, "Selection");
    txt.value(ph);
    // We associate our button with the method gettext
    button b(this) with gettext;
    b.create(1230, 20, 30, 30, FL_Regular, FL_NORMAL_BUTTON, "Ok");
    // No more objects, we end the session
    w.end();
    w.resizable(txt);
    // We then launch our window
    w.run();
}

block b;
b.launch("My sentence");
```

## Image

First, we need to load an image, then we create a button with the flag `FL_Image`.

```javascript
image myimage;
// We load a GIF image
image.loadgif('c:\\...');
// We associate our button with the method gettext
button b(this) with gettext;
// We create our image button
b.create(1230, 20, 30, 30, FL_Image, FL_NORMAL_BUTTON, "Ok");
// We associate our button with an inside label within the image
b.image(myimage, "Inside", FL_ALIGN_CENTER);
```