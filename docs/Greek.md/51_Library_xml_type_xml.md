# Βιβλιοθήκη xml: τύπος xml

Ο τύπος `xml` παρέχει μεθόδους για την χειρισμό των κόμβων XML.

**Σημαντικό**  
Αυτός ο τύπος υλοποιείται ως ένας αντικαταστάτης του τύπου `xmlNodePtr` από τη βιβλιοθήκη libxml2 (δείτε [http://xmlsoft.org/](http://xmlsoft.org/)), επομένως χρειάζεται η μέθοδος `new` για να αποκτηθεί ένα νέο αντικείμενο για την τρέχουσα μεταβλητή.

Αυτός ο τύπος προσπελαύνεται μέσω της βιβλιοθήκης xml: `use("xml")`.

## Μέθοδοι

1. `child()`: επιστρέφει τον πρώτο υποκόμβο κάτω από τον τρέχοντα κόμβο.
2. `child(xml)`: Προσθέτει έναν κόμβο XML ως υποκόμβο.
3. `content()`: Επιστρέφει το περιεχόμενο ενός κόμβου.
4. `content(string n)`: Αλλάζει το περιεχόμενο ενός κόμβου.
5. `delete()`: διαγράφει τον τρέχοντα εσωτερικό κόμβο.
6. `line()`: επιστρέφει τον αριθμό γραμμής του τρέχοντος κόμβου.
7. `id()`: επιστρέφει το αναγνωριστικό του τρέχοντος κόμβου (μόνο με κλήσεις συναρτήσεων).
8. `name()`: επιστρέφει το όνομα του κόμβου XML.
9. `name(string n)`: Αλλάζει το όνομα του κόμβου XML.
10. `namespace()`: Επιστρέφει το namespace του τρέχοντος κόμβου ως ένα διάνυσμα.
11. `new(string n)`: Δημιουργεί έναν νέο εσωτερικό κόμβο.
12. `next()`: επιστρέφει τον επόμενο κόμβο XML.
13. `next(xml)`: Προσθέτει έναν κόμβο XML μετά τον τρέχοντα κόμβο.
14. `parent()`: επιστρέφει τον γονέα του τρέχοντος κόμβου.
15. `previous()`: επιστρέφει τον προηγούμενο κόμβο XML.
16. `previous(xml)`: Προσθέτει έναν κόμβο XML πριν από τον τρέχοντα κόμβο.
17. `properties()`: Επιστρέφει τις ιδιότητες του κόμβου XML.
18. `properties(map props)`: Οι ιδιότητες αποθηκεύονται σε έναν χάρτη ως γνώρισμα/τιμή.
19. `root()`: επιστρέφει τον ριζικό κόμβο του δέντρου XML.
20. `xmlstring()`: επιστρέφει το υποδέντρο XML ως συμβολοσειρά.
21. `xmltype()`: επιστρέφει τον τύπο του κόμβου XML ως συμβολοσειρά.

**Παράδειγμα**

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