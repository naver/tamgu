# Biblioteca xml: tipo xml

El tipo `xml` proporciona métodos para manejar nodos XML.

**Importante**  
Este tipo se implementa como un marcador de posición para el tipo `xmlNodePtr` de la biblioteca libxml2 (ver [http://xmlsoft.org/](http://xmlsoft.org/)), de ahí el método `new` que es necesario para obtener un nuevo objeto para la variable actual.

Este tipo se accede a través de la biblioteca xml: `use("xml")`.

## Métodos

1. `child()`: devuelve el primer nodo hijo debajo del nodo actual.
2. `child(xml)`: Agrega un nodo XML como hijo.
3. `content()`: devuelve el contenido de un nodo.
4. `content(string n)`: cambia el contenido de un nodo.
5. `delete()`: elimina el nodo interno actual.
6. `line()`: devuelve el número de línea del nodo actual.
7. `id()`: devuelve el id del nodo actual (solo con funciones de llamada).
8. `name()`: devuelve el nombre del nodo XML.
9. `name(string n)`: cambia el nombre del nodo XML.
10. `namespace()`: devuelve el espacio de nombres del nodo actual como un vector.
11. `new(string n)`: Crea un nuevo nodo interno.
12. `next()`: devuelve el siguiente nodo XML.
13. `next(xml)`: Agrega un nodo XML después del nodo actual.
14. `parent()`: devuelve el nodo padre por encima del nodo actual.
15. `previous()`: devuelve el nodo XML anterior.
16. `previous(xml)`: Agrega un nodo XML antes del nodo actual.
17. `properties()`: devuelve las propiedades del nodo XML.
18. `properties(map props)`: Las propiedades se almacenan en un mapa como atributo/valor.
19. `root()`: devuelve el nodo raíz del árbol XML.
20. `xmlstring()`: devuelve el subárbol XML como una cadena.
21. `xmltype()`: devuelve el tipo del nodo XML como una cadena.

**Ejemplo**

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