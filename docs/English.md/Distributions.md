## Distributions

The first parameter, `nb`, defines the number of elements that are returned as vectors. If `nb` is empty, then a single value (integer or float) is returned. The values defined after the "=" sign are the default values.

### Random Choice
`random_choice(int nb, vector v)`

Tamgu provides a function that returns a list of values based on the values of `v`. The `random_choice` function is based on a discrete distribution, where each value has the same probability of occurring.

**Example:**
```
random_choice(10, ["a", "b", "c"])
```
Returns:
```
['c', 'a', 'b', 'a', 'b', 'c', 'b', 'c', 'a', 'a']
```

### Uniform Distributions
`uniform_int(int nb, int a = 0, int b = max_value)`

Produces random integer values `i`, uniformly distributed on the closed interval `[a, b]`, that is, distributed according to the discrete probability function.

Parameters: `a`, `b`

### Geometric Distribution
`geometric_distribution(float p)`

Constructs a `geometric_distribution` object, adopting the distribution parameter specified by `p`.

Parameter: `p`

Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This should be a value between 0.0 and 1.0 (both included).

### Poisson Distribution
`poisson_distribution(float mean = 1)`

Constructs a `poisson_distribution` object, adopting the distribution parameter specified by `mean`.

Parameter: `mean`

Expected number of events in the interval (μ). This represents the rate at which the events being counted are observed, on average. Its value should be positive (μ > 0).

### Exponential Distribution
`exponential_distribution(float lambda = 1)`

Constructs an `exponential_distribution` object, adopting the distribution parameter specified by `lambda`.

Parameter: `lambda`

Average rate of occurrence (λ). This represents the number of times the random events are observed by interval, on average. Its value should be positive (λ > 0).

### Normal Distribution
`normal_distribution(float mean = 0, float stddev = 1)`

Constructs a `normal_distribution` object, adopting the distribution parameters specified by `mean` and `stddev`, or by object `parm`.

Parameters: `mean`, `stddev`

- `mean`: Mean of the distribution (its expected value, μ), which coincides with the location of its peak.
- `stddev`: Standard deviation (σ), the square root of variance, representing the dispersion of values from the distribution mean. This should be a positive value (σ > 0).

### Lognormal Distribution
`lognormal_distribution(float m = 0, float s = 1)`

Constructs a `lognormal_distribution` object, adopting the distribution parameters specified by `m` and `s`.

Parameters: `m`, `s`

- `m`: Mean of the underlying normal distribution formed by the logarithm transformations of the possible values in this distribution.
- `s`: Standard deviation of the underlying normal distribution formed by the logarithm transformations of the possible values in this distribution. This should be a positive value (s > 0).

### Chi-Squared Distribution
`chi_squared_distribution(float n = 1)`

Constructs a `chi_squared_distribution` object, adopting the distribution parameter specified by `n`.

Parameter: `n`

Number of degrees of freedom. Its value should be positive (n > 0).

### Sampling Distributions
`discrete_distribution(ivector il)`

The sequence in the list `il` is used as the weights for each integer value from 0 to (n - 1), where `n` is the size of the initializer list.

**Example:**
```
ivector iv = discrete_distribution(10, [40, 10, 10, 40]);
```

`piecewise_constant_distribution(fvector firstb, fvector firstw)`

The values in the range defined within `firstB` are used as the bounds for the subintervals (bi), and the sequence beginning at `firstW` is used as the weights (wi) for each subinterval.

**Example:**
```
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res;
res = piecewise_constant_distribution(100, intervals, weights);
```

`piecewise_linear_distribution(fvector firstb, fvector firstw)`

The values in the range within `firstB` are used as the bounds for the subintervals (bi), and the sequence beginning at `firstW` is used as the weights (wi) for each subinterval bound.

**Example:**