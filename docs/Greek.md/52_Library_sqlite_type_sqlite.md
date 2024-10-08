# Βιβλιοθήκη Tamgu SQLite

Το Tamgu παρέχει μια απλή βιβλιοθήκη για την χειρισμό των βάσεων δεδομένων SQLite. Το SQLite είναι ένα δημοφιλές σύστημα βάσης δεδομένων που χρησιμοποιεί απλά αρχεία για την εκτέλεση εντολών SQL. Εάν χρειάζεστε περισσότερες πληροφορίες για το SQLite, μπορείτε να βρείτε πολλούς πόρους στο διαδίκτυο.

Για να χρησιμοποιήσετε τη βιβλιοθήκη SQLite στο Tamgu, πρέπει να την εισάγετε χρησιμοποιώντας την παρακάτω εντολή:

```python
use("sqlite")
```

## Μέθοδοι

Η βιβλιοθήκη SQLite παρέχει τις εξής μεθόδους:

1. `begin()`: Εισέρχεται στη λειτουργία δέσμευσης με την προεπιλεγμένη λειτουργία DEFERRED.
2. `begin(string mode)`: Εισέρχεται στη λειτουργία δέσμευσης με την καθορισμένη λειτουργία (DEFERRED, EXCLUSIVE, IMMEDIATE).
3. `close()`: Κλείνει τη βάση δεδομένων.
4. `commit()`: Εκτελεί τις εντολές SQL. Πρέπει να χρησιμοποιείται για να ολοκληρωθεί μια σειρά εντολών που ξεκίνησαν με την `begin()`.
5. `create(x1, x2, x3)`: Δημιουργεί μια πίνακα στη βάση δεδομένων με τα καθορισμένα ορίσματα.
6. `execute(string sqlcommand)`: Εκτελεί μια εντολή SQL χωρίς callback.
7. `insert(table, column, value, ...)`: Εισάγει μια γραμμή σε έναν πίνακα.
8. `open(string pathname)`: Ανοίγει μια βάση δεδομένων.
9. `run(string sqlcommand)`: Εκτελεί μια εντολή SQL με ένα callback για την αποθήκευση των αποτελεσμάτων. Εάν η μεταβλητή εισόδου είναι ένα διάνυσμα, όλες οι πιθανές τιμές θα αποθηκευτούν σε αυτό. Εάν η μεταβλητή εισόδου είναι ένας επαναληπτής, είναι δυνατή η επανάληψη των αποτελεσμάτων της εντολής SQL. Κάθε αποτέλεσμα είναι ένας χάρτης όπου κάθε κλειδί είναι το όνομα της στήλης.

## Παράδειγμα

Παρακάτω παρουσιάζεται ένα παράδειγμα χρήσης της βιβλιοθήκης SQLite:

```python
// Εισαγωγή της βιβλιοθήκης SQLite
use("sqlite")

// Δήλωση μιας νέας μεταβλητής SQLite
sqlite mydb;

// Άνοιγμα μιας βάσης δεδομένων. Εάν δεν υπάρχει, θα δημιουργηθεί
mydb.open('test.db');

try {
    // Δημιουργία ενός νέου πίνακα στην τρέχουσα βάση δεδομένων
    mydb.create("table1", "nom TEXT PRIMARY KEY", "age INTEGER");
    println("Ο πίνακας table1 δημιουργήθηκε");
} catch () {
    // Η βάση δεδομένων υπάρχει ήδη
    println("Ήδη δημιουργημένος");
}

int i;
string nm;

// Εισαγωγή τιμών στη βάση δεδομένων χρησιμοποιώντας τη λειτουργία δέσμευσης (πιο γρήγορη)
mydb.begin();

// Εισαγωγή 5000 στοιχείων
for (i = 0; i < 5000; i += 1) {
    nm = "tiia_" + i;
    try {
        // Εισαγωγή δύο τιμών στον πίνακα table1, μία για το 'nom' και μία για το 'age'
        // Παρατηρήστε την εναλλαγή μεταξύ των ονομάτων των στηλών και των τιμών
        mydb.insert("table1", "nom", nm, "age", i);
        println(i);
    } catch () {
        println("Ήδη εισαγμένο");
    }
}

// Ολοκλήρωση των εντολών
mydb.commit();

// Επανάληψη των τιμών για μια δεδομένη εντολή SQL
iterator it = mydb.run("select * from table1 where age > 10;");
for (it.begin(); it.end() == false; it.next()) {
    println("Τιμή: ", it.value());
}

// Εναλλακτικά, θα μπορούσαμε να έχουμε λάβει το ίδιο αποτέλεσμα με:
// vector v = mydb.execute("select * from table1 where age > 10;");
// Ωστόσο, υπάρχει κίνδυνος υπερχείλισης του διανύσματός μας, οπότε δεν συνιστάται.

// Κλείσιμο της βάσης δεδομένων
mydb.close();
```