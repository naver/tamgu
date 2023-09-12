## Usage

To use a model that has been created, simply follow these steps:

1. Load the model using the `loadmodel` function.
2. Use one of the following methods: `distance`, `analogy`, or `accuracy`. These methods will return a list of words along with their distance to the words in the input vectors.
3. The vocabulary used for comparison is extracted from the input document. You can access this vocabulary using the `vocabulary` function.

### Example

```python
from word2vec import Word2Vec

# Load the word2vec model
w2v = Word2Vec()
w2v.loadmodel("output.bin")

# Create an input vector
input_vector = ["word"]

# Get the distance of the input vector from the model
result = w2v.distance(input_vector)
```

In the example above, we first load the word2vec model from the "output.bin" file. Then, we create an input vector with the word "word". Finally, we use the `distance` method to calculate the distance between the input vector and the words in the model.