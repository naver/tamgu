# wchoice

Tamgu ltk provides a specific type to propose selections in a list. This element must be initialized with a specific menu, which will be described later on. It exposes the following methods.

## Methods

1. `create(int x, int y, int w, int h, string label)`: Creates a choice.
2. `font(string s)`: Sets or returns the text font.
3. `fontsize(int c)`: Sets or returns the text font size.
4. `menu(vector s)`: Initializes the menu. This should be the last operation in a wchoice creation.
5. `value(int s)`: Sets the choice initialization value.

## Menu

A menu description is a vector of vectors, each containing three elements.

```cpp
vmenu = [["First", callback, "1"], ["Second", callback, "2"], ["Third", callback, "3"]];
```

A menu item contains its name, the callback function it is associated with, and the object that will be passed to this callback function.

Menu Item: `[name, callback, object]`

The callback function must have the following signature:

```cpp
function callback_menu(wchoice c, myobject obj);
```

This function is called for each selection from the list.

## Example

```cpp
use("gui");
window w;

function callback_menu(wchoice c, string s) {
    println(s);
}

vector vmenu;

// Our menu description
vmenu = [["Premier", callback_menu, "RRRR"], ["Second", callback_menu, "OOOOOO"], ["Third", callback_menu, "BBBBBBB"]];

wchoice wch;

// We create our window
w.begin(300, 200, 1300, 500, "Fenetre");

// We create our choice widget
wch.create(20, 420, 100, 50, "Choix");
wch.fontsize(20);

// This should be the last operation on the selection list...
wch.menu(vmenu);

w.end();
w.run();
```