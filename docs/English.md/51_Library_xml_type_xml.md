# Library xml: type xml

The `xml` type provides methods to handle XML nodes.

**Important**  
This type is implemented as a placeholder for the `xmlNodePtr` type from the libxml2 library (see [http://xmlsoft.org/](http://xmlsoft.org/)), hence the `new` method which is necessary to get a new object for the current variable.

This type is accessed through the library xml: `use("xml")`.

## Methods

1. `child()`: return the first child node under the current node.
2. `child(xml)`: Add an XML node as a child.
3. `content()`: Return the content of a node.
4. `content(string n)`: Change the content of a node.
5. `delete()`: delete the current internal node.
6. `line()`: return the line number of the current node.
7. `id()`: return the id of the current node (only with call functions).
8. `name()`: return the XML node name.
9. `name(string n)`: Change the XML node name.
10. `namespace()`: Return the namespace of the current node as a vector.
11. `new(string n)`: Create a new internal node.
12. `next()`: return the next XML node.
13. `next(xml)`: Add an XML node after the current node.
14. `parent()`: return the parent node above the current node.
15. `previous()`: return the previous XML node.
16. `previous(xml)`: Add an XML node before the current node.
17. `properties()`: Return the properties of the XML node.
18. `properties(map props)`: Properties are stored in a map as attribute/value.
19. `root()`: return the root node of the XML tree.
20. `xmlstring()`: return the XML subtree as a string.
21. `xmltype()`: return the type of the XML node as a string.

**Example**

```lua
use("xml");

function Test(xml n, self nn) {
    map m = n.properties();
    println(n.name(), m, n.content());
}

xmldoc doc with Test;
doc.load("resTamgu.xml");
xml nd = doc.node();
println(nd);

while(nd != null) {
    println(nd.content(), nd.namespace());
    nd = nd.child();
}

xmldoc nouveau;
nouveau.create("TESTAGE");
xml nd = nouveau.node();
xml n("toto");
nd.child(n);
n.new("titi");
n.content("Toto is happy");
nd.child(n);
nouveau.save("mynewfile.xml");
```