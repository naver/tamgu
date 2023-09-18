# Bibliothèque Tamgu SQLite

Tamgu fournit une bibliothèque simple pour gérer les bases de données SQLite. SQLite est un système de base de données populaire qui utilise des fichiers simples pour exécuter des commandes SQL. Si vous avez besoin de plus d'informations sur SQLite, vous pouvez trouver de nombreuses ressources sur le web.

Pour utiliser la bibliothèque SQLite dans Tamgu, vous devez l'importer en utilisant la commande suivante:

```python
use("sqlite")
```

## Méthodes

La bibliothèque SQLite fournit les méthodes suivantes:

1. `begin()`: Entre en mode de validation avec le mode DEFERRED.
2. `begin(string mode)`: Entre en mode de validation avec le mode spécifié (DEFERRED, EXCLUSIVE, IMMEDIATE).
3. `close()`: Ferme la base de données.
4. `commit()`: Traite les commandes SQL. Cela doit être utilisé pour terminer une série de commandes initiées avec `begin()`.
5. `create(x1, x2, x3)`: Crée une table dans la base de données avec les arguments spécifiés.
6. `execute(string sqlcommand)`: Exécute une commande SQL sans rappel.
7. `insert(table, column, value, ...)`: Insère une ligne dans une table.
8. `open(string pathname)`: Ouvre une base de données.
9. `run(string sqlcommand)`: Exécute une commande SQL avec un rappel pour stocker les résultats. Si la variable d'entrée est un vecteur, toutes les valeurs possibles y seront stockées. Si la variable d'entrée est un itérateur, il est possible d'itérer sur les résultats de la commande SQL. Chaque résultat est une carte où chaque clé est un nom de colonne.

## Exemple

Voici un exemple d'utilisation de la bibliothèque SQLite:

```python
// Importer la bibliothèque SQLite
use("sqlite")

// Déclarer une nouvelle variable SQLite
sqlite mydb;

// Ouvrir une base de données. Si elle n'existe pas, elle sera créée
mydb.open('test.db');

try {
    // Créer une nouvelle table dans la base de données actuelle
    mydb.create("table1", "nom TEXT PRIMARY KEY", "age INTEGER");
    println("table1 est créée");
} catch () {
    // La base de données existe déjà
    println("Déjà créée");
}

int i;
string nm;

// Insérer des valeurs dans la base de données en utilisant le mode de validation (plus rapide)
mydb.begin();

// Insérer 5000 éléments
for (i = 0; i < 5000; i += 1) {
    nm = "tiia_" + i;
    try {
        // Insérer deux valeurs dans table1, une pour 'nom' et l'autre pour 'age'
        // Remarquez l'alternance entre les noms de colonnes et les valeurs
        mydb.insert("table1", "nom", nm, "age", i);
        println(i);
    } catch () {
        println("Déjà inséré");
    }
}

// Valider les commandes
mydb.commit();

// Itérer sur les valeurs pour une commande SQL donnée
iterator it = mydb.run("select * from table1 where age > 10;");
for (it.begin(); it.end() == false; it.next()) {
    println("Valeur: ", it.value());
}

// Alternativement, nous aurions pu obtenir le même résultat avec:
// vector v = mydb.execute("select * from table1 where age > 10;");
// Cependant, il y a un risque de débordement de notre vecteur, donc ce n'est pas recommandé.

// Fermer la base de données
mydb.close();
```