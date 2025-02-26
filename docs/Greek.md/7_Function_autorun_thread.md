# Συνάρτηση, Αυτόματη εκτέλεση, Νήμα

Στο Tamgu, μια συνάρτηση δηλώνεται χρησιμοποιώντας τη λέξη-κλειδί `function`, ακολουθούμενη από το όνομά της και τις παραμέτρους της. Μπορεί να επιστραφεί μια τιμή χρησιμοποιώντας τη λέξη-κλειδί `return`. Οι παράμετροι αποστέλλονται πάντα ως τιμές, εκτός αν ο τύπος είναι `self`. Σημειώνεται ότι μια συνάρτηση δεν παρέχει καμία πληροφορία τύπου για την τιμή που επιστρέφει.

## Επιβολή τύπου επιστροφής

Μια συνάρτηση μπορεί να καθορίσει έναν τύπο επιστροφής χρησιμοποιώντας το σύμβολο `::` μετά τη λίστα των ορισμάτων. Για παράδειγμα:

```
function toto(int i) :: int {
    i += 10;
    return i;
}
```

Σε αυτό το παράδειγμα, η συνάρτηση `toto` δηλώνεται να επιστρέφει έναν ακέραιο αριθμό.

## Αυτόματη εκτέλεση

Μια συνάρτηση `autorun` εκτελείται αυτόματα μετά τη δήλωσή της. Οι συναρτήσεις `autorun` εκτελούνται μόνο στο κύριο αρχείο. Αν έχετε συναρτήσεις `autorun` σε ένα αρχείο που καλείται από ένα άλλο αρχείο, αυτές οι συναρτήσεις δεν θα εκτελεστούν.

Παράδειγμα:

```
autorun waitonred() {
    println("Φόρτωση:", _paths[1]);
}
```

## Νήμα

Όταν ξεκινάει μια συνάρτηση `thread`, εκτελείται σε ένα ανεξάρτητο νήμα του συστήματος.

Παράδειγμα:

```
thread toto(int i) {
    i += 10;
    return i;
}

int j = toto(10);
print("J=" + j + "\n");
```

Έξοδος:

```
J=20
```

### Προστατευμένο νήμα

Ένα προστατευμένο νήμα αποτρέπει δύο νήματα από την πρόσβαση στις ίδιες γραμμές κώδικα ταυτόχρονα. Ένα προστατευμένο νήμα τοποθετεί ένα κλείδωμα στην αρχή της εκτέλεσής του, το οποίο απελευθερώνεται μόλις η συνάρτηση εκτελεστεί. Αυτό εξασφαλίζει ότι διάφορες κλήσεις μιας προστατευμένης συνάρτησης θα γίνουν με τη σειρά και όχι ταυτόχρονα. Το προστατευμένο νήμα πρέπει να χρησιμοποιείται για κώδικα που δεν είναι επαναδρομήσιμος.

Παράδειγμα:

```
// Υλοποιούμε ένα συγχρονισμένο νήμα
protected thread launch(string n, int m) {
    int i;
    println(n);
    // Εμφανίζουμε όλες τις τιμές μας
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // Ξεκινάμε το νήμα μας
    launch("Πρώτο", 2);
    launch("Δεύτερο", 4);
    println("Τέλος");
}

principal();
```

Έξοδος:

```
Τέλος
Πρώτο
0 1
Δεύτερο
0 1 2
```

### Αποκλειστικό νήμα

Ένα αποκλειστικό νήμα είναι παρόμοιο με ένα προστατευμένο νήμα, αλλά με μία διαφορά. Στην περίπτωση ενός προστατευμένου νήματος, η προστασία είναι στο επίπεδο της μεθόδου, ενώ σε ένα αποκλειστικό νήμα είναι στο επίπεδο του αντικειμένου πλαισίου. Αυτό σημαίνει ότι τα αποκλειστικά νήματα επιτρέπουν μόνο σε ένα νήμα να έχει πρόσβαση στο αντικείμενο πλαίσιο την κάθε στιγμή.

Παράδειγμα:

