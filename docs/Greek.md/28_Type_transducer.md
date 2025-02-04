# Τύπος Transducer

Αυτός ο τύπος επικεντρώνεται στην αποθήκευση και την χειρισμό λεξικών με έναν πολύ συμπαγή και αποδοτικό τρόπο. Εκθέτει τις ακόλουθες μεθόδους:

## Μέθοδοι

1. `add(container, bool norm, int encoding)`: Μετατρέπει έναν container (διάνυσμα ή χάρτη) σε ένα λεξικό transducer. Εάν το container είναι ένα διάνυσμα, πρέπει να έχει ένα άρτιο αριθμό τιμών.
2. `build(string input, string output, bool norm, int encoding)`: Δημιουργεί ένα αρχείο transducer από ένα αρχείο κειμένου που περιέχει την επιφανειακή μορφή στην πρώτη γραμμή και το λήμμα+χαρακτηριστικά στην επόμενη γραμμή.
3. `compilergx(string rgx, svector features, string filename)`: Δημιουργεί ένα αρχείο transducer από κανονικές εκφράσεις. Το όνομα αρχείου είναι προαιρετικό και ο αυτόματος που προκύπτει αποθηκεύεται σε ένα αρχείο.
4. `load(string file)`: Φορτώνει ένα αρχείο transducer.
5. `lookdown(string lemFeat, byte lemma)`: Αναζητά μια επιφανειακή μορφή που ταιριάζει με ένα λήμμα συν τα χαρακτηριστικά του. Το λήμμα είναι προαιρετικό. Όταν είναι 1 ή 2, η συμβολοσειρά που θα συγκριθεί μπορεί να μειωθεί μόνο σε ένα λήμμα. Εάν το λήμμα είναι 2, επιστρέφονται επίσης και τα χαρακτηριστικά. Σημείωση: Το λήμμα πρέπει να χωρίζεται από τα χαρακτηριστικά με ένα tab.
6. `lookup(string wrd, int threshold, int flags)`: Αναζητά μια λέξη χρησιμοποιώντας ένα transducer και ένα σύνολο δυνητικών ενεργειών σε συνδυασμό με ένα κατώφλι. Οι τελευταίοι δύο παράμετροι μπορούν να παραλειφθούν. Οι διαθέσιμες ενέργειες είναι:
   - `a_first`: Ο αυτόματος μπορεί να εφαρμόσει ενέργειες στον πρώτο χαρακτήρα.
   - `a_change`: Ο αυτόματος μπορεί να αλλάξει ένα χαρακτήρα σε έναν άλλο.
   - `a_delete`: Ο αυτόματος μπορεί να διαγράψει ένα χαρακτήρα.
   - `a_insert`: Ο αυτόματος μπορεί να εισαγάγει ένα χαρακτήρα.
   - `a_switch`: Ο αυτόματος μπορεί να ανταλλάξει δύο χαρακτήρες.
   - `a_nocase`: Ο αυτόματος ελέγχει εάν οι δύο χαρακτήρες μπορούν να ταιριάξουν ανεξάρτητα από την πεζή.
   - `a_repetition`: Ο αυτόματος δέχεται ότι ένας χαρακτήρας επαναλαμβάνεται μερικές φορές.
   - `a_vowel`: Ο αυτόματος συγκρίνει αποτονωμένα φωνήεντα μαζί. Για παράδειγμα, το "e" θα ταιριάξει με το "é" ή το "è" αλλά όχι με το "a".
   - `a_surface`: Ο αυτόματος επιστρέφει μόνο την επιφανειακή μορφή.
7. `parse(string sentence, int option, int threshold, int flags)`: Αναλύει μια ακολουθία λέξεων χρησιμοποιώντας ένα transducer. Η επιλογή μπορεί να πάρει τις ακόλουθες τιμές:
   - `0`: Επιστρέφει μόνο τις επιφανειακές μορφές που αναγνωρίστηκαν στο αρχικό string.
   - `1`: Ίδιο με το `0`, αλλά με τις θέσεις τους.
   - `2`: Επιστρέφει τις επιφανειακές μορφές και τα λήμματα με τα χαρακτηριστικά τους που αναγνωρίστηκαν στην αρχική είσοδο.
   - `3`: Ίδιο με το `2`, αλλά με τις θέσεις τους. Το κατώφλι και τα flags είναι προαιρετικά και ακολουθούν την ίδια συμβολή όπως και για το `lookup`.
