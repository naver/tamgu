## Methods

1. `cleandata()`: Cleans internal data.
2. `crossvalidation()`: Relaunches the cross-validation with new parameters. The result is a `fmap`.
3. `loadmodel(string filename)`: Loads your model. A model can also be automatically loaded with a constructor.
4. `options(smap actions)`: Sets the training options (see below).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: Predicts from a vector of `treemapif`. `labels` is optional. When it is provided, it is used to test the predicted label against the target label stored in `labels`. If `infos` is true, the first element of this vector is an info map.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: Predicts from a file `input`. The result is a vector. If `infos` is true, the first element of this vector is an info map.
7. `savemodel(string outputfilename)`: Saves your model in a file.
8. `trainingset(fvector labels, vector data)`: Creates your training set out of a `treemapif` vector.
9. `train(string inputdata, smap options, string outputfilename)`: Trains your training data with some options. `outputfilename` is optional. It will be used to store the final model if provided (the method can also be called with the name `load`).