```
// Αυτό το πλαίσιο εκθέτει δύο μεθόδους
frame disp {
    // Αποκλειστικό
    exclusive thread edisplay(string s) {
        println("Αποκλειστικό:", s);
    }
    // Προστατευμένο
    protected thread pdisplay(string s) {
        println("Προστατευμένο:", s);
    }
}

// Υλοποιούμε επίσης ένα πλαίσιο εργασίας
frame task {
    string s;
    // Με ένα συγκεκριμένο "disp" παράδειγμα
    disp cx;

    function _initial(string x) {
        s = x;
    }

    // Καλούμε το τοπικό παράδειγμα μας με προστασία
    function pdisplay() {
        cx.pdisplay(s);
    }

    // Καλούμε το τοπικό παράδειγμα μας με αποκλειστικότητα
    function edisplay() {
        cx.edisplay(s);
    }

    // Καλούμε το γενικό παράδειγμα με αποκλειστικότητα
    function display(disp c) {
        c.edisplay(s);
    }
}

// Η κοινή εκδοχή
disp c;
vector v;
int i;
string s = "T";

for (i = 0; i < 100; i++) {
    s = "T" + i;
    task t(s);
    v.push(t);
}

// Σε αυτή την περίπτωση, η εμφάνιση θα είναι ταξινομημένη αφού το προστατευμένο δεν είναι επαναδρομήσιμο
// Μόνο ένα pdisplay μπορεί να εκτελεστεί ταυτόχρονα
for (i = 0; i < 100; i++)
    v[i].pdisplay();

// Σε αυτή την περίπτωση, η εμφάνιση θα είναι μια συνδυασμένη εκτέλεση όλων των edisplay που εκτελούνται παράλληλα
// Αφού η αποκλειστικότητα προστατεύει μόνο τις μεθόδους εντός ενός παραδείγματος...
for (i = 0; i < 100; i++)
    v[i].edisplay();

// Σε αυτή την τελευταία περίπτωση, έχουμε ένα μόνο κοινό αντικείμενο "disp" που κοινοποιείται από όλα τα "task"
// Η εμφάνιση θα είναι και πάλι ταξινομημένη όπως και με το προστατευμένο, αφού αυτή τη φορά βρισκόμαστε στο ίδιο
// "c disp" παράδειγμα.
for (i = 0; i < 100; i++)
    v[i].display(c);
```

### Ένωση και Αναμονή για Ένωση

Ένα νήμα μπορεί να δηλωθεί ως ένωση, πράγμα που σημαίνει ότι το κύριο νήμα θα περιμένει μέχρι να ολοκληρωθούν όλα τα νήματα που ξεκίνησαν πριν ολοκληρωθεί ο ίδιος ο κώδικας του. Μπορείτε να χρησιμοποιήσετε τη συνάρτηση `waitonjoined()` για να περιμένετε να ολοκληρωθούν αυτά τα νήματα. Μπορείτε να χρησιμοποιήσετε όσες `waitonjoined()` χρειάζεστε σε διάφορα νήματα. Η `waitonjoined()` περιμένει μόνο για τα ενωμένα νήματα που ξεκίνησαν εντός ενός δεδομένου νήματος.

### Ατομικές τιμές

Οι περισσότερες δομές δεδομένων (χάρτες, διανύσματα, συμβολοσειρές κλπ.) προστατεύονται από κλειδώματα για την παράλληλη πρόσβαση. Ωστόσο, αυτά τα κλειδώματα μπορεί να είναι δαπανηρά σε ό,τι αφορά τον χρόνο και τον χώρο. Ο Tamgu παρέχει δομές δεδομένων χωρίς κλειδώματα (ατομικούς τύπους) όπως `a_int`, `a_string` ή `a_mapii`. Αυτές οι δομές δεδομένων επιτρέπουν μια πιο αποδοτική χρήση της μηχανής, καθώς τα νήματα που έχουν πρόσβαση σε αυτούς τους τύπους δεν τίθενται σε αναμονή. Ωστόσο, η υλοποίησή τους τους καθιστά πιο αργούς από άλλους τύπους σε ένα μη-πολυνηματικό περιβάλλον. Είναι χρήσιμοι μόνο όταν κοινοποιούνται μεταξύ νημάτων.

### Τελεστής Ροής '<<<'

Όταν ξεκινάτε ένα νήμα, το αποτέλεσμα του νήματος δεν μπορεί να αποθηκευτεί απευθείας σε μια μεταβλητή χρησιμοποιώντας τον τελεστή `=`, αφού ένα νήμα ζει τη δική του ζωή χωρίς καμία σύνδεση με τον κώδικα που το καλεί. Ο Tamgu παρέχει έναν ειδικό τελεστή για αυτήν την εργασία: το `<<<`, επίσης γνωστό ως τελεστής ροής. Μια ροή είναι μια μεταβλητή που συνδέεται με το νήμα με τέτοιο τρόπο ώστε οι τιμές που επιστρέφονται από το νήμα να μπορούν να αποθηκευτούν σε αυτήν τη μεταβλητή. Φυσικά, η μεταβλητή πρέπει να υπάρχει ανεξάρτητα από το νήμα.

Παράδειγμα:

