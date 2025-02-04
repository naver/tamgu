# Βιβλιοθήκη wapiti

Το libwapiti είναι μια ενθυλάκωση της βιβλιοθήκης wapiti, η οποία είναι διαθέσιμη στη διεύθυνση [http://wapiti.limsi.fr](http://wapiti.limsi.fr).

Πνευματικά δικαιώματα (c) 2009-2013 CNRS. Με επιφύλαξη παντός δικαιώματος.

Το Wapiti παρέχει μια αποτελεσματική υλοποίηση της μεθόδου CRF για επισήμανση και εξαγωγή οντοτήτων. Εάν χρειάζεστε περισσότερες πληροφορίες για το σύστημα, ανατρέξτε στο εγχειρίδιο στη διεύθυνση [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html).

## Χρήση

Για να χρησιμοποιήσετε τη βιβλιοθήκη, πρέπει να φορτώσετε το αρθρωτό wapiti:

```python
use('wapiti')
```

Η βιβλιοθήκη εκθέτει τις παρακάτω μεθόδους:

### Μέθοδοι

1. `loadmodel(string crfmodel)`: Φορτώνει ένα μοντέλο CRF.
2. `options(svector options)`: Ορίζει επιλογές. Δείτε παρακάτω τις διαθέσιμες επιλογές. Οι επιλογές πρέπει να τοποθετηθούν στο svector όπως χρησιμοποιούνται στη γραμμή εντολών του wapiti.
3. `train()`: Ξεκινά την εκπαίδευση. Απαιτεί τις επιλογές να έχουν οριστεί εκ των προτέρων.
4. `label(vector words)`: Ξεκινά την επισήμανση για ένα διάνυσμα των τεκμηρίων. Επιστρέφει ένα διάνυσμα ετικετών για κάθε τεκμήριο.
5. `lasterror()`: Επιστρέφει το τελευταίο σφάλμα που προέκυψε.

### Επιλογές

Το Wapiti παρέχει ορισμένες επιλογές για να αντιμετωπίσει όλες τις δυνατότητες που ενσωματώνονται στο σύστημα. Παρακάτω ακολουθεί μια λίστα αυτών των επιλογών, οι οποίες πρέπει να παρέχονται ως svector, ακριβώς όπως αυτές θα παρέχονταν με τη γραμμή εντολών του wapiti.

#### Λειτουργία εκπαίδευσης:

- `train [επιλογές] [δεδομένα εισόδου] [αρχείο μοντέλου]`
  - `--me`: Εξαναγκάζει τη λειτουργία maxent
  - `--type STRING`: Τύπος μοντέλου για εκπαίδευση
  - `--algo STRING`: Αλγόριθμος εκπαίδευσης που θα χρησιμοποιηθεί
  - `--pattern FILE`: Πρότυπα για την εξαγωγή χαρακτηριστικών
  - `--model FILE`: Αρχείο μοντέλου για προφόρτωση
  - `--devel FILE`: Σύνολο δεδομένων ανάπτυξης
  - `--rstate FILE`: Κατάσταση βελτιστοποιητή για ανάκτηση
  - `--sstate FILE`: Κατάσταση βελτιστοποιητή για αποθήκευση
  - `--compact`: Συμπιεσμένο μοντέλο μετά την εκπαίδευση
  - `--nthread INT`: Αριθμός νημάτων εργασίας
  - `--jobsize INT`: Μέγεθος εργασίας για τα νήματα εργασίας
  - `--sparse`: Ενεργοποίηση αραιής προώθησης/πίσω προώθησης
  - `--maxiter INT`: Μέγιστος αριθμός επαναλήψεων
  - `--rho1 FLOAT`: Παράμετρος ποινής L1
  - `--rho2 FLOAT`: Παράμετρος ποινής L2
  - `--objwin INT`: Μέγεθος παραθύρου σύγκλισης
  - `--stopwin INT`: Μέγεθος παραθύρου διακοπής
  - `--stopeps FLOAT`: Τιμή επανειλημμένης σύγκλισης
  - `--clip`: Διαίρεση γραμμής κλίσης (l-bfgs)
  - `--histsz INT`: Μέγεθος ιστορικού (l-bfgs)
  - `--maxls INT`: Μέγιστος αριθμός αναζήτησης γραμμής (l-bfgs)
  - `--eta0 FLOAT`: Ρυθμός μάθησης (sgd-l1)
  - `--alpha FLOAT`: Παράμετρος εκθετικής υποβάθμισης (sgd-l1)
  - `--kappa FLOAT`: Παράμετρος σταθερότητας (bcd)
  - `--stpmin FLOAT`: Ελάχιστο μέγεθος βήματος (rprop)
  - `--stpmax FLOAT`: Μέγιστο μέγεθος βήματος (rprop)
  - `--stpinc FLOAT`: Παράγοντας αύξησης βήματος (rprop)
  - `--stpdec FLOAT`: Παράγοντας μείωσης βήματος (rprop)
  - `--cutoff`: Εναλλακτική προβολή (rprop)

#### Λειτουργία επισήμανσης:

- `label [επιλογές] [δεδομένα εισόδου] [δεδομένα εξόδου]`
  - `--me`: Εξαναγκάζει τη λειτουργία maxent
  - `--model FILE`: Αρχείο μοντέλου για φόρτωση
  - `--label`: Εξαγωγή μόνο ετικετών
  - `--check`: Τα εισερχόμενα έχουν ήδη επισημανθεί
  - `--score`: Προσθήκη σκορ στην έξοδο
  - `--post`: Επισήμανση χρησιμοποιώντας τις αφίσεις
  - `--nbest INT`: Έξοδος n-best λίστας
  - `--force`: Χρήση αναγκαστικής αποκωδικοποίησης

## Εκπαίδευση

Για να εκπαιδεύσετε ένα CRF, χρειάζεστε ένα αρχείο κειμένου με επισημάνσεις, όπου κάθε γραμμή είναι ένα τεκμήριο με τις ετικέτες του χωρισμένες από ένα tab.

Παράδειγμα:

```
UNITED STATES  NOUN  LOCATION_b
SECURITIES  NOUN  ORGANISATION_i
AND  CONJ  ORGANISATION_i
EXCHANGE  NOUN  ORGANISATION_i
COMMISSION  NOUN  ORGANISATION_i
Washington  NOUN  ORGANISATION_i
, PUNCT  ORGANISATION_i
D.C. NOUN  LOCATION_b
20549  DIG NUMBER_b
FORM  VERB  null
N NOUN  null
```

Σε αυτό το παράδειγμα, κάθε γραμμή αναπαριστά ένα τεκμήριο που συσχετίζεται με δύο διαφορετικές ετικέτες. Η ετικέτα "null" σε αυτό το παράδειγμα είναι μια απλή συμβολοσειρά που δεν έχει μια συγκεκριμένη ερμηνεία εκτός από αυτό το συγκεκριμένο παράδειγμα.

Χρειάζεστε επίσης ένα αρχείο "pattern", το οποίο πρέπει να υλοποιηθεί σύμφωνα με το εγχειρίδιο του CRF++ ή όπως περιγράφεται στο [εγχειρίδιο του Wapiti](http://wapiti.limsi.fr/manual.html).

Παράδειγμα αρχείου προτύπου:

```
# Unigram
U00:%x[-2,0]
U01:%x[-1,0]
U02:%x[0,0]
U03:%x[1,0]
U04:%x[2,0]
U05:%x[-2,1]
U06:%x[-1,1]
U07:%x[0,1]
U08:%x[1,1]
U09:%x[2,1]
U10:%x[-2,0]/%x[0,0]
U11:%x[-1,0]/%x[0,0]
U12:%x[0,0]/%x[1,0]
U13:%x[0,0]/%x[2,0]
U14:%x[-2,1]/%x[0,1]
U15:%x[-1,1]/%x[0,1]
U16:%x[0,1]/%x[1,1]
U17:%x[0,1]/%x[2,1]
# Bigram
B
```

## Πρόγραμμα

Παρακάτω παρουσιάζεται ένα μικρό πρόγραμμα που χρησιμοποιεί τη βιβλιοθήκη για να εκπαιδεύσει ένα μοντέλο και να επισημάνει οντότητες.

```python
use('wapiti')

wapiti tst

# Φόρτωση του μοντέλου
tst.loadmodel("model")

# Επισήμανση ενός διανύσματος τεκμηρίων
words = ['Growth NOUN', '& CONJ', 'Income NOUN', 'Fund NOUN', '( PUNCT', 'Exact ADJ', 'name NOUN', 'of PREP', 'registrant NOUN', 'as PREP', 'specified ADJ', 'in PREP', 'charter NOUN']
res = tst.label(words)

# Εκτύπωση της λίστας ετικετών
println(res)
```

Το αποτέλεσμα είναι: `['ORGANISATION_b', 'ORGANISATION_i', 'ORGANISATION_i', 'ORGANISATION_i', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null']`