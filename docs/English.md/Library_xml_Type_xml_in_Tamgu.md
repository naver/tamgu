# Library xml: Type xml in Tamgu

The `xml` type provides methods for handling XML nodes.

**Important:** This type is implemented as a placeholder for the `xmlNodePtr` type from the libxml2 library (see [http://xmlsoft.org/](http://xmlsoft.org/)). Therefore, the `new` method is necessary to create a new object for the current variable.

This type is accessed through the `xml` library: `use("xml");`

## Methods

1. `child()`: Returns the first child node under the current node.
2. `child(xml)`: Adds an XML node as a child.
3. `content()`: Returns the content of a node.
4. `content(string n)`: Changes the content of a node.
5. `delete()`: Deletes the current internal node.
6. `line()`: Returns the line number of the current node.
7. `id()`: Returns the id of the current node (only with call functions).
8. `name()`: Returns the XML node name.
9. `name(string n)`: Changes the XML node name.
10. `namespace()`: Returns the namespace of the current node as a vector.
11. `new(string n)`: Creates a new internal node.
12. `next()`: Returns the next XML node.
13. `next(xml)`: Adds an XML node after the current node.
14. `parent()`: Returns the parent node above the current node.
15. `previous()`: Returns the previous XML node.
16. `previous(xml)`: Adds an XML node before the current node.
17. `properties()`: Returns the properties of the XML node.
18. `properties(map props)`: Properties are stored in a map as attribute/value.
19. `root()`: Returns the root node of the XML tree.
20. `xmlstring()`: Returns the XML subtree as a string.
21. `xmltype()`: Returns the type of the XML node as a string (the XML node name).

## Example

```tamgu
use("xml");

function Test(xml n, self nn) {
    map m = n.properties();
    println(n.name(), m, n.content());
}

xmldoc doc with Test;
doc.load("resTamgu.xml");
xml nd = doc.node();
println(nd);

while (nd != null) {
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