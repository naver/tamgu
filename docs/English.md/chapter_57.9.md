## w2vector

The `w2vector` is a Python library that allows you to associate each word extracted from an input document with a specific vector. The size of the vector is determined during training and can be set using the `size` option. In our example, the size is set to 200.

You can extract a specific vector from the training vocabulary and store it in the `w2vector` object.

### Methods

1. `dot(element)`: Returns the dot product between two words. The `element` parameter can be either a string or a `w2vector` object.

2. `cosine(element)`: Returns the cosine distance between two words. The `element` parameter can be either a string or a `w2vector` object.