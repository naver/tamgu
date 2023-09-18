# Biblioteca xml: tipo xmldoc

Este tipo se utiliza para manejar documentos XML. Se puede utilizar para crear un nuevo documento XML o para analizar uno existente. Es posible asociar una función con una variable xmldoc al analizar un documento para tener acceso a cada nodo mientras se procesa. Este tipo se accede a través de la biblioteca xml: `use("xml")`.

## Métodos

1. `close()`: Cierra el documento XML actual y libera la memoria de todos los valores XML.
2. `create(string topnode)`: Crea un nuevo documento XML, cuyo nodo principal tiene como nombre topnode. Si topnode es una estructura XML completa, entonces se utiliza para crear el documento.
3. `load(string filename)`: Carga un archivo XML.
4. `node()`: Devuelve el nodo principal del documento.
5. `onclosing(function f, myobject o)`: Función que se llama cuando se encuentra una etiqueta de cierre (ver función asociada a continuación).
6. `parse(string buffer)`: Carga un búfer XML.
7. `save(string filename, string encoding)`: Guarda un documento XML. Si se omite encoding, entonces encoding es "utf-8".
8. `serialize(object)`: Serializa cualquier objeto Tamgu como un documento XML.
9. `serializestring(object)`: Serializa cualquier objeto Tamgu como un documento XML y devuelve la cadena correspondiente. El documento también se limpia en el proceso.
10. `xmlstring()`: Devuelve un documento XML como una cadena.
11. `xpath(string myxpath)`: Evalúa una expresión XPath y devuelve un vector de nodos XML.

## Función asociada

La función asociada debe tener la siguiente firma:

```lua
function xmlnode(xml n, object)
```

Debe declararse de la siguiente manera:

```lua
use("xml");
xmldoc mydoc(obj) with xmlnode;
```