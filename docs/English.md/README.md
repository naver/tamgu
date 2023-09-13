## README

This README provides an improved version of the original text.

### Keyboard Actions

To associate a keyboard action with a callback function, use the following function:

`onkey(action, callback, myobject)`

- `action` must be one of the following values:
  - `FL_KEYUP`: when a key is pushed
  - `FL_KEYDOWN`: when a key is released

The `callback` function must have the following signature:

`function callback_key(window w, string skey, int ikey, int combine, myobject object)`

- The first parameter is the window itself.
- The second parameter is the text matching the key that was pressed.
- The third parameter is the key code.
- The fourth parameter is a combination of all command keys that were pressed together with the current key.
- The last parameter is the object that was provided with the key function.

### Combination Values

The combination value is a binary coded integer with the following possible values:
- 1: the ctrl-key was pressed
- 2: the alt-key was pressed
- 4: the command-key was pressed
- 8: the shift-key was pressed

### Example

```javascript
use("gui");

// Declare the window, drawing function, and object
function pushed(window w, string skey, int ikey, int comb, mycoord o) {
    // If the key pressed is "J", move the rectangle by 10 pixels up and down
    if (skey == "J") {
        o.x += 10;
        o.y += 10;
        // Redraw the whole stuff to update the coordinates
        w.redraw();
    }
}

window wnd(coords) with display;

wnd.begin(100, 100, 300, 300, "Drawing");
wnd.sizerange(10, 10, 0, 0);
// Add a key callback, every push of the key will trigger a call to "pushed"
// Sharing the same object coords with the window
wnd.onkey(FL_PUSH, pushed, coords);
wnd.end();
wnd.run();
```

### Adding a Menu

Adding a menu to a window requires more work than for other elements of the interface. A menu is composed of a series of top menu items, and for each of these top menu items, you must provide a specific description.

### Moving Rectangle

Since FLTK is event-based, animation can be done with a proper function. The only way is to use a thread, which will run on its own, independently from the window environment.

```javascript
use("gui");

// A small frame to record our data
frame mycoord {
    int x, y;
    function _initial() {
        x = 0;
        y = 0;
    }
}

// Declare the object to record our data
mycoord coords;
bool first = true;

// Display function
// Every time the window is modified, this function will be called with a mycoord object
function display(window w, mycoord o) {
    if (first) {
        w.drawcolor(FL_RED);
        w.drawtext("Press T", 20, 20);
    } else {
        // Select the color to be applied to all objects that follow
        w.cursorstyle(FL_CURSOR_CROSS, FL_BLACK, FL_WHITE);
        w.drawcolor(FL_RED);
        w.rectangle(o.x, o.y, 60, 60);
        // Add some text
        w.drawtext("TEST", o.x + 20, o.y + 20);
    }
}

wnd.run();
```

### Creating Windows in a Thread

It is possible to create windows within a thread with some specific precautions. FLTK does not allow the creation of windows within a thread per se, however, a message mechanism is available which can be used to post window creation or enrichment requests.

- Set a lock around the window creation itself to avoid problems.
- Define a timeout to avoid any inner locking when the creation of a window fails.
- If any problem occurs during the widget creation, close the window under scope.
- Whenever a window is moved or modified by a user, this might result in a momentary freeze of other thread display since the display and update of windows can only be done within the main thread.

### Example

```javascript
use("gui");

int px = 300;
int py = 400;
int nb = 1;

// This thread will display a counter
thread bouge() {
    int i = 0;
    window wx;
    woutput wo;
    string err;

    // Initialize the main window with a timeout that will be shared by all sub-objects
    wx.timeout(0.1);

    // Main lock to allow only one thread to create a window at a time
    lock("creation");

    function pressed(button b, self n) {
        bouge();
    }

    // Declare the window, associated drawing function, and the object coord
    window wnd;

    // Instantiate the mouse callback
    wnd.begin(100, 50, 500, 300, "Drawing");
    button b with pressed;
    b.create(430, 20, 60, 60, FL_Regular, FL_NORMAL_BUTTON, "Ok");
    wnd.sizerange(10, 10, 0, 0);
    // Add a mouse callback
    wnd.end();
    wnd.run();
}
```