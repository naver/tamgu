# Library xml: type xmldoc in Tamgu

This library provides functionality to handle XML documents in Tamgu. It allows you to create new XML documents, parse existing ones, and perform various operations on them. The library is accessed using the `xml` module.

## Methods

1. `close()`: Closes the current XML document and frees up memory from all XML values.
2. `create(string topnode)`: Creates a new XML document with a specified top node. If the `topnode` parameter is a full XML structure, it will be used to create the document.
3. `load(string filename)`: Loads an XML file.
4. `node()`: Returns the top node of the document.
5. `onclosing(function f, myobject o)`: Associates a function `f` with an `xmldoc` variable to be called when a closing tag is found during parsing.
6. `parse(string buffer)`: Parses an XML buffer.
7. `save(string filename, string encoding)`: Saves the XML document to a file. If the `encoding` parameter is omitted, the encoding will default to "utf-8".
8. `serialize(object)`: Serializes a Tamgu object as an XML document.
9. `serializestring(object)`: Serializes a Tamgu object as an XML document and returns the corresponding string. The document is also cleaned in the process.
10. `xmlstring()`: Returns the XML document as a string.
11. `xpath(string myxpath)`: Evaluates an XPath expression and returns a vector of XML nodes.

## Associated function

The associated function must have the following signature: