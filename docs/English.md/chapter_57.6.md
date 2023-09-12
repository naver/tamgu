# Methods

1. `accuracy(vector words, int threshold)`: This method finds accuracies for a vector of four words. It returns a `fmap`. If the threshold is not supplied, its default value is 30000.

2. `analogy(vector words)`: This method finds analogies for a group of words. It returns a `fmap`.

3. `distance(vector words)`: This method finds the distance in a vector of words. It returns a `fmap`.

4. `features()`: This method returns a map of the vocabulary with their feature values.

5. `initialization(map m)`: This method initializes a word2vec training set.

6. `loadmodel(string filename, bool normalize)`: This method loads a model.

7. `trainmodel(vector v)`: This method launches the training. If `v` is not supplied, then the system utilizes the input file given in the initialization options.

8. `vocabulary()`: This method returns an `itreemap` of the vocabulary covered by the training.