# winput (input zone)

The `winput` object defines an input area in a window, which can be used in conjunction with a callback function that will be called when the zone is dismissed.

## Methods

1. `i[a]`: Extract the character from the input at position `a`.
2. `i[a:b]`: Extract the characters between `a` and `b`.
3. `color(int c)`: Set or return the text color.
4. `create(int x, int y, int w, int h, boolean multiline, string label)`: Create an input area with multiline if this parameter is `true`.
5. `font(string s)`: Set or return the text font.
6. `fontsize(int c)`: Set or return the text font size.
7. `insert(string s, int p)`: Insert `s` at position `p` in the input.
8. `selection()`: Return the selected text in the input.
9. `value()|(string v)`: Return the input buffer or set the initial buffer.
10. `word(int pos)`: Return the word at position `pos`.

## Example

```java
use("gui");
frame block {
    // We first declare our window object
    window w;
    string finalResult;
    
    function result(winput txt, block bb) {
        // We store the content of that field in a variable for further use
        finalResult = txt.value();
    }
    
    // We first declare our winput associated with result
    winput txt(this) with result;
    
    function launch() {
        // We then begin our window instantiation
        w.begin(300, 200, 1300, 150, "Modification");
        // We want our window to be resizable
        w.sizerange(10, 20, 0, 0);
        // We create our multiline winput, which is placed within the current
        // window
        txt.create(200, 20, 1000, 50, true, "Selection");
        // We initialize our input with some text
        txt.value("Some Input Text");
        // The text will be in BLUE
        txt.color(FL_BLUE);
        // No more objects, we end the session
        w.end();
        // We want our text to follow the size of the main window
        w.resizable(txt);
        // We then launch our window
        w.run();
    }
}

// We open a block
block b;
// Which will display our input
b.launch();
// b.finalResult contains the string that was keyed in
println("Result:", b.finalResult);
```