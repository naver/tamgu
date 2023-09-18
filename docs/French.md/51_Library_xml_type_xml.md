# Bibliothèque xml: type xml

Le type `xml` fournit des méthodes pour manipuler les nœuds XML.

**Important**  
Ce type est implémenté en tant que substitut du type `xmlNodePtr` de la bibliothèque libxml2 (voir [http://xmlsoft.org/](http://xmlsoft.org/)), d'où la méthode `new` qui est nécessaire pour obtenir un nouvel objet pour la variable actuelle.

Ce type est accessible via la bibliothèque xml: `use("xml")`.

## Méthodes

1. `child()`: retourne le premier nœud enfant sous le nœud actuel.
2. `child(xml)`: ajoute un nœud XML en tant qu'enfant.
3. `content()`: retourne le contenu d'un nœud.
4. `content(string n)`: modifie le contenu d'un nœud.
5. `delete()`: supprime le nœud interne actuel.
6. `line()`: retourne le numéro de ligne du nœud actuel.
7. `id()`: retourne l'identifiant du nœud actuel (uniquement avec les fonctions d'appel).
8. `name()`: retourne le nom du nœud XML.
9. `name(string n)`: modifie le nom du nœud XML.
10. `namespace()`: retourne l'espace de noms du nœud actuel sous forme de vecteur.
11. `new(string n)`: crée un nouveau nœud interne.
12. `next()`: retourne le prochain nœud XML.
13. `next(xml)`: ajoute un nœud XML après le nœud actuel.
14. `parent()`: retourne le nœud parent au-dessus du nœud actuel.
15. `previous()`: retourne le nœud XML précédent.
16. `previous(xml)`: ajoute un nœud XML avant le nœud actuel.
17. `properties()`: retourne les propriétés du nœud XML.
18. `properties(map props)`: les propriétés sont stockées dans une map en tant qu'attribut/valeur.
19. `root()`: retourne le nœud racine de l'arbre XML.
20. `xmlstring()`: retourne le sous-arbre XML sous forme de chaîne de caractères.
21. `xmltype()`: retourne le type du nœud XML sous forme de chaîne de caractères.

**Exemple**

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
n.content("Toto est heureux");
nd.child(n);
nouveau.save("mynewfile.xml");
```