```cpp
// Δημιουργούμε ένα νήμα ως ένωση νήματος, ώστε να μπορούμε να χρησιμοποιήσουμε την waitonjoined.
// Αυτό το νήμα επιστρέφει απλώς 2 * x
joined thread Test(int i) {
    return (i * 2);
}

// Η συνάρτηση μας εκκίνησης, η οποία θα ξεκινήσει 10 νήματα
function launch() {
    // Πρώτα δηλώνουμε τη μεταβλητή αποθήκευσης του χάρτη μέσα σε αυτήν τη συνάρτηση
    treemapi m;
    int i = 0;

    // Στη συνέχεια ξεκινάμε 10 νήματα και αποθηκεύουμε το αποτέλεσμα κάθε ενός στο m σε μια συγκεκριμένη θέση
    for (i in <0,10,1>)
        m[i] <<< Test(i);

    // Περιμένουμε να ολοκληρωθούν όλα τα νήματα
    waitonjoined();

    // Εμφανίζουμε την τελική τιμή
    println(m); // {0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```

## Πολλαπλοί Ορισμοί

Ο Tamgu επιτρέπει τους πολλαπλούς ορισμούς συναρτήσεων που μοιράζονται το ίδιο όνομα αλλά διαφέρουν στον ορισμό των παραμέτρων τους. Για παράδειγμα, μπορείτε να υλοποιήσετε μια `display(string s)` και μια `display(int s)`. Σε αυτήν την περίπτωση, το σύστημα θα επιλέξει την πιο κατάλληλη συνάρτηση βάσει της λίστας των ορισμάτων της κλήσης της συνάρτησης.

Παράδειγμα:

```cpp
function testmultipledeclaration(string s, string v) {
    println("Συμβολοσειρά:", s, v);
}

function testmultipledeclaration(int s, int v) {
    println("Ακέραιος:", s, v);
}

// Δηλώνουμε τις μεταβλητές μας
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// Σε αυτήν την περίπτωση, το σύστημα θα επιλέξει τη σωστή συνάρτηση ανάλογα με την λίστα των ορισμάτων της κλήσης
testmultipledeclaration(s1, s2); // Η υλοποίηση της συμβολοσειράς
testmultipledeclaration(i, j); // Η υλοποίηση του ακεραίου
```

## Προκαθορισμένα Ορίσματα

Ο Tamgu παρέχει ένα μηχανισμό για τη δήλωση προκαθορισμένων ορισμάτων σε μια συνάρτηση. Μπορείτε να καθορίσετε μια τιμή για ένα παράμετρο που μπορεί να παραλειφθεί από την κλήση.

Παράδειγμα:

```cpp
function acall(int x, int y = 12, int z = 30, int u = 43) {
    println(x + y + z + u);
}

acall(10, 5); // Το αποτέλεσμα είναι: 88 = 10 + 5 + 30 + 43
```

Σημείωση: Μόνο οι τελευταίες παράμετροι σε μια λίστα ορισμάτων μπορούν να είναι προαιρετικές.

## Ειδικές Σημαίες: Ιδιωτικό & Αυστηρό

Οι συναρτήσεις μπορούν επίσης να δηλωθούν με δύο ειδικές σημαίες: `private` και `strict`.

### Ιδιωτικό

Όταν μια συνάρτηση δηλώνεται ως `private`, δεν μπορεί να προσπελαστεί από έξω του τρέχοντος αρχείου Tamgu. Αν ένα πρόγραμμα Tamgu φορτωθεί από ένα άλλο πρόγραμμα Tamgu, οι ιδιωτικές συναρτήσεις δεν είναι προσπελάσιμες από τον φορτωτή.

Παράδειγμα:

```cpp
// Αυτή η συνάρτηση είναι αόρατη από εξωτερικούς φορτωτές
private function toto(int i) {
    i += 10;
    return i;
}
```

### Αυστηρό

Από προεπιλογή, όταν μια συνάρτηση δηλώνεται στο Tamgu, η γλώσσα προσπαθεί να μετατρέψει κάθε όρισμα από την κλήση της συνάρτησης στις παραμέτρους που αναμένονται από την υλοποίηση της συνάρτησης. Ωστόσο, σε ορισμένες περιπτώσεις απαιτείται αυστηρότερος έλεγχος παραμέτρων. Η σημαία `strict` βοηθά να λυθεί αυτό το πρόβλημα απαιτώντας αυστηρό έλεγχο παραμέτρων για μια συνάρτηση.

Παράδειγμα:

```cpp
strict function teststrictdeclaration(int s, int v) {
    println("Ακέραιος:", s, v);
}

// Δηλώνουμε τις μεταβλητές μας
string s1 = "s1";
string s2 = "s2";

// Σε αυτήν την περίπτωση, το σύστημα θα αποτύχει να βρει την σωστή συνάρτηση για αυτά τα ορίσματα
// και θα επιστρέψει ένα μήνυμα σφάλματος
teststrictdeclaration(s1, s2); // Δεν υπάρχει υλοποίηση συμβολοσειράς
```