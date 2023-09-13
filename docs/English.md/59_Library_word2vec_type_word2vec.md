# Library word2vec

The **word2vec** library in Tamgu provides an encapsulation of the word2vec tool. For more information about word2vec, please visit the [Word2Vec website](https://code.google.com/p/word2vec/).

This library allows you to train the system on corpora and use the trained model to find distances or analogies between words.

## Methods

1. `accuracy(vector words, int threshold)`: This method finds accuracies for a vector of words. It returns a frequency map (fmap). If the threshold is not supplied, its default value is 30000.
2. `analogy(svector words)`: This method finds analogies for a group of words. It returns a frequency map.
3. `distance(svector words)`: This method finds the distance between words in a vector. It returns a frequency map.
4. `features()`: This method returns a map of the vocabulary with their feature values.
5. `initialization(map m)`: This method initializes a word2vec training set.
6. `loadmodel(string filename, bool normalize)`: This method loads a word2vec model.
7. `trainmodel(vector v)`: This method launches the training. If the vector is not supplied, the system uses the input file given in the initialization options.
8. `vocabulary()`: This method returns an iterator map of the vocabulary covered by the training.

## Options

The options for the library are supplied as a map. These options are the same as the ones expected by the word2vec tool.

Example options:
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

For a better explanation of these options, please read the appropriate information on the Word2Vec website. The most important options are:
- "train": This option should be associated with the file that will be used as training material.
- "output": The value for this key is the output file where the final training model will be stored.
- "window": This value defines the number of words taken into account as a proper context for a given token.
- "threads": Word2Vec utilizes threads to speed up the process. You can define the number of threads the system can use.
- "size": This value defines the size of the vector associated with each token.
- "iter": This value defines the number of iterations to build the model.

Once these options have been supplied, call the `initialization` method to set them in.

Example:
```cpp
use("word2vec");
word2vec wrd;

// Window will be 5 words around the main word.
// Vector size for each word will be 200.
// The system will use 20 threads to compute the final model with 15 iterations.
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

wrd.initialization(options);
wrd.trainmodel();
```

## Usage

To use a trained model, you need to load it using the `loadmodel` method. After that, you can use the `distance`, `analogy`, or `accuracy` methods to find distances, analogies, or accuracies between words. All these methods return a list of words with their distances to the words in the input vectors. The vocabulary against which the words are compared is the one extracted from the input document. You can access all these words with the `vocabulary` method.

Example:
```cpp
use("word2vec");
word2vec wrd;

// Load the model obtained through training
wrd.loadmodel("output.bin");

svector v = ["word"];
fmap res = wrd.distance(v);
```

## Type w2vector

Each word extracted from the input document is associated with a specific vector whose size is defined at training time with the "size" option. In the example, the size is set to 200.

It is possible to extract a specific vector from the training vocabulary and store it into a specific object called w2vector.

### Methods

1. `dot(element)`: This method returns the dot product between two words. The element can be either a string or a w2vector.
2. `cosine(element)`: This method returns the cosine distance between two words. The element can be either a string or a w2vector.
3. `distance(element)`: This method returns the distance between two words. The element can be either a string or a w2vector.
4. `threshold(element)`: This method returns or sets the threshold.
5. `norm(element)`: This method returns the vector norm.

### Creation

To create a w2vector object, you need to load a model first and then provide both the token string and a threshold.

Example:
```cpp
use("word2vec");
w2vector wrd;

// Load the model obtained through training
wrd.loadmodel("output.bin");

w2vector w = wrd["tsunami": 0.5];
w2vector ww = wrd["earthquake": 0.5];
println(w.distance(ww));
```

The threshold is not mandatory. It is used when comparing two w2vector elements to see if they are close. The threshold is used to detect whether the distance between the two elements is greater than or equal to the threshold.

In other words, `if (w == ww)` is equivalent to `if (w.distance(ww) >= w.threshold())`.

Example:
```cpp
if (w == ww)
    println("ok");
if (w == "earthquake") // We can compare against a simple string
    println("ok");
```