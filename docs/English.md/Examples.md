## Examples
### Training example
```python
use("linear")
# Load the library
use("liblinear")
trainFile = "output.dat"
# Declare a liblinear variable
train = liblinear()
# Set the options
options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01}
# Load the model, with the training output stored in the model_test file
train.load(trainFile, options, "model_test")
```

### Predict example
```python
use("linear")
# The input file
testFile = "trainData.dat"
# Declare a liblinear variable with its model (we could use loadmodel instead)
```