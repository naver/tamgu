# Library wapiti: Text Classification and Entity Extraction

**libwapiti** is an encapsulation of the wapiti library, which can be found at [http://wapiti.limsi.fr](http://wapiti.limsi.fr). 

© 2009-2013 CNRS. All rights reserved.

Wapiti provides an efficient implementation of the CRF method for tagging and entity extraction. If you need any information about the system, please refer to the manual at [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html).

## Usage

The name of the library is **wapiti**. To use it, use the following command: `use('wapiti');`

**libwapiti** exposes the following methods:

### Methods

1. `loadmodel(string crfmodel)`: Load a CRF model.
2. `options(svector options)`: Set options. See below for the available options. Options should be placed in the svector as used on the command line of wapiti.
3. `train()`: Launch training. Requires options to be set in advance.
4. `label(vector words)`: Launch labeling for a vector of tokens. Returns a vector of labels for each token.
5. `lasterror()`: Return the last error that occurred.

### Options

Wapiti exposes some options to deal with all the possibilities embedded in the system. Below is a list of these options, which should be supplied as a svector, exactly as these options would be provided with the command line version of wapiti.

- Train mode:
  - `train [options] [input data] [model file]`
  - `-s` or `--score`: Add scores to output.
  - `-p` or `--post`: Label using posteriors.
  - `-n` or `--nbest INT`: Output n-best list.
  - `--force`: Use forced decoding.

## Training

To train a CRF, you need a text file with annotations, where each line is a token with its tags separated by a tab.

Example:
```
UNITED STATES  NOUN  LOCATION_b
SECURITIES  NOUN  ORGANISATION_i
AND  CONJ  ORGANISATION_i
EXCHANGE  NOUN  ORGANISATION_i
COMMISSION  NOUN  ORGANISATION_i
Washington  NOUN  ORGANISATION_i
, PUNCT  ORGANISATION_i
D.C. NOUN  LOCATION_b
20549  DIG NUMBER_b
FORM  VERB  null
N NOUN  null
```

In this example, each line represents a token associated with two different tags. The tag "null" in this example is a simple string that does not have a specific interpretation except for this specific example.

You also need a "pattern" file, which should be implemented according to the manual of CRF++ (on which it is based) or as described in [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html).

Example pattern file:
```
# Unigram
U00:%x[-2,0]
U01:%x[-1,0]
U02:%x[0,0]
U03:%x[1,0]
U04:%x[2,0]
U05:%x[-2,1]
U06:%x[-1,1]
U07:%x[0,1]
U08:%x[1,1]
U09:%x[2,1]
U10:%x[-2,0]/%x[0,0]
U11:%x[-1,0]/%x[0,0]
U12:%x[0,0]/%x[1,0]
U13:%x[0,0]/%x[2,0]
U14:%x[-2,1]/%x[0,1]
U15:%x[-1,1]/%x[0,1]
U16:%x[0,1]/%x[1,1]
U17:%x[0,1]/%x[2,1]
# Bigram
B
```

## Program

Here is an example program that takes a training file and a pattern file as input to produce the model that will be used to label entities.

```python
use('wapiti');
wapiti tst;

# We are going to produce our model based on the pattern file and the training file
svector v = ["train", "-p", "pattern", "-1", "5", "training", "model"];
tst.options(v);
tst.train();
```

## Labeling

The labeling is done through the method `label`. In order to use this method, you must first load the model that you produced through training. The process consists of sending a list of tokens and receiving as output a vector of the same size containing the corresponding labels. You need to provide the system with a list of tokens, each associated with their specific tags.