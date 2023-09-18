# Library xml: type xmldoc

This type is used to handle XML documents. It can be used to create a new XML document or to parse one. It is possible to associate a function with an xmldoc variable when parsing a document to have access to each node on the fly. This type is accessed through the library xml: `use("xml")`.

## Methods

1. `close()`: Close the current XML document and clean the memory from all XML values.
2. `create(string topnode)`: Create a new XML document, whose main node has topnode as its name. If topnode is a full XML structure, then use it to create the document.
3. `load(string filename)`: Load an XML file.
4. `node()`: Return the top node of the document.
5. `onclosing(function f, myobject o)`: Function to call when a closing tag is found (see associate function below).
6. `parse(string buffer)`: Load an XML buffer.
7. `save(string filename, string encoding)`: Save an XML document. If encoding is omitted, then encoding is "utf-8".
8. `serialize(object)`: Serialize any Tamgu object as an XML document.
9. `serializestring(object)`: Serialize any Tamgu object as an XML document and return the corresponding string. The document is also cleaned in the process.
10. `xmlstring()`: Return an XML document as a string.
11. `xpath(string myxpath)`: Evaluate an XPath and return a vector of XML nodes.

## Associated function

The associated function must have the following signature:

```lua
function xmlnode(xml n, object)
```

It must be declared in the following way:

```lua
use("xml");
xmldoc mydoc(obj) with xmlnode;
```