# Tabs and Group

The `tabs` object exposes everything necessary to create tabs in a window. This object is associated with the `group` object, which is used to group widgets together in a single block.

## Tabs Methods

The `tabs` object exposes the following methods:

1. `add(wgroup g)`: Dynamically add a new tab.
2. `begin(int x, int y, int w, int h, string title)`: Create a tab window and begin initialization.
3. `current()`: Return the current active tab.
4. `current(wgroup t)`: Activate this tab.
5. `end()`: End the tabs construction.
6. `remove(wgroup g)`: Remove the group `g` from the tabs.

## Group Methods

The `group` object exposes the following methods:

1. `activate()`: Activate the tab.
2. `begin(int x, int y, int w, int h, string title)`: Create a widget group and begin initialization.
3. `end()`: End the group construction.

## Important

- Creating a tabs section is simple. You create a tabs box, in which all the different elements will be stored.
- For each tab, you need to create a specific widget group.
- The dimension of a group should be smaller in height than the original tabs box.
- Each group should have the same dimension.
- The second group should always be hidden.

## Callback

It is also possible to associate a group with a callback, similar to a window. When a group is declared with an associated callback, this callback is called each time the window must be redrawn. Refer to the window section for more information. Most of the functions available for drawing in the window object are also available for `wgroup`.

## Simple Example

In this example, we build a simple window with two tabs.

```lua
function callback_filebrowser(filebrowser f, myobject object);
```

However, if you do not declare any callback function, the `create` function returns the selected file pathname.

It exposes the following methods.

### Methods

1. `close()`: Close the file browser.
2. `create(string initialdirectory, string filter, int method, string label)`: Open a file browser according to the method (see below). If no callback function is declared, it returns the selected pathname.
3. `ok()`: Return `true` if "Ok" was pressed.
4. `value()`: Return the selected file.

### Method

There are different ways to open a file browser, each with a different action. The possible values are as follows:

- `FL_DIR_SINGLE`: To open a single file at a time.
- `FL_DIR_MULTI`: To open multiple files at a time.
- `FL_DIR_CREATE`: To create a new file.
- `FL_DIR_DIRECTORY`: To select a directory.

### Example

```lua
use("gui");
// We check whether the element was chosen
// Then we close our window
function choose(filebrowser f, self b) {
    if (f.ok()) {
        println("Ok:", f.value());
        b = true;
        f.close();
    }
}
```