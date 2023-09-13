# Random

## Random number: random(), a_random()

Tamgu provides a function to return a random value between 0 and 99. The `random()` function returns a long value. You can also provide a maximum boundary value as an argument. `a_random()` is a lock-free version of `random()`.

Example:
```cpp
float rd = random(); // value between 0 and 99
rd = random(999); // value between 0 and 999
```

## Distributions

### Random Choice

The `random_choice()` function in Tamgu returns a list of values based on the values of a vector. The randomization is based on a discrete distribution, where each value has the same probability to occur.

Example:
```cpp
random_choice(10, ["a", "b", "c"])
// returns for instance: ['c', 'a', 'b', 'a', 'b', 'c', 'b', 'c', 'a', 'a']
```

### Uniform Distributions

#### Uniform Int

The `uniform_int()` function produces random integer values `i`, uniformly distributed on the closed interval `[a, b]`, that is, distributed according to the discrete probability function.

Parameters: `a`, `b`
- `a`: Lower bound of the range of possible values the distribution can generate.
- `b`: Upper bound of the range of possible values the distribution can generate.

Note that `b` shall be greater than or equal to `a` (`a < b`).

It returns an `int` or an `ivector` of size `nb`.

#### Uniform Real

The `uniform_real()` function constructs a `uniform_real_distribution` object, adopting the distribution parameters specified by `a` and `b`.

Parameters: `a`, `b`
- `a`: Lower bound of the range of possible values the distribution can generate.
- `b`: Upper bound of the range of possible values the distribution can generate.

Note that the range includes `a` but not `b`. Also, `b` shall be greater than or equal to `a` (`a <= b`).

It returns a `float` or an `fvector` of size `nb`.

### Bernoulli Distributions

#### Bernoulli Distribution

The `bernoulli_distribution()` function constructs a `bernoulli_distribution` object, adopting the distribution parameters specified by `t` and `p`.

Parameters: `t`, `p`
- `t`: The upper bound of the range `[0, t]` of possible values the distribution can generate. This represents the number of independent Bernoulli-distributed experiments each generated value is said to simulate.
- `p`: Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This shall be a value between 0.0 and 1.0 (both included).

It returns an `ivector` of Boolean values (0 or 1) of size `nb`.

#### Binomial Distribution

The `binomial_distribution()` function constructs a `binomial_distribution` object, adopting the distribution parameters specified by `t` and `p`.

Parameters: `t`, `p`
- `t`: The upper bound of the range `[0, t]` of possible values the distribution can generate. This represents the number of independent Bernoulli-distributed experiments each generated value is said to simulate.
- `p`: Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This shall be a value between 0.0 and 1.0 (both included).

It returns an `int` or an `ivector` of size `nb`.

#### Negative Binomial Distribution

The `negative_binomial_distribution()` function constructs a `negative_binomial_distribution` object, adopting the distribution parameters specified by `k` and `p`.

Parameters: `k`, `p`
- `k`: Parameter `k` of the negative binomial distribution. This represents the number of unsuccessful trials that stops the count of successful Bernoulli-distributed experiments each generated value is said to simulate.
- `p`: Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This shall be a value between 0.0 and 1.0 (both included).

It returns an `int` or an `ivector` of size `nb`.

#### Geometric Distribution

The `geometric_distribution()` function constructs a `geometric_distribution` object, adopting the distribution parameter specified by `p`.

Parameter: `p`
- `p`: Probability of success. This represents the probability of success on each of the independent Bernoulli-distributed experiments each generated value is said to simulate. This shall be a value between 0.0 and 1.0 (both included).

It returns an `int` or an `ivector` of size `nb`.

### Poisson Distributions

#### Poisson Distribution

The `poisson_distribution()` function constructs a `poisson_distribution` object, adopting the distribution parameter specified by `mean`.

Parameter: `mean`
- `mean`: Expected number of events in the interval (μ). This represents the rate at which the events being counted are observed, on average. Its value shall be positive (μ > 0).

It returns an `int` or an `ivector` of size `nb`.

#### Exponential Distribution

The `exponential_distribution()` function constructs an `exponential_distribution` object, adopting the distribution parameter specified by `lambda`.

Parameter: `lambda`
- `lambda`: Average rate of occurrence (λ). This represents the number of times the random events are observed by interval, on average. Its value shall be positive (λ > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Gamma Distribution

The `gamma_distribution()` function constructs a `gamma_distribution` object, adopting the distribution parameters specified by `alpha` and `beta`.

Parameters: `alpha`, `beta`
- `alpha`: Parameter `alpha` (α), that defines the shape of the distribution. This shall be a positive value (α > 0).
- `beta`: Parameter `beta` (β), that defines the scale of the distribution. This shall be a positive value (β > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Weibull Distribution

The `weibull_distribution()` function constructs a `weibull_distribution` object, adopting the distribution parameters specified by `a` and `b`.

Parameters: `a`, `b`
- `a`: Distribution parameter `a`, which defines the shape of the distribution. This shall be a positive value (a > 0).
- `b`: Distribution parameter `b`, which defines the scale of the distribution. This shall be a positive value (b > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Extreme Value Distribution

The `extreme_value_distribution()` function constructs an `extreme_value_distribution` object, adopting the distribution parameters specified by `a` and `b`.

Parameters: `a`, `b`
- `a`: Distribution parameter `a`, which defines the location (shift) of the distribution.
- `b`: Distribution parameter `b`, which defines the scale of the distribution. This shall be a positive value (b > 0).

It returns a `float` or an `fvector` of size `nb`.

### Normal Distributions

#### Normal Distribution

The `normal_distribution()` function constructs a `normal_distribution` object, adopting the distribution parameters specified by `mean` and `stddev`.

Parameters: `mean`, `stddev`
- `mean`: Mean of the distribution (its expected value, μ), which coincides with the location of its peak.
- `stddev`: Standard deviation (σ), the square root of variance, representing the dispersion of values from the distribution mean. This shall be a positive value (σ > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Lognormal Distribution

The `lognormal_distribution()` function constructs a `lognormal_distribution` object, adopting the distribution parameters specified by `m` and `s`.

Parameters: `m`, `s`
- `m`: Mean of the underlying normal distribution formed by the logarithm transformations of the possible values in this distribution.
- `s`: Standard deviation of the underlying normal distribution formed by the logarithm transformations of the possible values in this distribution. This shall be a positive value (s > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Chi-squared Distribution

The `chi_squared_distribution()` function constructs a `chi_squared_distribution` object, adopting the distribution parameter specified by `n`.

Parameter: `n`
- `n`: Number of degrees of freedom, which specifies the number of independent variables simulated by the distribution. This shall be a positive value (n > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Cauchy Distribution

The `cauchy_distribution()` function constructs a `cauchy_distribution` object, adopting the distribution parameters specified by `a` and `b`.

Parameters: `a`, `b`
- `a`: Distribution parameter `a`, which specifies the location of the peak (its mode).
- `b`: Distribution parameter `b`, which defines the scale of the distribution. This shall be a positive value (b > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Fisher F-distribution

The `fisher_distribution()` function constructs a `fisher_f_distribution` object, adopting the distribution parameters specified by `m` and `n`.

Parameters: `m`, `n`
- `m`: Distribution parameter `m`, which specifies the numerator's degrees of freedom. This shall be a positive value (m > 0).
- `n`: Distribution parameter `n`, which specifies the denominator's degrees of freedom. This shall be a positive value (n > 0).

It returns a `float` or an `fvector` of size `nb`.

#### Student T-distribution

The `student_distribution()` function constructs a `student_t_distribution` object, adopting the distribution parameter specified by `n`.

Parameter: `n`
- `n`: Degrees of freedom. Its value shall be positive (n > 0).

It returns a `float` or an `fvector` of size `nb`.

### Sampling Distributions

#### Discrete Distribution

The `discrete_distribution()` function uses the sequence in the list `il` as the weights for each integer value from 0 to `(n - 1)`, where `n` is the size of the initializer list.

It returns an `int` or an `ivector` of size `nb`.

Example:
```cpp
ivector iv = discrete_distribution(10, [40, 10, 10, 40]);
```

#### Piecewise Constant Distribution

The `piecewise_constant_distribution()` function uses the values in the range defined within `firstB` as the bounds for the subintervals (`bi`), and the sequence beginning at `firstW` as the weights (`wi`) for each subinterval.

It returns a `float` or an `fvector` of size `nb`.

Example:
```cpp
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res = piecewise_constant_distribution(100, intervals, weights);
```

#### Piecewise Linear Distribution

The `piecewise_linear_distribution()` function uses the values in the range within `firstB` as the bounds for the subintervals (`bi`), and the sequence beginning at `firstW` as the weights (`wi`) for each subinterval bound.

It returns a `float` or an `fvector` of size `nb`.

Example:
```cpp
fvector intervals = [0.0, 4.5, 9.0];
fvector weights = [10.0, 0.0, 10.0];
fvector res = piecewise_linear_distribution(100, intervals, weights);
```