8. `store(string output, bool norm, int encoding)`: Αποθηκεύει ένα transducer σε ένα αρχείο. Οι τελευταίες δύο παράμετροι είναι προαιρετικές.

## Μορφή

Η μορφή των αρχείων που μεταγλωττίζονται σε λεξικά είτε μέσω της `build` είτε μέσω της `add` έχει μια παρόμοια δομή. Στην περίπτωση ενός αρχείου, η πρώτη γραμμή πρέπει να είναι μια επιφανειακή μορφή, ενώ η επόμενη γραμμή πρέπει να είναι ένα λήμμα με ορισμένα χαρακτηριστικά, χωρισμένα με ένα tab.

Παράδειγμα:
```
classes
class +Πληθυντικός+Ουσιαστικό
class
class +Ενικός+Ουσιαστικό
```

Η συνάρτηση `build` παίρνει ένα τέτοιο αρχείο ως είσοδο και δημιουργεί ένα αρχείο που περιέχει το αντίστοιχο transducer με βάση αυτές τις γραμμές. Οι άλλες δύο παράμετροι χρησιμοποιούνται κατά την επεξεργασία μιας λέξης ή κειμένου.

- Η κανονικοποίηση σημαίνει ότι το λεξικό μπορεί να αναγνωρίσει λέξεις χωρίς να είναι διάκριση πεζών-κεφαλαίων. Έτσι, αυτό το λεξικό θα αναγνωρίσει "CLASS" ως μια λέξη.
- Το σύστημα έχει υλοποιηθεί για να αναγνωρίζει λέξεις σε κωδικοποίηση UTF8 (στην πραγματικότητα τα αυτόματα αποθηκεύονται σε Unicode). Ωστόσο, είναι δυνατό να ενημερώσετε το σύστημα για τον τρόπο με τον οποίο λαμβάνει υπόψη τους κωδικοποιητές Latin. Για παράδειγμα, μπορείτε να δώσετε στο σύστημα τον αριθμό 5 ως κωδικοποίηση, που σε αυτήν την περίπτωση αναφέρεται στο Latin 5, που χρησιμοποιείται για να κωδικοποιήσει χαρακτήρες της κυριλλικής γλώσσας. Η προεπιλεγμένη τιμή είναι Latin 1.

## Διάνυσμα

Στην περίπτωση ενός διανύσματος ως εισόδου στην `add`, η δομή θα είναι λίγο διαφορετική. Οι άρτιες θέσεις στο διάνυσμα θα είναι η επιφανειακή μορφή, ενώ οι μονές θέσεις θα είναι τα λήμματα συν τα χαρακτηριστικά τους, και πάλι χωρισμένα με ένα tab.

## Χάρτης

Για ένα χάρτη, το κλειδί θα είναι η επιφανειακή μορφή και η τιμή θα είναι τα λήμματα με τα χαρακτηριστικά τους. Ένας χάρτης μπορεί να δημιουργήσει προβλήματα για να αποθηκεύσει αμφίσημες λέξεις.

## Επεξεργασία Συμβολοσειρών

Υπάρχουν διάφοροι τρόποι επεξεργασίας συμβολοσειρών με ένα transducer.

### `lookup`

Το `lookup` χρησιμοποιείται για να εντοπίσει εάν μια λέξη ανήκει στο transducer και σε αυτήν την περίπτωση επιστρέφει το λήμμα και τα χαρακτηριστικά της. Ο transducer μπορεί να επιστρέψει περισσότερες από μία λύσεις. Η μεταβλητή παραλήπτης πρέπει να είναι ένα διάνυσμα εάν θέλετε να ανακτήσετε όλες τις δυνατές λύσεις.

Παράδειγμα:
```cpp
t.lookup("class") επιστρέφει: class +Ενικός+Ουσιαστικό
```

Μπορείτε να περιορίσετε την επεξεργασία μιας συμβολοσειράς με ένα κατώφλι απόστασης επεξεργασίας και ενέργειες.

```cpp
t.lookup("cliss", 1, a_change) επιστρέφει: class +Ενικός+Ουσιαστικό
```

