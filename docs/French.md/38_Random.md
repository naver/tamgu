# Aléatoire

## Nombre aléatoire : random(), a_random()

Tamgu fournit une fonction pour retourner une valeur aléatoire entre 0 et 99. La fonction `random()` retourne une valeur longue. Vous pouvez également fournir une valeur limite maximale en argument. `a_random()` est une version sans verrou de `random()`.

Exemple :
```cpp
float rd = random(); // valeur entre 0 et 99
rd = random(999); // valeur entre 0 et 999
```

## Distributions

### Choix aléatoire

La fonction `random_choice()` dans Tamgu retourne une liste de valeurs basée sur les valeurs d'un vecteur. La randomisation est basée sur une distribution discrète, où chaque valeur a la même probabilité d'apparaître.

Exemple :
```cpp
random_choice(10, ["a", "b", "c"])
// retourne par exemple : ['c', 'a', 'b', 'a', 'b', 'c', 'b', 'c', 'a', 'a']
```

### Distributions uniformes

#### Uniforme entier

La fonction `uniform_int()` produit des valeurs entières aléatoires `i`, uniformément distribuées sur l'intervalle fermé `[a, b]`, c'est-à-dire distribuées selon la fonction de probabilité discrète.

Paramètres : `a`, `b`
- `a` : Limite inférieure de la plage de valeurs possibles que la distribution peut générer.
- `b` : Limite supérieure de la plage de valeurs possibles que la distribution peut générer.

Notez que `b` doit être supérieur ou égal à `a` (`a < b`).

Elle retourne un `int` ou un `ivector` de taille `nb`.

#### Uniforme réel

La fonction `uniform_real()` construit un objet `uniform_real_distribution`, en adoptant les paramètres de distribution spécifiés par `a` et `b`.

Paramètres : `a`, `b`
- `a` : Limite inférieure de la plage de valeurs possibles que la distribution peut générer.
- `b` : Limite supérieure de la plage de valeurs possibles que la distribution peut générer.

Notez que l'intervalle inclut `a` mais pas `b`. De plus, `b` doit être supérieur ou égal à `a` (`a <= b`).

Elle retourne un `float` ou un `fvector` de taille `nb`.

### Distributions de Bernoulli

#### Distribution de Bernoulli

La fonction `bernoulli_distribution()` construit un objet `bernoulli_distribution`, en adoptant les paramètres de distribution spécifiés par `t` et `p`.

Paramètres : `t`, `p`
- `t` : La limite supérieure de l'intervalle `[0, t]` des valeurs possibles que la distribution peut générer. Cela représente le nombre d'expériences indépendantes suivant une distribution de Bernoulli que chaque valeur générée est censée simuler.
- `p` : Probabilité de succès. Cela représente la probabilité de succès pour chacune des expériences indépendantes suivant une distribution de Bernoulli que chaque valeur générée est censée simuler. Cela doit être une valeur entre 0,0 et 1,0 (inclus).

Elle retourne un `ivector` de valeurs booléennes (0 ou 1) de taille `nb`.

#### Distribution binomiale

La fonction `binomial_distribution()` construit un objet `binomial_distribution`, en adoptant les paramètres de distribution spécifiés par `t` et `p`.

Paramètres : `t`, `p`
- `t` : La limite supérieure de l'intervalle `[0, t]` des valeurs possibles que la distribution peut générer. Cela représente le nombre d'expériences indépendantes suivant une distribution de Bernoulli que chaque valeur générée est censée simuler.
- `p` : Probabilité de succès. Cela représente la probabilité de succès pour chacune des expériences indépendantes suivant une distribution de Bernoulli que chaque valeur générée est censée simuler. Cela doit être une valeur entre 0,0 et 1,0 (inclus).

Elle retourne un `int` ou un `ivector` de taille `nb`.

#### Distribution binomiale négative

La fonction `negative_binomial_distribution()` construit un objet `negative_binomial_distribution`, en adoptant les paramètres de distribution spécifiés par `k` et `p`.

Paramètres : `k`, `p`
- `k` : Paramètre `k` de la distribution binomiale négative. Cela représente le nombre d'essais infructueux qui arrête le décompte des expériences réussies suivant une distribution de Bernoulli que chaque valeur générée est censée simuler.
- `p` : Probabilité de succès. Cela représente la probabilité de succès pour chacune des expériences indépendantes suivant une distribution de Bernoulli que chaque valeur générée est censée simuler. Cela doit être une valeur entre 0,0 et 1,0 (inclus).

Elle retourne un `int` ou un `ivector` de taille `nb`.

#### Distribution géométrique

La fonction `geometric_distribution()` construit un objet `geometric_distribution`, en adoptant le paramètre de distribution spécifié par `p`.

Paramètre : `p`
- `p` : Probabilité de succès. Cela représente la probabilité de succès pour chacune des expériences indépendantes suivant une distribution de Bernoulli que chaque valeur générée est censée simuler. Cela doit être une valeur entre 0,0 et 1,0 (inclus).

Elle retourne un `int` ou un `ivector` de taille `nb`.

### Distributions de Poisson

#### Distribution de Poisson

La fonction `poisson_distribution()` construit un objet `poisson_distribution`, en adoptant le paramètre de distribution spécifié par `mean`.

Paramètre : `mean`
- `mean` : Nombre attendu d'événements dans l'intervalle (μ). Cela représente le taux auquel les événements comptés sont observés, en moyenne. Sa valeur doit être positive (μ > 0).

Elle retourne un `int` ou un `ivector` de taille `nb`.

#### Distribution exponentielle

La fonction `exponential_distribution()` construit un objet `exponential_distribution`, en adoptant le paramètre de distribution spécifié par `lambda`.

