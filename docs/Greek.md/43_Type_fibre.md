# Τύπος Ίνα

Μια ίνα είναι ένα ελαφρύ νήμα εκτέλεσης. Όπως και τα νήματα, οι ίνες μοιράζονται τον ίδιο χώρο διευθύνσεων. Ωστόσο, οι ίνες χρησιμοποιούν συνεργατική πολυεργασία ενώ τα νήματα χρησιμοποιούν προεκτεταμένη πολυεργασία. Τα νήματα συνήθως βασίζονται στον προγραμματιστή νημάτων του πυρήνα για να προεκτείνουν ένα απασχολημένο νήμα και να επαναλάβουν ένα άλλο νήμα, ενώ οι ίνες υποχωρούν για να επιτρέψουν σε μια άλλη ίνα να τρέξει κατά τη διάρκεια της εκτέλεσής τους.

Στην Tamgu, οι ίνες υλοποιούνται πάνω από τις συναρτήσεις Taskell. Αυτές οι συναρτήσεις πρέπει να περιέχουν μια επανάληψη προκειμένου να εκτελεστούν ως ίνες, καθώς δεν υπάρχει πραγματική συνάρτηση υποχώρησης.

Ο τύπος `fibre` αποκαλύπτει τις ακόλουθες μεθόδους:

1. `run()`: Εκτελεί τις ίνες που έχουν καταγραφεί.
2. `block()`: Οι ίνες αποθηκεύονται σε μια αλυσίδα συνδεδεμένων στοιχείων που επαναλαμβάνεται από την αρχή στο τέλος. Όταν μια νέα ίνα προσαρτάται σε αυτήν τη λίστα, γίνεται το νέο τερματικό στοιχείο. Η `block()` χρησιμοποιείται για να καθορίσει το τρέχον τερματικό στοιχείο αυτής της λίστας ως όριο επανάληψης. Νέες ίνες μπορούν ακόμα να προσαρτηθούν, αλλά δεν θα εκτελεστούν μέχρι να κληθεί η `unblock()`. Εάν η `block()` κληθεί ξανά με ένα διαφορετικό τρέχον τερματικό στοιχείο, τότε το προηγούμενο όριο μεταφέρεται σε αυτό το νέο τερματικό στοιχείο.
3. `unblock()`: Απελευθερώνει το όριο επανάληψης.

## Δήλωση μιας Ίνας

Για να δηλώσετε μια ίνα, πρέπει πρώτα να υλοποιήσετε μια συνάρτηση Taskell και στη συνέχεια να αποθηκεύσετε αυτήν τη συνάρτηση Taskell σε μια μεταβλητή ίνας.

```cpp
<Myfunc(a1, a2, ..., an) = ...>
fibre f = MyFunc;
```

Για να καταγράψετε μια νέα ίνα, απλά καλέστε τη με ορισμένες παραμέτρους:

```cpp
f(p1, p2, ..., pn);
```

Εάν η `MyFunc` δεν περιέχει μια επανάληψη, θα εκτελεστεί αυτόματα. Εάν χρειάζεστε να αποθηκεύσετε κάποια έξοδο, μπορείτε να χρησιμοποιήσετε τον τελεστή ροής.

```cpp
vector v;
v.push(ivector());
v[-1] <<< f(200, 210);
```

## Τερματισμός μιας Ίνας: `break` ή Τέλος της Επανάληψης

Υπάρχουν δύο τρόποι για να τερματίσετε μια ίνα: είτε ο επαναλήπτης φτάνει σε ένα τέλος είτε καλείται η εντολή `break`.

```cpp
<Myfunc(x, y) : if (x == 10) break else (x + 1) ...>
```

## Νήματα

Οι ίνες μπορούν να εκτελεστούν μέσα σε ένα νήμα, αλλά μόνο μια ίνα μπορεί να εκτελέσει τη μέθοδο `run()` κάθε φορά σε ένα νήμα. Ωστόσο, τα νήματα μπορούν να καταγράψουν νέες ίνες παράλληλα χωρίς κανένα πρόβλημα.

## Παράδειγμα

```cpp
vector v;
int i;

// Αρχικοποιούμε ένα διάνυσμα ακεραίων για να αποθηκεύσουμε τα αποτελέσματα...
for (i in <3>)
    v.push(ivector());

//-----------------------------------------------------------------------
function compute(int i, int x, int y) {
    return i + x + y;
}

<myfiber(x, y) = compute(i, x, y) | i <- [x..y]>

// Αρχικοποιούμε μια μεταβλητή ίνας με τη myfiber
fibre f = myfiber;

//-----------------------------------------------------------------------
// Η καταγραφή γίνεται από ένα νήμα...
joined thread recording(int i, int x, int y) {
    println("Ίνα:", i);
    v[i] <<< f(x, y);
}

function running() {
    f.run();
}

//-----------------------------------------------------------------------
// Οι ίνες ξεκινούν από ένα νήμα...
int n;

for (i in <0, 60, 20>) {
    recording(n, i, i + 19);
    n++;
}

// Περιμένουμε όλες οι ίνες να καταγράψουν
waitonjoined();

//-----------------------------------------------------------------------
// Τις εκτελούμε...
running();

println(v);
```

**Αποτέλεσμα:**

```
Ίνα: 0
Ίνα: 2
Ίνα: 1
[[19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38],
[79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98],
[139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158]]
```