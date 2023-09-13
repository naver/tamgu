# Usage

To use a model that has been created, you simply need to follow these steps:

1. Load the model using the `loadmodel` function.
2. Choose one of the following methods: `distance`, `analogy`, or `accuracy`.
3. Each of these methods will return a list of words along with their distance to the words in the input vectors.
4. The vocabulary used for comparison is extracted from the input document.
5. You can access all the words in the vocabulary using the `vocabulary` function.

Here is an example:

```python
import word2vec

# Load the word2vec model
model = word2vec.loadmodel("output.bin")

# Create an input vector
input_vector = ["word"]

# Calculate the distance between the input vector and the words in the model
result = model.distance(input_vector)
```

Note: Replace `"output.bin"` with the path to your trained model file.