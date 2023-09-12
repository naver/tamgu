# wcounter

The `wcounter` module in Tamgu ltk offers two types of counters. One type displays two steps of progression, while the other type displays only one.

In order to catch value modifications, a `wcounter` must be attached with a callback function. The callback function must have the following signature:

```javascript
function callback_counter(s, obj) {
    // the counter value is returned with value()
    println(s.value());
}
```

To create a counter with a callback function, use the following syntax:

```javascript
counter s(obj) with callback_counter;
```

The `counter` object exposes the following methods:

1. `bounds(float x, float y)`: Defines the counter boundary.
2. `create(int x, int y, int w, int h, int alignment, string label)`: Creates a counter.
3. `font(int s)`: Sets or returns the text font.
4. `lstep(float)`: Defines the large counter step.
5. `step(float)`: Defines the counter step.
6. `steps(float)`: Defines the counter steps, normal and large.
7. `textcolor(string code|int code)`: Sets the color of the text.
8. `textsize(string l)`: Returns a map with width and height of the string in pixels.
9. `type(bool normal)`: If `true`, then it's a normal counter, otherwise it's a simple counter.
10. `value(float)`: Defines the value for the counter or returns its value.

Example usage:

```javascript
use("gui");
window w;

function tst(wcounter e, self i) {
    printlnerr(e.value());
}

wcounter c with tst;
w.begin(50, 50, 500, 500, "test");
c.create(30, 30, 300, 100, "Counter");
c.steps(0.01, 0.1);
c.textsize(20);
c.textcolor(FL_RED);
w.end();
w.run();
```