# Βιβλιοθήκη LINEAR: Γραμμικός Τύπος

Το Tamgu παρέχει μια ενθυλάκωση της βιβλιοθήκης liblinear, η οποία χρησιμοποιείται για την υλοποίηση ταξινομητών. Για περισσότερες πληροφορίες σχετικά με αυτήν τη βιβλιοθήκη, επισκεφθείτε τη διεύθυνση [http://www.csie.ntu.edu.tw/~cjlin/liblinear/](http://www.csie.ntu.edu.tw/~cjlin/liblinear/).

## Μέθοδοι

Η βιβλιοθήκη προσφέρει τις ακόλουθες μεθόδους:

1. `cleandata()`: Καθαρίζει τα εσωτερικά δεδομένα.
2. `crossvalidation()`: Επανεκκινεί τη διασταυρούμενη επικύρωση με νέες παραμέτρους. Το αποτέλεσμα είναι ένα fmap.
3. `loadmodel(string filename)`: Φορτώνει ένα μοντέλο. Ένα μοντέλο μπορεί επίσης να φορτωθεί αυτόματα με έναν κατασκευαστή.
4. `options(smap actions)`: Ορίζει τις επιλογές εκπαίδευσης (βλ. παρακάτω).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: Προβλέπει από ένα διάνυσμα τύπου treemapif. Ο παράμετρος `labels` είναι προαιρετικός. Όταν παρέχεται, χρησιμοποιείται για να ελέγξει την προβλεπόμενη ετικέτα έναντι της ετικέτας προορισμού που αποθηκεύεται στις `labels`. Εάν το `infos` είναι true, το πρώτο στοιχείο αυτού του διανύσματος είναι ένας χάρτης πληροφοριών.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: Προβλέπει από ένα αρχείο εισόδου. Το αποτέλεσμα είναι ένα διάνυσμα. Εάν το `infos` είναι true, το πρώτο στοιχείο αυτού του διανύσματος είναι ένας χάρτης πληροφοριών.
7. `savemodel(string outputfilename)`: Αποθηκεύει το μοντέλο σε ένα αρχείο.
8. `trainingset(fvector labels, vector data)`: Δημιουργεί το σύνολο εκπαίδευσης από ένα διάνυσμα τύπου treemapif.
9. `train(string inputdata, smap options, string outputfilename)`: Εκπαιδεύει τα δεδομένα εκπαίδευσης με κάποιες επιλογές. Το `outputfilename` είναι προαιρετικό. Θα χρησιμοποιηθεί για να αποθηκεύσει το τελικό μοντέλο εάν παρέχεται (η μέθοδος μπορεί επίσης να καλεστεί με το όνομα `load`).

## Επιλογές Εκπαίδευσης

Οι επιλογές εκπαίδευσης πρέπει να παρέχονται ως smap, με τα ακόλουθα κλειδιά: `s`, `e`, `c`, `p`, `B`, `wi`, `M` και `v`.

1. `s` type: Ορίζει τον τύπο του επιλυτή (προεπιλογή 1). Οι δυνατές τιμές είναι:
   - Για πολυκατηγορική ταξινόμηση:
     - 0: Λογαριθμική παλινδρόμηση με L2-κανονικοποίηση (πρωτεύον)
     - 1: Υποστήριξη διανυσματικής ταξινόμησης με L2-απώλεια και L2-κανονικοποίηση (διπλό)
     - 2: Υποστήριξη διανυσματικής ταξινόμησης με L2-απώλεια και L2-κανονικοποίηση (πρωτεύον)
     - 3: Υποστήριξη διανυσματικής ταξινόμησης με L1-απώλεια και L2-κανονικοποίηση (διπλό)
     - 4: Ταξινόμηση με υποστήριξη διανυσματικών προτύπων από τον Crammer και τον Singer
     - 5: Υποστήριξη διανυσματικής ταξινόμησης με L2-απώλεια και L1-κανονικοποίηση
     - 6: Λογαριθμική παλινδρόμηση με L1-κανονικοποίηση
     - 7: Λογαριθμική παλινδρόμηση με L2-κανονικοποίηση (διπλό)
   - Για παλινδρόμηση:
     - 11: Παλινδρόμηση με υποστήριξη διανυσματικών προτύπων και L2-απώλεια (πρωτεύον)
     - 12: Παλινδρόμηση με υποστήριξη διανυσματικών προτύπων και L2-απώλεια (διπλό)
     - 13: Παλινδρόμηση με υποστήριξη διανυσματικών προτύπων και L1-απώλεια (διπλό)
2. `c` cost: Ορίζει την παράμετρο C (προεπιλογή 1).
3. `p` epsilon: Ορίζει το επιτρεπτό σφάλμα στη συνάρτηση απώλειας του SVR (προεπιλογή 0,1).
4. `e` epsilon: Ορίζει την ακρίβεια του κριτηρίου τερματισμού. Οι τιμές εξαρτώνται από τον τύπο `s`:
   - Για `s` 0 και 2: `|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2`, όπου `f` είναι η πρωτεύουσα συνάρτηση και `pos/neg` είναι ο αριθμός των θετικών/αρνητικών δεδομένων (προεπιλογή 0,01).
   - Για `s` 11: `|f'(w)|_2 <= eps*|f'(w0)|_2` (προεπιλογή 0,001).
   - Για `s` 1, 3, 4 και 7: Διπλή μέγιστη παραβίαση <= eps; παρόμοιο με το libsvm (προεπιλογή 0,1).
   - Για `s` 5 και 6: `|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1`, όπου `f` είναι η πρωτεύουσα συνάρτηση (προεπιλογή 0,01).
   - Για `s` 12 και 13: `|f'(alpha)|_1 <= eps |f'(alpha0)|`, όπου `f` είναι η δυαδική συνάρτηση (προεπιλογή 0,1).
5. `B` bias: Εάν η σταθερά είναι >= 0, το αντικείμενο x γίνεται [x; bias]; εάν < 0, δεν προστίθεται όρος σταθεράς (προεπιλογή -1).
6. `wi` weight: Τα βάρη προσαρμόζουν την παράμετρο C των διαφορετικών κατηγοριών. Το 'i' εδώ αντιπροσωπεύει ένα δείκτη. Ένα κλειδί μπορεί να φαίνεται ως "w10" για παράδειγμα.
7. `M` type: Ορίζει τον τύπο πολυκατηγορικής ταξινόμησης (προεπιλογή 0). Οι δυνατές τιμές είναι:
   - `M` 0: ένας έναντι όλων
   - `M` 1: ένας έναντι ενός
8. `v` n: λειτουργία διασταυρούμενης επικύρωσης n-fold.

Σημειώστε ότι είναι δυνατή η χρήση των ακόλουθων συμβολοσειρών αντί για ακέραιους για τον επιλυτή:
- "L2R_LR" είναι το 0
- "L2R_L2LOSS_SVC_DUAL" είναι το 1
- "L2R_L2LOSS_SVC" είναι το 2
- "L2R_L1LOSS_SVC_DUAL" είναι το 3
- "MCSVM_CS" είναι το 4
- "L1R_L2LOSS_SVC" είναι το 5
- "L1R_LR" είναι το 6
- "L2R_LR_DUAL" είναι το 7
- "L2R_L2LOSS_SVR = 11" είναι το 8
- "L2R_L2LOSS_SVR_DUAL" είναι το 9
- "L2R_L1LOSS_SVR_DUAL" είναι το 10

## Παραδείγματα

### Παράδειγμα Εκπαίδευσης

```cpp
use("linear");
// Φορτώστε τη βιβλιοθήκη
use("liblinear");

string trainFile = "output.dat";
// Δηλώστε μια μεταβλητή liblinear
liblinear train;
// Ορίστε τις επιλογές
map options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01};
// Φορτώστε το μοντέλο και αποθηκεύστε την έξοδο εκπαίδευσης στο αρχείο "model_test"
train.load(trainFile, options, "model_test");
```

### Παράδειγμα Πρόβλεψης

```cpp
use("linear");
// Το αρχείο εισόδου
string testFile = "trainData.dat";
// Δηλώστε μια μεταβλητή liblinear με το μοντέλο της (θα μπορούσαμε να χρησιμοποιήσουμε το loadmodel αντί)
liblinear predict("model_test");
// Εκτελέστε την πρόβλεψη από ένα αρχείο
vector result = predict.predictfromfile(testFile, true);
```