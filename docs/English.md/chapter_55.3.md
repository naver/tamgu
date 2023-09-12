## Training Options

The training options should be provided as a map, with the following keys:

1. 's' type: set the type of solver (default 1)
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
2. 'c' cost: set the parameter C (default 1)
3. 'p' epsilon: set the epsilon in the loss function of SVR (default 0.1)
4. 'e' epsilon: set the tolerance of the termination criterion
   - For 's' 0 and 2:
     - |f'(w)|_2 <= eps * min(pos, neg) / l * |f'(w0)|_2, where f is the primal function and pos/neg are the number of positive/negative data (default 0.01)
   - For 's' 11:
     - |f'(w)|_2 <= eps * |f'(w0)|_2 (default 0.001)
   - For 's' 1, 3, 4, and 7:
     - Dual maximal violation <= eps; similar to libsvm (default 0.1)
   - For 's' 5 and 6:
     - |f'(w)|_1 <= eps * min(pos, neg) / l * |f'(w0)|_1, where f is the primal function (default 0.01)
   - For 's' 12 and 13:
     - |f'(alpha)|_1 <= eps * |f'(alpha0)|, where f is the dual function (default 0.1)
5. 'B' bias: if bias >= 0, instance x becomes [x; bias]; if < 0, no bias term is added (default -1)
6. 'wi' weight: weights adjust the parameter C of different classes. 'i' stands for an index. A key might look like "w10" for instance.
7. 'M' type: type of multiclass classification (default 0)
   - 'M' 0: one-versus-all
   - 'M' 1: one-versus-one
8. 'v' n: n-fold cross-validation mode

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

Example:

```python
s = {'s': 'L1R_LR', 'B': 1, 'v': 9}
```

The input structure for both the predict and training set methods:

These two methods accept as input two structures. The first one is a feature vector, which will contain the so-called labels (float elements). The second one is a vector of treemaps. The two structures should have exactly the same size.

Each element in the second parameter vector is a treemap, where the key is the index and the value is the associated probability. This structure has been chosen to store sparse vectors.

The output of the predict methods:

The output of the predict methods is a vector of maps. The first element is an info map, which contains some measures over the whole analysis (such as the "accuracy"). The next elements, depending on the flag "predict_probability", are either the predicted label or a map containing for each line from the input structure the label with the associated list of probabilities.

With predict probability:

```python
{'1': [0.999725, 3.66243e-05, 4.85055e-06, 4.49336e-07, 6.43783e-05]}
```

The key is the chosen label, with the list of its probabilities.

Without predict probability:

It is simply the chosen label.