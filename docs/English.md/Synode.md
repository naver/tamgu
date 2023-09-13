# Synode

A synode is a syntactic node defined by a set of features (implemented here as a map) and its position in the constituent tree. The synode class provides the following methods:

1. `initial(map m)`: Creates a syntactic node with the specified features.
2. `addchild(synode)`: Adds a child node.
3. `addnext(synode)`: Adds a node after the current node.
4. `addprevious(synode)`: Adds a node before the current node.
5. `after(synode)`: Returns true if the node is after the specified node under the same parent.
6. `attributes()`: Returns the feature attributes as a vector.
7. `before(synode)`: Returns true if the node is before the specified node under the same parent.
8. `child()`: Returns the first child node or checks if it matches the specified parameter.
9. `children()`: Returns the list of children for a given node or checks if the node is a child.
10. `definitions(map)`: Sets the valid feature definitions for all synodes.
11. `last()`: Returns the last child node or checks if it matches the specified parameter.
12. `nbchildren()`: Returns the number of direct children.
13. `next(synode)`: Returns the next node or checks if it matches the specified parameter.
14. `parent()`: Returns the parent node or checks if it matches the specified parameter.
15. `previous(synode)`: Returns the previous node or checks if it matches the specified parameter.
16. `precede(synode)`: Returns true if the node is before (anywhere in the tree).
17. `sibling(synode)`: Checks if the node is a sibling (either a sister or a descendant).
18. `sisters(synode)`: Returns the list of sister nodes or checks if the node is a sister node.
19. `succeed(synode)`: Returns true if the node is after (anywhere in the tree).
20. `test(string attribute)`: Checks if an attribute is part of the feature structure.
21. `values()`: Returns the feature values as a vector.

Creating a constituent tree:
A constituent tree is built from top to bottom. When using the `addchild` function, it adds a child node under the current node. Each subsequent call to this function adds a new child after the previous child node.

Example:
```cpp
// Create NP node
synode np({"pos": "np"});

// Create three lexical nodes
synode det({"pos": "det", "surface": "the"});
synode adj({"pos": "adj", "surface": "big"});
synode noun({"pos": "noun", "surface": "dog"});

// Add the lexical nodes under np, one after the other
np.addchild(det);
np.addchild(adj);
np.addchild(noun);

// Display the nodes in an indented way
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 4);
    Display(x.next(), i);
}

Display(np, 0);
```

Result:
```
#0['pos':'np']
    #0['pos':'det','surface':'the']
    #0['pos':'adj','surface':'big']
    #0['pos':'noun','surface':'dog']
```

Note: The "#0" indicates that the synode is not a dependency variable.