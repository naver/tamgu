# Bibliothèque xml : type xmldoc

Ce type est utilisé pour manipuler des documents XML. Il peut être utilisé pour créer un nouveau document XML ou pour en analyser un. Il est possible d'associer une fonction à une variable xmldoc lors de l'analyse d'un document afin d'avoir accès à chaque nœud en temps réel. Ce type est accessible via la bibliothèque xml : `use("xml")`.

## Méthodes

1. `close()`: Ferme le document XML actuel et libère la mémoire de toutes les valeurs XML.
2. `create(string topnode)`: Crée un nouveau document XML dont le nœud principal a pour nom topnode. Si topnode est une structure XML complète, alors elle est utilisée pour créer le document.
3. `load(string filename)`: Charge un fichier XML.
4. `node()`: Renvoie le nœud principal du document.
5. `onclosing(function f, myobject o)`: Fonction à appeler lorsqu'une balise de fermeture est trouvée (voir fonction associée ci-dessous).
6. `parse(string buffer)`: Charge un tampon XML.
7. `save(string filename, string encoding)`: Enregistre un document XML. Si l'encodage est omis, alors l'encodage est "utf-8".
8. `serialize(object)`: Sérialise n'importe quel objet Tamgu en tant que document XML.
9. `serializestring(object)`: Sérialise n'importe quel objet Tamgu en tant que document XML et renvoie la chaîne correspondante. Le document est également nettoyé dans le processus.
10. `xmlstring()`: Renvoie un document XML sous forme de chaîne.
11. `xpath(string myxpath)`: Évalue un XPath et renvoie un vecteur de nœuds XML.

## Fonction associée

La fonction associée doit avoir la signature suivante :

```lua
function xmlnode(xml n, object)
```

Elle doit être déclarée de la manière suivante :

```lua
use("xml");
xmldoc mydoc(obj) with xmlnode;
```