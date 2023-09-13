# Library LINEAR : Linear Type

Tamgu provides an encapsulation of the liblinear library, which is used to implement classifiers. For more information about this library, please visit [http://www.csie.ntu.edu.tw/~cjlin/liblinear/](http://www.csie.ntu.edu.tw/~cjlin/liblinear/).

## Methods

The library exposes the following methods:

1. `cleandata()`: Cleans internal data.
2. `crossvalidation()`: Relaunches the cross validation with new parameters. The result is a fmap.
3. `loadmodel(string filename)`: Loads a model. A model can also be automatically loaded with a constructor.
4. `options(smap actions)`: Sets the training options (see below).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: Predicts from a vector of treemapif. The `labels` parameter is optional. When provided, it is used to test the predicted label against the target label stored in `labels`. If `infos` is true, the first element of this vector is an info map.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: Predicts from a file input. The result is a vector. If `infos` is true, the first element of this vector is an info map.
7. `savemodel(string outputfilename)`: Saves the model in a file.
8. `trainingset(fvector labels, vector data)`: Creates the training set out of a treemapif vector.
9. `train(string inputdata, smap options, string outputfilename)`: Trains the training data with some options. `outputfilename` is optional. It will be used to store the final model if provided (the method can also be called with the name `load`).

## Training Options

The training options should be provided as a smap, with the following keys: `s`, `e`, `c`, `p`, `B`, `wi`, `M`, and `v`.

1. `s` type: Sets the type of solver (default 1). The possible values are:
   - For multiclass classification:
     - 0: L2-regularized logistic regression (primal)
     - 1: L2-regularized L2-loss support vector classification (dual)
     - 2: L2-regularized L2-loss support vector classification (primal)
     - 3: L2-regularized L1-loss support vector classification (dual)
     - 4: Support vector classification by Crammer and Singer
     - 5: L1-regularized L2-loss support vector classification
     - 6: L1-regularized logistic regression
     - 7: L2-regularized logistic regression (dual)
   - For regression:
     - 11: L2-regularized L2-loss support vector regression (primal)
     - 12: L2-regularized L2-loss support vector regression (dual)
     - 13: L2-regularized L1-loss support vector regression (dual)
2. `c` cost: Sets the parameter C (default 1).
3. `p` epsilon: Sets the epsilon in the loss function of SVR (default 0.1).
4. `e` epsilon: Sets the tolerance of the termination criterion. The values depend on the type `s`:
   - For `s` 0 and 2: `|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2`, where `f` is the primal function and `pos/neg` are the number of positive/negative data (default 0.01).
   - For `s` 11: `|f'(w)|_2 <= eps*|f'(w0)|_2` (default 0.001).
   - For `s` 1, 3, 4, and 7: Dual maximal violation <= eps; similar to libsvm (default 0.1).
   - For `s` 5 and 6: `|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1`, where `f` is the primal function (default 0.01).
   - For `s` 12 and 13: `|f'(alpha)|_1 <= eps |f'(alpha0)|`, where `f` is the dual function (default 0.1).
5. `B` bias: If bias >= 0, instance x becomes [x; bias]; if < 0, no bias term is added (default -1).
6. `wi` weight: Weights adjust the parameter C of different classes. 'i' stands here for an index. A key might look like "w10" for instance.
7. `M` type: Sets the type of multiclass classification (default 0). The possible values are:
   - `M` 0: one-versus-all
   - `M` 1: one-versus-one
8. `v` n: n-fold cross-validation mode.

Note that it is possible to use the following strings instead of integers for the solver:
- "L2R_LR" is 0
- "L2R_L2LOSS_SVC_DUAL" is 1
- "L2R_L2LOSS_SVC" is 2
- "L2R_L1LOSS_SVC_DUAL" is 3
- "MCSVM_CS" is 4
- "L1R_L2LOSS_SVC" is 5
- "L1R_LR" is 6
- "L2R_LR_DUAL" is 7
- "L2R_L2LOSS_SVR = 11" is 8
- "L2R_L2LOSS_SVR_DUAL" is 9
- "L2R_L1LOSS_SVR_DUAL" is 10

## Examples

### Training Example

```cpp
use("linear");
// Load the library
use("liblinear");

string trainFile = "output.dat";
// Declare a liblinear variable
liblinear train;
// Set the options
map options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01};
// Load the model, and store the training output in the "model_test" file
train.load(trainFile, options, "model_test");
```

### Prediction Example

```cpp
use("linear");
// The input file
string testFile = "trainData.dat";
// Declare a liblinear variable with its model (we could use loadmodel instead)
liblinear predict("model_test");
// Perform the prediction from a file
vector result = predict.predictfromfile(testFile, true);
```