Paramètre : `lambda`
- `lambda` : Taux moyen d'occurrence (λ). Cela représente le nombre de fois où les événements aléatoires sont observés par intervalle, en moyenne. Sa valeur doit être positive (λ > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution gamma

La fonction `gamma_distribution()` construit un objet `gamma_distribution`, en adoptant les paramètres de distribution spécifiés par `alpha` et `beta`.

Paramètres : `alpha`, `beta`
- `alpha` : Paramètre `alpha` (α), qui définit la forme de la distribution. Sa valeur doit être positive (α > 0).
- `beta` : Paramètre `beta` (β), qui définit l'échelle de la distribution. Sa valeur doit être positive (β > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution de Weibull

La fonction `weibull_distribution()` construit un objet `weibull_distribution`, en adoptant les paramètres de distribution spécifiés par `a` et `b`.

Paramètres : `a`, `b`
- `a` : Paramètre de distribution `a`, qui définit la forme de la distribution. Sa valeur doit être positive (a > 0).
- `b` : Paramètre de distribution `b`, qui définit l'échelle de la distribution. Sa valeur doit être positive (b > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution de valeur extrême

La fonction `extreme_value_distribution()` construit un objet `extreme_value_distribution`, en adoptant les paramètres de distribution spécifiés par `a` et `b`.

Paramètres : `a`, `b`
- `a` : Paramètre de distribution `a`, qui définit la position (décalage) de la distribution.
- `b` : Paramètre de distribution `b`, qui définit l'échelle de la distribution. Sa valeur doit être positive (b > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

### Distributions normales

#### Distribution normale

La fonction `normal_distribution()` construit un objet `normal_distribution`, en adoptant les paramètres de distribution spécifiés par `mean` et `stddev`.

Paramètres : `mean`, `stddev`
- `mean` : Moyenne de la distribution (sa valeur attendue, μ), qui coïncide avec la position de son pic.
- `stddev` : Écart type (σ), la racine carrée de la variance, représentant la dispersion des valeurs par rapport à la moyenne de la distribution. Sa valeur doit être positive (σ > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution lognormale

La fonction `lognormal_distribution()` construit un objet `lognormal_distribution`, en adoptant les paramètres de distribution spécifiés par `m` et `s`.

Paramètres : `m`, `s`
- `m` : Moyenne de la distribution normale sous-jacente formée par les transformations logarithmiques des valeurs possibles dans cette distribution.
- `s` : Écart type de la distribution normale sous-jacente formée par les transformations logarithmiques des valeurs possibles dans cette distribution. Sa valeur doit être positive (s > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution du chi carré

La fonction `chi_squared_distribution()` construit un objet `chi_squared_distribution`, en adoptant le paramètre de distribution spécifié par `n`.

Paramètre : `n`
- `n` : Nombre de degrés de liberté, qui spécifie le nombre de variables indépendantes simulées par la distribution. Sa valeur doit être positive (n > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution de Cauchy

La fonction `cauchy_distribution()` construit un objet `cauchy_distribution`, en adoptant les paramètres de distribution spécifiés par `a` et `b`.

Paramètres : `a`, `b`
- `a` : Paramètre de distribution `a`, qui spécifie la position du pic (son mode).
- `b` : Paramètre de distribution `b`, qui définit l'échelle de la distribution. Sa valeur doit être positive (b > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution de Fisher

La fonction `fisher_distribution()` construit un objet `fisher_f_distribution`, en adoptant les paramètres de distribution spécifiés par `m` et `n`.

Paramètres : `m`, `n`
- `m` : Paramètre de distribution `m`, qui spécifie les degrés de liberté du numérateur. Sa valeur doit être positive (m > 0).
- `n` : Paramètre de distribution `n`, qui spécifie les degrés de liberté du dénominateur. Sa valeur doit être positive (n > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

#### Distribution de Student

La fonction `student_distribution()` construit un objet `student_t_distribution`, en adoptant le paramètre de distribution spécifié par `n`.

Paramètre : `n`
- `n` : Degrés de liberté. Sa valeur doit être positive (n > 0).

Elle retourne un `float` ou un `fvector` de taille `nb`.

### Distributions d'échantillonnage

#### Distribution discrète

La fonction `discrete_distribution()` utilise la séquence dans la liste `il` comme poids pour chaque valeur entière de 0 à `(n - 1)`, où `n` est la taille de la liste d'initialisation.

Elle retourne un `int` ou un `ivector` de taille `nb`.

Exemple :
```cpp
ivector iv = discrete_distribution(10, [40, 10, 10, 40]);
```

#### Distribution constante par morceaux

La fonction `piecewise_constant_distribution()` utilise les valeurs dans la plage définie par `firstB` comme bornes pour les sous-intervalles (`bi`), et la séquence commençant à `firstW` comme poids (`wi`) pour chaque sous-intervalle.

Elle retourne un `float` ou un `fvector` de taille `nb`.

Exemple :
```cpp
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res = piecewise_constant_distribution(100, intervals, weights);
```

#### Distribution linéaire par morceaux

La fonction `piecewise_linear_distribution()` utilise les valeurs dans la plage définie par `firstB` comme bornes pour les sous-intervalles (`bi`), et la séquence commençant à `firstW` comme poids (`wi`) pour chaque borne de sous-intervalle.

Elle retourne un `float` ou un `fvector` de taille `nb`.

Exemple :
```cpp
fvector intervals = [0.0, 4.5, 9.0];
fvector weights = [10.0, 0.0, 10.0];
fvector res = piecewise_linear_distribution(100, intervals, weights);
```