# pytamgu

Η βιβλιοθήκη `pytamgu` είναι μια διπλή βιβλιοθήκη που μπορεί να χρησιμοποιηθεί τόσο ως βιβλιοθήκη Python όσο και ως βιβλιοθήκη Tamgu. Σας επιτρέπει να εκτελέσετε κώδικα Python από μέσα σε ένα πρόγραμμα Tamgu ή να εκτελέσετε ένα πρόγραμμα Tamgu από μέσα σε κώδικα Python.

## Ως βιβλιοθήκη Tamgu

Όταν χρησιμοποιείται ως βιβλιοθήκη Tamgu, το `pytamgu` αποκαλύπτει ένα νέο τύπο με το όνομα `python`. Οι βασικοί τύποι Tamgu (boolean, integer, long, float, fraction, string, vector containers και map containers) αντιστοιχίζονται αυτόματα στους αντίστοιχους τύπους Python.

Ο τύπος `python` προσφέρει τις εξής μεθόδους:

1. `close()`: Κλείστε την τρέχουσα συνεδρία Python.
2. `execute(string funcname, p1, p2...)`: Εκτελέστε μια συνάρτηση Python με τις παραμέτρους `p1`, `p2` κ.λπ.
3. `import(string python)`: Εισαγάγετε ένα αρχείο Python.
4. `run(string code)`: Εκτελέστε κώδικα Python.
5. `setpath(string path1, string path2, κ.λπ...)`: Προσθέστε συστημικά μονοπάτια στο Python.

Η μέθοδος `setpath` είναι κρίσιμη για τη χρήση της μεθόδου `import`, η οποία λειτουργεί με τον ίδιο τρόπο με τη λέξη-κλειδί `import` στην Python. Εάν θέλετε να εισάγετε ένα πρόγραμμα Python σε μια συγκεκριμένη τοποθεσία που δεν έχει αναφερθεί μέσω του `PYTHONPATH`, πρέπει να το προσθέσετε με την `setpath` πρώτα.

### Παράδειγμα

Πρώτα, υλοποιούμε ένα μικρό πρόγραμμα Python με το όνομα `testpy.py`:

```python
val = "εδώ"
# Οι μεταβλητές εισόδου μεταφράζονται αυτόματα από το Tamgu σε μεταβλητές Python
def Iteste(s, v):
    v.append(s)
    v.append(val)
    return v
```

Στη συνέχεια, υλοποιούμε το δικό μας πρόγραμμα Tamgu, το οποίο θα καλέσει αυτό το αρχείο (υποθέτοντας ότι είναι στον ίδιο κατάλογο με το πρόγραμμα Tamgu μας):

```tamgu
// Χρειαζόμαστε το Pytamgu για το καλό μας
use("pytamgu");

// Χρειαζόμαστε μια μεταβλητή για τη χειρισμό της Python
python p;

// Υποθέτουμε ότι το πρόγραμμα Python μας είναι στον ίδιο κατάλογο με το πρόγραμμα Tamgu μας
p.setpath(_paths[1]);

// Στη συνέχεια εισάγουμε το πρόγραμμά μας
p.import("testpy");

vector v;
string s = "kkk";

// Εκτελούμε την Python συνάρτηση Itest, η οποία παίρνει ως είσοδο ένα string και ένα vector,
// τα οποία μετατρέπονται αυτόματα σε αντικείμενα Python κατά τη διάρκεια της εκτέλεσης.
// Το αποτέλεσμα μετατρέπεται αυτόματα πίσω σε έναν Tamgu vector (από τον Python vector)
vector vv = p.execute("Itest", s, v);

println(vv); // Το αποτέλεσμα είναι: ['kkk', 'εδώ']

p.close(); // Κλείνουμε τη συνεδρία
```

## Ως βιβλιοθήκη Python

Όταν χρησιμοποιείται ως βιβλιοθήκη Python, μπορείτε να εισάγετε τη βιβλιοθήκη `pytamgu`, η οποία προσφέρει δύο μεθόδους.

1. `load(file, arguments, mapping)`
   - `file` είναι το όνομα του αρχείου Tamgu που θέλετε να φορτώσετε.
   - `arguments` είναι μια συμβολοσειρά που παρέχει τα ορίσματα για το αρχείο Tamgu, χωρισμένα με ένα κενό.
   - Εάν η `mapping` έχει τιμή 1, δημιουργείται μια μέθοδος Python για κάθε συνάρτηση στο αρχείο Tamgu με το ίδιο όνομα με τις συναρτήσεις Tamgu.
   - Αυτή η μέθοδος επιστρέφει ένα χειριστή.

2. `execute(handle, function_name, [arg1, arg2, ..., argn])`
   - `handle` είναι ο χειριστής του αρχείου που περιέχει τη συνάρτηση που θέλουμε να εκτελέσουμε.
   - `function_name` είναι το όνομα της συνάρτησης στο αρχείο Tamgu.
   - `[arg1, arg2, ..., argn]` είναι η λίστα των ορισμάτων που θα παρέχονται στο πρόγραμμα Tamgu ως ένα διάνυσμα συμβολοσειρών.

Εάν χρησιμοποιήσετε την επιλογή mapping, η μέθοδος `execute` είναι προαιρετική. Οι τιμές που επιστρέφονται από το πρόγραμμα Tamgu μεταφράζονται αυτόματα σε αντικείμενα Python, και το ίδιο ισχύει για τα ορίσματα που μεταδίδονται στο πρόγραμμα Tamgu.

**Σημείωση:** Το Tamgu επιστρέφει πάντα Python Unicode strings.

### Παράδειγμα

Πρόγραμμα Tamgu:

```tamgu
vector v = [1..10];

function rappel(string s, int j) {
    j += 10;
    v.push(j);
    v.push(s);
    return v;
}
```

Πρόγραμμα Python:

```python
import pytamgu

h0 = pytamgu.load("rappel.kif", "", 1)
# Χρησιμοποιούμε την αντιστοίχιση σε μια συνάρτηση Python
v = rappel("Τεστ", 10)
for i in v:
    print(i)
# Αυτό είναι ισοδύναμο με
v = pytamgu.execute(h0, "rappel", ["Τεστ", 10])
```