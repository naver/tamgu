# Browser (Browsing Strings)

The `Browser` object defines a box in which strings can be displayed and, if necessary, selected as a list.

## Methods

1. `add(label)`: Add a string to the browser.
2. `clear()`: Clear the browser of all values.
3. `columnChar()`: Return the column char separator.
4. `columnChar(string)`: Set the column char separator.
5. `create(x, y, w, h, label)`: Create a browser.
6. `deselect()`: Deselect all items.
7. `deselect(int i)`: Deselect item `i`.
8. `formatChar()`: Return the format char.
9. `formatChar(string)`: Set the format char.
10. `insert(l, label)`: Insert a label before line `l`.
11. `load(filename)`: Load a file into the browser.
12. `select()`: Return the selected string.
13. `select(int i)`: Return the string at position `i`.
14. `size()`: Return the number of values within the browser.
15. `value()`: Return the currently selected value as an index.

## Selection

The only way to use the browser in selection mode is to associate it with a callback function whose signature must match the following:

```python
function browser_callback(browser b, myobject o);
```

A callback function is declared with `with`.

## Example

```python
use("gui");
// The callback function
function avec(browser b, self n) {
    println("Selection:", b.select(), b.value());
}

window w;
w.begin(40, 40, 400, 500, "Browsing");
browser b with avec;
b.create(10, 10, 100, 150, "Test");
b.add("first");
b.add("second");
b.add("third");
b.add("fourth");
w.end();
w.run();
```