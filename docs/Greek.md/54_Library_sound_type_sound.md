# Βιβλιοθήκη Ήχου

Η βιβλιοθήκη Ήχου στο Tamgu παρέχει τρόπο αναπαραγωγής διαφόρων τύπων αρχείων ήχου, όπως WAV, MP3, FLAC, OGG, κ.λπ. Μπορείτε εύκολα να φορτώσετε ένα αρχείο και να το αναπαραγάγετε οπουδήποτε στον κώδικά σας χρησιμοποιώντας τον τύπο "sound".

**Σημείωση:** Το Tamgu βασίζεται στις παρακάτω βιβλιοθήκες για την αποκωδικοποίηση και αναπαραγωγή: libao4, libsndfile-1 και libmpg123.

Για να χρησιμοποιήσετε αυτήν τη βιβλιοθήκη, μπορείτε να την καλέσετε χρησιμοποιώντας `use("sound")`.

## Μέθοδοι

Η API παρέχει τις εξής μεθόδους:

1. `close()`: Κλείνει ένα κανάλι ήχου.
2. `decode(ivector soundbuffer)`: Αποκωδικοποιεί το αρχείο ήχου και επιστρέφει το περιεχόμενο buffer ανά buffer στο `soundbuffer`. Επιστρέφει false όταν φτάνει στο τέλος του αρχείου.
3. `encode(ivector soundbuffer)`: Αναπαράγει ένα buffer ήχου που επιστρέφεται από το `decode`.
4. `load(string pathname)`: Φορτώνει τον ήχο από την καθορισμένη `pathname`.
5. `open(map params)`: Ανοίγει ένα κανάλι ήχου με τις παραμέτρους του τρέχοντος αρχείου ήχου (δείτε τις παραμέτρους).
6. `parameters()`: Επιστρέφει τις παραμέτρους του τρέχοντος αρχείου ήχου ως έναν χάρτη.
7. `parameters(map modifs)`: Μόνο οι "ρυθμός" και "κανάλια" μπορούν να τροποποιηθούν.
8. `play()`: Αναπαράγει τον ήχο.
9. `play(bool beg)`: Αναπαράγει τον ήχο από την αρχή.
10. `play(ivector soundbuffer)`: Αναπαράγει το buffer ήχου (δείτε `encode`).
11. `reset()`: Επαναφέρει το αρχείο ήχου στην αρχή.
12. `stop()`: Διακόπτει τον ήχο. Είναι απαραίτητο να αναπαραγάγετε το αρχείο ήχου σε ένα νήμα για να χρησιμοποιήσετε αυτήν την εντολή.

## Παράδειγμα

```cpp
use("sound");
sound s;
s.load("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
s.play();
```

Μπορείτε επίσης να φορτώσετε έναν ήχο με την ακόλουθη δήλωση:

```cpp
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
```

## Παράδειγμα Αποκωδικοποίησης

```cpp
use("sound");

// Ανοίξτε ένα αρχείο ήχου
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");

// Ανοίξτε ένα δεύτερο κανάλι ήχου
sound c;

// Πάρτε τις παραμέτρους του ήχου
map params = s.parameters();

// Χρησιμοποιήστε τις παραμέτρους του ήχου για να ανοίξετε ένα κανάλι
c.open(params);

// Επανάλαβε με την αποκωδικοποίηση στο αρχείο ήχου
// και για κάθε νέο buffer, αναπαράγετε τον ήχο
// Θα μπορούσαμε να χρησιμοποιήσουμε "play" αντί για "encode",
// αλλά είναι λίγο πιο αργό
ivector snd;
while (s.decode(snd))
    c.encode(snd);

// Κλείστε το κανάλι
c.close();
```