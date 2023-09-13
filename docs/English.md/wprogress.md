# wprogress

Tamgu ltk offers a progress bar widget that can display a progression from a minimum to a maximum value.

A wprogress can be attached to a callback function in order to capture value modifications. The function must have the following signature:

```javascript
function callback_progress(wprogress s, myobj obj) {
    // the progress value is returned with value()
    println(s.value());
}
```

Create a progress object `s` with `obj` and attach the `callback_progress` function to it.

The progress object exposes the following functions:

Methods:
1. `backgroundcolor(int color)`: set or return the background color
2. `barcolor(string code|int code)`: set the bar color
3. `create(int x, int y, int w, int h, int alignment, string label)`: create a progress bar
4. `minimum(float x)`: define or return the progress bar minimum
5. `maximum(float x)`: define or return the progress bar maximum
6. `value(float)`: define the value for the progress or return its value

Example:
```javascript
use("gui");
window w;
wprogress c;

thread progressing() {
    for (int i in <0, 100, 1>) {
        for (int j in <0, 100000, 1>) {}
        c.value(i);
    }
    printlnerr("End");
}

function launch(button b, self e) {
    progressing();
}

button b with lance;

w.begin(50, 50, 500, 500, "test");
c.create(30, 30, 300, 30, "progression");
b.create(100, 100, 50, 50, "Ok");
c.minimum(0);
c.maximum(100);
c.barcolor(FL_BLUE);
w.end();
w.run();
```