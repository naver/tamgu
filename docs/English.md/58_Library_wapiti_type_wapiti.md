# Library wapiti

libwapiti is an encapsulation of the wapiti library, which is available at [http://wapiti.limsi.fr](http://wapiti.limsi.fr). 

Copyright (c) 2009-2013 CNRS. All rights reserved.

Wapiti provides an efficient implementation of the CRF method for tagging and entity extraction. If you need any information on the system, please refer to the manual at [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html).

## Usage

To use the library, you need to load the wapiti module:

```python
use('wapiti')
```

The library exposes the following methods:

### Methods

1. `loadmodel(string crfmodel)`: Loads a CRF model.
2. `options(svector options)`: Sets options. See below for the available options. Options should be placed in the svector as used on the command line of wapiti.
3. `train()`: Launches training. Requires options to have been set in advance.
4. `label(vector words)`: Launches labeling for a vector of tokens. Returns a vector of labels for each token.
5. `lasterror()`: Returns the last error that occurred.

### Options

Wapiti exposes some options to deal with all the possibilities ingrained in the system. Below is a list of these options, which should be supplied as an svector, exactly as these options would be provided with the command line version of wapiti.

#### Train mode:

- `train [options] [input data] [model file]`
  - `--me`: Force maxent mode
  - `--type STRING`: Type of model to train
  - `--algo STRING`: Training algorithm to use
  - `--pattern FILE`: Patterns for extracting features
  - `--model FILE`: Model file to preload
  - `--devel FILE`: Development dataset
  - `--rstate FILE`: Optimizer state to restore
  - `--sstate FILE`: Optimizer state to save
  - `--compact`: Compact model after training
  - `--nthread INT`: Number of worker threads
  - `--jobsize INT`: Job size for worker threads
  - `--sparse`: Enable sparse forward/backward
  - `--maxiter INT`: Maximum number of iterations
  - `--rho1 FLOAT`: L1 penalty parameter
  - `--rho2 FLOAT`: L2 penalty parameter
  - `--objwin INT`: Convergence window size
  - `--stopwin INT`: Stop window size
  - `--stopeps FLOAT`: Stop epsilon value
  - `--clip`: Clip gradient (l-bfgs)
  - `--histsz INT`: History size (l-bfgs)
  - `--maxls INT`: Max linesearch iterations (l-bfgs)
  - `--eta0 FLOAT`: Learning rate (sgd-l1)
  - `--alpha FLOAT`: Exp decay parameter (sgd-l1)
  - `--kappa FLOAT`: Stability parameter (bcd)
  - `--stpmin FLOAT`: Minimum step size (rprop)
  - `--stpmax FLOAT`: Maximum step size (rprop)
  - `--stpinc FLOAT`: Step increment factor (rprop)
  - `--stpdec FLOAT`: Step decrement factor (rprop)
  - `--cutoff`: Alternate projection (rprop)

#### Label mode:

- `label [options] [input data] [output data]`
  - `--me`: Force maxent mode
  - `--model FILE`: Model file to load
  - `--label`: Output only labels
  - `--check`: Input is already labeled
  - `--score`: Add scores to output
  - `--post`: Label using posteriors
  - `--nbest INT`: Output n-best list
  - `--force`: Use forced decoding

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

You also need a "pattern" file, which should be implemented according to the manual of CRF++ or as described in the [Wapiti manual](http://wapiti.limsi.fr/manual.html).

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

Here is a small program that uses the library to train a model and label entities. 

```python
use('wapiti')

wapiti tst

# Load the model
tst.loadmodel("model")

# Label a vector of tokens
words = ['Growth NOUN', '& CONJ', 'Income NOUN', 'Fund NOUN', '( PUNCT', 'Exact ADJ', 'name NOUN', 'of PREP', 'registrant NOUN', 'as PREP', 'specified ADJ', 'in PREP', 'charter NOUN']
res = tst.label(words)

# Print the list of tags
println(res)
```

The result is: `['ORGANISATION_b', 'ORGANISATION_i', 'ORGANISATION_i', 'ORGANISATION_i', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null']`