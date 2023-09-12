# wtree and wtreeitem

These two objects are used to handle a clickable tree. The first object is the tree itself, which is composed of a set of wtreeitem objects. The tree is displayed as a hierarchy of nodes, and each node can be selected through a callback function.

## wtree Methods

1. `add(string path)`: Add a tree item and return the new wtreeitem.
2. `add(wtreeitem e, string n)`: Add a tree item after e and return the new wtreeitem.
3. `clear()`: Delete all tree items.
4. `clicked()`: Return the element that was clicked.
5. `close(string path)`: Close the specified element.
6. `close(wtreeitem e)`: Close the specified element.
7. `connectorcolor(int c)`: Set or return the connector color.
8. `connectorstyle(int s)`: Set or return the connector style (see below).
9. `connectorwidth(int s)`: Set or return the connector width.
10. `create(int x, int y, int h, int w, string label)`: Create a tree.
11. `find(string path)`: Return the element matching the specified path.
12. `first()`: Return the first element.
13. `insert(wtreeitem e, string label, int pos)`: Insert an element after e with the specified label at the specified position in the children list.
14. `insertabove(wtreeitem e, string label)`: Insert an element above e with the specified label.
15. `isclosed(string path)`: Return true if the specified element is closed.
16. `isclosed(wtreeitem e)`: Return true if the specified element is closed.
17. `itembgcolor(int c)`: Set or return the item background color.
18. `itemfgcolor(int c)`: Set or return the foreground color.
19. `itemfont(int c)`: Set or return the item font.
20. `itemsize(int c)`: Set or return the item font size.
21. `last()`: Return the last element as a wtreeitem.
22. `marginleft(int s)`: Set or get the amount of white space (in pixels) that should appear between the widget's left border and the tree's contents.
23. `margintop(int s)`: Set or get the amount of white space (in pixels) that should appear between the widget's top border and the top of the tree's contents.
24. `next(wtreeitem e)`: Return the next element after e as a wtreeitem.
25. `open(string path)`: Open the specified element.
26. `open(wtreeitem e)`: Open the specified element.
27. `previous(wtreeitem e)`: Return the previous element before e as a wtreeitem.
28. `remove(wtreeitem e)`: Remove the specified element from the tree.
29. `root()`: Return the root element as a wtreeitem.
30. `rootlabel(string r)`: Set the root label.
31. `selectmode(int s)`: Set or return the selection mode (see below).
32. `sortorder(int s)`: Set or return the sort order (see below).

## wtreeitem Methods

1. `activate()`: Activate the current element.
2. `bgcolor(int c)`: Set or return the item background color.
3. `child(int i)`: Return the child element at position i.
4. `children()`: Return the number of children.
5. `clean()`: Remove the object associated with the value.
6. `deactivate()`: Deactivate the current element.
7. `depth()`: Return the depth of the item.
8. `fgcolor(int c)`: Set or return the foreground color.
9. `font(int c)`: Set or return the item font.
10. `fontsize(int c)`: Set or return the item font size.
11. `isactive()`: Return true if the element is active.
12. `isclosed()`: Return true if the element is closed.
13. `isopen()`: Return true if the element is open.
14. `isroot()`: Return true if the element is root.
15. `isselected()`: Return true if the element is selected.
16. `label()`: Return the item label.
17. `next()`: Return the next element.
18. `parent()`: Return the parent element.
19. `previous()`: Return the previous element.
20. `value()`: Return the value associated with the object.
21. `value(object)`: Associate the item with a value.

## Callback

It is possible to associate a wtree object with a callback. This callback must have the following signature:

```c
function wtree_callback(wtree t, wtreeitem i, int reason, myobject o);
```

This function will be called each time an item is selected from the tree. The `reason` parameter can have one of the following values:

- `FL_TREE_REASON_NONE`: Unknown reason.
- `FL_TREE_REASON_SELECTED`: An item was selected.
- `FL_TREE_REASON_DESELECTED`: An item was deselected.
- `FL_TREE_REASON_OPENED`: An item was opened.
- `FL_TREE_REASON_CLOSED`: An item was closed.

## Iterator

The wtree object can be iterated over.

## Path

Certain functions, such as `add` or `find`, require a path. A path is similar to a Unix path and defines a path from the root to the leaf. For example: "/Root/Top/subnode".

## Connector style

The style of connectors between nodes is controlled by the following flags:

- `FL_TREE_CONNECTOR_NONE`: Use no lines connecting items.
- `FL_TREE_CONNECTOR_DOTTED`: Use dotted lines connecting items (default).
- `FL_TREE_CONNECTOR_SOLID`: Use solid lines connecting items.

## Selection mode

The way nodes are selected in the tree is controlled by the following flags:

- `FL_TREE_SELECT_NONE`: Nothing is selected when items are clicked.
- `FL_TREE_SELECT_SINGLE`: Only a single item is selected when an item is clicked (default).
- `FL_TREE_SELECT_MULTI`: Multiple items can be selected by clicking with.

## Sort order

Items can be added to the tree in an ordered manner controlled by the following flags:

- `FL_TREE_SORT_NONE`: No sorting; items are added in the order defined (default).
- `FL_TREE_SORT_ASCENDING`: Add items in ascending sort order.
- `FL_TREE_SORT_DESCENDING`: Add items in descending sort order.

## Example

```c
use("gui");
FL_TREE_SELECT_NONE; // Nothing selected when items are clicked.
FL_TREE_SORT_NONE; // No sorting; items are added in the order defined (default).
```