### `lookdown`

Το `lookdown` χρησιμοποιείται για να ανακτήσει τη σωστή επιφανειακή μορφή μιας λέξης χρησιμοποιώντας το λήμμα και τα χαρακτηριστικά της.

Παράδειγμα:
```cpp
t.lookdown("class +Πληθυντικός+Ουσιαστικό") επιστρέφει: classes
```

### `parse`

Το `parse` διαιρεί μια συμβολοσειρά σε τμήματα και επιστρέφει, για κάθε τμήμα, το λήμμα+χαρακτηριστικά του ως ένα διάνυσμα όλων των πιθανοτήτων.

Παράδειγμα:
```cpp
transducer t(_current + "english.tra");
string sentence = "The lady drinks a glass of milk.";
vector v = t.parse(sentence);
printjln(v);
```

Έξοδος:
```
['The', 'The +0+3+0+3+Prop+WordParticle+Sg+NOUN', 'the +0+3+0+3+Det+Def+SP+DET']
['lady', 'lady +4+8+4+8+Noun+Sg+NOUN']
['drinks', 'drink +9+15+9+15+Verb+Trans+Pres+3sg+VERB', 'drink +9+15+9+15+Noun+Pl+NOUN']
['a', 'a +16+17+16+17+Det+Indef+Sg+DET']
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Verb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

Σημείωση: Το `parse` επιστρέφει επίσης τη θέση κάθε λέξης στην αρχική πρόταση.

## Κανονικές Εκφράσεις

Οι κανονικές εκφράσεις που επεξεργάζεται το transducer είναι πολύ περιορισμένες:

1. `%c`: Ορίζει ένα χαρακτήρα, όπου `c` είναι ένας χαρακτήρας UTF8.
2. `$..`: Ορίζει μια συμβολοσειρά.
3. `u-u`: Ορίζει ένα διάστημα μεταξύ δύο χαρακτήρων Unicode.
4. `[..]`: Ορίζει μια ακολουθία χαρακτήρων.
5. `{...}`: Ορίζει μια διάκριση συμβολοσειρών.
6. `.+`: Η δομή πρέπει να εμφανίζεται τουλάχιστον μία φορά.
7. `(..)`: Ορίζει μια προαιρετική δομή.
8. `!n`: Εισάγει μια δομή χαρακτηριστικών μαζί με τον αριθμό της στο διάνυσμα χαρακτηριστικών (n>=1).

Παραδείγματα:
```cpp
transducer t;
// Αυτή η έκφραση αναγνωρίζει Ρωμαϊκούς Αριθμούς
t.compilergx("{DMCLXVI}+!1", ["\t+Rom"]);
// Αυτή η έκφραση αναγνωρίζει οποιονδήποτε τύπο αριθμών, συμπεριλαμβανομένου του δεκαδικού διαχωριστικού και των εκθετικών εκφράσεων.
t.compilergx("({-+}){0-9}+!1(%.{0-9}+!2({eE}({-+}){0-9}+!3))", ["+Card", "+Dec", "+Exp+Dec"]);
// Για να αναγνωρίσετε τους τακτικούς αριθμούς
t.compilergx("{[1ος][2ος][3ος]}!1", ["+Ord"]);
t.compilergx("[3-9]([0-9]+)$ος!1", ["+Ord"]);
// Θέλουμε να αναγνωρίσουμε οποιεσδήποτε συμβολοσειρές που αποτελούνται από το ελληνικό αλφάβητο
t.compilergx("{α-ω0-9}+!1", ["+Greek"]);

int i;
string s;
for (i in <945,970,1>) s += i.chr();
println(t.lookup("MMMDDD")); // MMMDDD +Rom
println(t.lookup("1234")); // 1234 +Card
println(t.lookup("1.234")); // 1.234 +Dec
println(t.lookup("1.234e-8")); // 1.234e-8 +Exp+Dec
println(t.lookup("1ος")); // 1ος +Ord
println(t.lookup("2ος")); // 2ος +Ord
println(t.lookup("3ος")); // 3ος +Ord
println(t.lookup("4ος")); // 4ος +Ord
println(t.lookup(s)); // αβγδεζηθικλμνξοπρςστυφχψ +Greek
```