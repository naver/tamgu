# Slider

Tamgu ltk offers two types of sliders. One type of slider displays the value along with the slide bar itself.

To catch any modifications, the slider must be attached with a callback function. The function must have the following signature:

```javascript
function callback_slider(slider s, myobj obj) {
  // the slider value is returned with value()
  println(s.value());
}
```

To create a slider with a callback function, use the following syntax:

```javascript
slider s(obj) with callback_slider;
```

The slider exposes the following methods:

Methods:
1. `align(int align)`: defines the slider alignment
2. `bounds(int x, int y)`: defines the slider boundaries
3. `create(int x, int y, int w, int h, int align, bool valueslider, string label)`: creates a slider or a valueslider (see below for a list of alignment values)
4. `resize(object)`: makes the object resizable
5. `step(int)`: defines the slider step
6. `type(int x)`: sets the value slider type (see below for the list of slider types)
7. `value()`: returns the slider value
8. `value(int i)`: sets the initial value for the slider

Slider types:
- FL_VERT_SLIDER
- FL_HOR_SLIDER
- FL_VERT_FILL_SLIDER
- FL_HOR_FILL_SLIDER
- FL_VERT_NICE_SLIDER
- FL_HOR_NICE_SLIDER

## Example

This example shows how a slider can control the movement of a rectangle in another window.

```javascript
use("gui");

// A small frame to record our data
frame mycoord {
  int color;
  int x, y;

  function _initial() {
    color = FL_RED;
    x = 0;
    y = 0;
  }
}

// We declare our object, which will record our data
mycoord coords;

// We declare our window together with its associated drawing function and the object coord
window wnd(coords) with display;

// We cheat a little bit as we use the global variable wnd to access our window
function slidercall(slider s, mycoord o) {
  // We position our window X according to the slider value
  o.x = s.value();
  wnd.redraw();
}

slider vs(coords) with slidercall;

// We need to instantiate the mouse callback
wnd.begin(100, 100, 300, 300, "Drawing");
wnd.sizerange(10, 10, 0, 0);

// We create our value slider
vs.create(10, 10, 180, 20, FL_ALIGN_LEFT, true, "Position");

// The values will be between 0 and 300
vs.bounds(0, 300);

// With the initial value 100
vs.value(100);

wnd.end();
wnd.run();
```