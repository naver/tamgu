# w2vector

The `w2vector` class is used to associate each word extracted from an input document with a specific vector. The size of the vector is determined during training and can be set using the `size` option. In our example, the size is set to 200.

It is possible to extract a specific vector from the training vocabulary and store it in a `w2vector` object.

## Methods

1. `dot(element)`: Returns the dot product between two words. The `element` parameter can be either a string or a `w2vector` object.
2. `cosine(element)`: Returns the cosine distance between two words. The `element` parameter can be either a string or a `w2vector` object.
3. `distance(element)`: Returns the distance between two words. The `element` parameter can be either a string or a `w2vector` object.
4. `threshold(element)`: Returns or sets the threshold.
5. `norm(element)`: Returns the vector norm.

## Creation

To initialize a `w2vector` object, you need to first load a model and then provide the token string and a threshold.

Example:

```python
use("word2vec")
wrd = w2vector()
# Load the model obtained through training
wrd.loadmodel("output.bin")
w = wrd["tsunami": 0.5]
ww = wrd["earthquake": 0.5]
println(w.distance(ww))
```

The threshold is not mandatory. It is used to compare two `w2vector` elements and determine if they are close. The threshold is used to detect if the distance between the two elements is greater than the threshold value.

In other words, `if (w == ww)` is equivalent to `if (w.distance(ww) >= w.threshold())`.

Example:

```python
if (w == ww)
    println("ok")
if (w == "earthquake")  # We can compare against a simple string
    println("ok")
```

## fvector

It is also possible to retrieve the inner float vector from a `w2vector`.

Example:

```python
fvector vvv = w
vvv is:
[0.049775, -0.0498451, -0.0722533, 0.0536649, -0.000515156, -0.0947062, 0.0294775, -0.0146792, -0.100351, 0.0480318, 0.071128, 0.0268629...]
```