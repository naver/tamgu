## Options

The options are supplied as a map to the library. These options are exactly the same as the ones expected by the library.

```python
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

## Library word2vec: type word2vec in Tamgu

Tamgu provides an encapsulation of word2vec. See [here](https://code.google.com/p/word2vec/) for more information.

With this library, you can both train the system on corpora and use the result through distance or analogy.

The name of the library is word2vec: `use("word2vec");`