## Ψευδώνυμο

Ένα ψευδώνυμο είναι παρόμοιο με μια συνάρτηση, αλλά με ορισμένες συγκεκριμένες διαφορές:

1. Οι παράμετροι περιγράφονται χωρίς τύπους.
1. Οι ορίσματα σε ένα ψευδώνυμο μπορούν να αλλάξουν όλα.
1. Ένα ψευδώνυμο μπορεί να χρησιμοποιηθεί για να αντικαταστήσει μια πολύπλοκη κλήση δομής.
1. Ένα ψευδώνυμο μπορεί να χρησιμοποιηθεί ως μια απλή συνάρτηση, στην οποία πρέπει να επιστρέφει την τιμή της με την εντολή `return`.

### Μοτίβο

Ένα ψευδώνυμο συνάρτηση υλοποιείται με τον εξής τρόπο:

```
ψευδώνυμο όνομα(α, β, ..., ω) = κώδικας;
```

### Παράδειγμα

```python
ψευδώνυμο αφαίρεση(α) = α.replace("-", "").replace(r"%p", " ").lower();
ψευδώνυμο εμφάνιση(α, β) = {
    println(α, β);
}

α = αφαίρεση(α);
εμφάνιση("Τεστ", α);
```

### Πολύπλοκα Μοτίβα

Το παρακάτω παράδειγμα δείχνει πώς μπορείτε να δημιουργήσετε ένα ψευδώνυμο συνάρτηση που έχει πρόσβαση στο περιεχόμενο ενός πλαισίου:

```python
ψευδώνυμο πάρε(α, ι) = α.πρόσωπα.ονόματα[ι];
```

Αυτό το ψευδώνυμο μπορεί να χρησιμοποιηθεί τόσο για να ανακτήσει ένα στοιχείο όσο και για να το τροποποιήσει. Όταν χρειάζεστε να τροποποιήσετε ένα στοιχείο, πρέπει να χρησιμοποιήσετε τις ακόλουθες αγκύλες: `[*...]` για να παρέχετε τα διάφορα ορίσματα.

#### Παράδειγμα

```python
// Δύο περιγραφές πλαισίου
frame ΛίσταΠροσώπων {
    svector ονόματα;
    function _αρχικοποίηση() {
        ονόματα = ["α".."ε"];
    }
    function συμβολοσειρά() {
        return ονόματα;
    }
}

frame Αντικείμενα {
    ΛίσταΠροσώπων πρόσωπα;
    function συμβολοσειρά() {
        return πρόσωπα;
    }
}

// Ορίζουμε ένα ψευδώνυμο για να έχουμε πρόσβαση στα ονόματα στο ΛίσταΠροσώπων
ψευδώνυμο πάρε(α, ι) = α.πρόσωπα.ονόματα[ι];

// Δηλώνουμε ένα στοιχείο τύπου Αντικείμενα.
Αντικείμενα αντικείμενα;

// Αποκτούμε την τιμή του ονόματος στη θέση 1
println(πάρε(αντικείμενα, 1));

// Τροποποιούμε την τιμή ενός ονόματος στη θέση 1
// Παρατηρήστε τη χρήση των [*αντικείμενα, 1]
πάρε[*αντικείμενα, 1] = "Β";
```