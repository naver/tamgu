## Methods

1. `accuracy(vector words, int threshold)`: Find accuracies for a vector of words, which may occur multiple times. Returns a fmap. If the threshold is not provided, its default value is 30000.

2. `analogy(svector words)`: Find analogies for a group of words. Returns a fmap.

3. `distance(svector words)`: Find the distance in a vector of words. Returns a fmap.

4. `features()`: Return a map of the vocabulary with their feature values.

5. `initialization(map m)`: Initialize a word2vec training set.

6. `loadmodel(string filename, bool normalize)`: Load a model.

7. `trainmodel(vector v)`: Launch the training. If `v` is not provided, the system utilizes the input file given in the initialization options.

8. `vocabulary()`: Return an itreemap of the vocabulary covered by the training.