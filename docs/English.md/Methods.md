# Methods

1. `accuracy(vector words, int threshold)`: Finds accuracies for a vector of four words. Returns a frequency map. If the threshold is not provided, its default value is 30000.
2. `analogy(vector words)`: Finds analogies for a group of words. Returns a frequency map.
3. `distance(vector words)`: Finds the distance in a vector of words. Returns a frequency map.
4. `features()`: Returns a map of the vocabulary with their feature values.
5. `initialization(map m)`: Initializes a word2vec training set.
6. `loadmodel(string filename, bool normalize)`: Loads a model.
7. `trainmodel(vector v)`: Launches the training. If the vector is not provided, the system utilizes the input file given in the initialization options.
8. `vocabulary()`: Returns a sorted map of the vocabulary covered by the training.