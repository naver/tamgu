# Bibliothèque word2vec

La bibliothèque **word2vec** dans Tamgu fournit une encapsulation de l'outil word2vec. Pour plus d'informations sur word2vec, veuillez visiter le [site web Word2Vec](https://code.google.com/p/word2vec/).

Cette bibliothèque vous permet d'entraîner le système sur des corpus et d'utiliser le modèle entraîné pour trouver des distances ou des analogies entre les mots.

## Méthodes

1. `accuracy(vector mots, int seuil)`: Cette méthode trouve les précisions pour un vecteur de mots. Elle renvoie une carte de fréquence (fmap). Si le seuil n'est pas fourni, sa valeur par défaut est 30000.
2. `analogie(svector mots)`: Cette méthode trouve des analogies pour un groupe de mots. Elle renvoie une carte de fréquence.
3. `distance(svector mots)`: Cette méthode trouve la distance entre les mots dans un vecteur. Elle renvoie une carte de fréquence.
4. `caractéristiques()`: Cette méthode renvoie une carte du vocabulaire avec leurs valeurs de caractéristiques.
5. `initialisation(map m)`: Cette méthode initialise un ensemble d'entraînement word2vec.
6. `chargement_modele(string nom_fichier, bool normaliser)`: Cette méthode charge un modèle word2vec.
7. `entrainement_modele(vector v)`: Cette méthode lance l'entraînement. Si le vecteur n'est pas fourni, le système utilise le fichier d'entrée donné dans les options d'initialisation.
8. `vocabulaire()`: Cette méthode renvoie une carte d'itérateur du vocabulaire couvert par l'entraînement.

## Options

Les options pour la bibliothèque sont fournies sous la forme d'une carte. Ces options sont les mêmes que celles attendues par l'outil word2vec.

Exemple d'options :
```cpp
map options = {
    "train": "input.txt",
    "output": "output.txt",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};
```

Pour une meilleure explication de ces options, veuillez lire les informations appropriées sur le site web Word2Vec. Les options les plus importantes sont :
- "train" : Cette option doit être associée au fichier qui sera utilisé comme matériel d'entraînement.
- "output" : La valeur de cette clé est le fichier de sortie où le modèle d'entraînement final sera stocké.
- "window" : Cette valeur définit le nombre de mots pris en compte comme contexte approprié pour un jeton donné.
- "threads" : Word2Vec utilise des threads pour accélérer le processus. Vous pouvez définir le nombre de threads que le système peut utiliser.
- "size" : Cette valeur définit la taille du vecteur associé à chaque jeton.
- "iter" : Cette valeur définit le nombre d'itérations pour construire le modèle.

Une fois ces options fournies, appelez la méthode `initialisation` pour les définir.

Exemple :
```cpp
use("word2vec");
word2vec wrd;

// La fenêtre sera de 5 mots autour du mot principal.
// La taille du vecteur pour chaque mot sera de 200.
// Le système utilisera 20 threads pour calculer le modèle final avec 15 itérations.
map options = {
    "train": "input.txt",
    "output": "output.bin",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};

wrd.initialisation(options);
wrd.entrainement_modele();
```

## Utilisation

Pour utiliser un modèle entraîné, vous devez le charger en utilisant la méthode `chargement_modele`. Ensuite, vous pouvez utiliser les méthodes `distance`, `analogie` ou `précision` pour trouver les distances, les analogies ou les précisions entre les mots. Toutes ces méthodes renvoient une liste de mots avec leurs distances par rapport aux mots dans les vecteurs d'entrée. Le vocabulaire par rapport auquel les mots sont comparés est celui extrait du document d'entrée. Vous pouvez accéder à tous ces mots avec la méthode `vocabulaire`.

Exemple :
```cpp
use("word2vec");
word2vec wrd;

// Charger le modèle obtenu par l'entraînement
wrd.chargement_modele("output.bin");

svector v = ["mot"];
fmap res = wrd.distance(v);
```

## Type w2vector

Chaque mot extrait du document d'entrée est associé à un vecteur spécifique dont la taille est définie lors de l'entraînement avec l'option "size". Dans l'exemple, la taille est fixée à 200.

Il est possible d'extraire un vecteur spécifique du vocabulaire d'entraînement et de le stocker dans un objet spécifique appelé w2vector.

### Méthodes

1. `produit_scalaire(élément)`: Cette méthode renvoie le produit scalaire entre deux mots. L'élément peut être soit une chaîne de caractères, soit un w2vector.
2. `cosinus(élément)`: Cette méthode renvoie la distance cosinus entre deux mots. L'élément peut être soit une chaîne de caractères, soit un w2vector.
3. `distance(élément)`: Cette méthode renvoie la distance entre deux mots. L'élément peut être soit une chaîne de caractères, soit un w2vector.
4. `seuil(élément)`: Cette méthode renvoie ou définit le seuil.
5. `norme(élément)`: Cette méthode renvoie la norme du vecteur.

### Création

Pour créer un objet w2vector, vous devez d'abord charger un modèle, puis fournir à la fois la chaîne de jeton et un seuil.

Exemple :
```cpp
use("word2vec");
w2vector wrd;

// Charger le modèle obtenu par l'entraînement
wrd.chargement_modele("output.bin");

w2vector w = wrd["tsunami": 0.5];
w2vector ww = wrd["earthquake": 0.5];
println(w.distance(ww));
```

Le seuil n'est pas obligatoire. Il est utilisé lors de la comparaison de deux éléments w2vector pour voir s'ils sont proches. Le seuil est utilisé pour détecter si la distance entre les deux éléments est supérieure ou égale au seuil.

En d'autres termes, `if (w == ww)` est équivalent à `if (w.distance(ww) >= w.seuil())`.

Exemple :
```cpp
if (w == ww)
    println("ok");
if (w == "earthquake") // On peut comparer avec une simple chaîne de caractères
    println("ok");
```