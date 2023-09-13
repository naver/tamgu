# 랜덤

## 랜덤 숫자: random(), a_random()

Tamgu는 0부터 99까지의 랜덤 값을 반환하는 함수를 제공합니다. `random()` 함수는 long 값을 반환합니다. 최대 경계 값을 인수로 제공할 수도 있습니다. `a_random()`은 `random()`의 락 프리 버전입니다.

예시:
```cpp
float rd = random(); // 0과 99 사이의 값
rd = random(999); // 0과 999 사이의 값
```

## 분포

### 랜덤 선택

Tamgu의 `random_choice()` 함수는 벡터의 값을 기반으로 한 값 목록을 반환합니다. 랜덤화는 각 값이 발생할 확률이 동일한 이산 분포를 기반으로 합니다.

예시:
```cpp
random_choice(10, ["a", "b", "c"])
// 예를 들어 반환 값: ['c', 'a', 'b', 'a', 'b', 'c', 'b', 'c', 'a', 'a']
```

### 균등 분포

#### 균등 정수

`uniform_int()` 함수는 닫힌 구간 `[a, b]`에서 균등하게 분포된 랜덤 정수 값을 생성합니다. 이는 이산 확률 함수에 따라 분포됩니다.

매개변수: `a`, `b`
- `a`: 분포가 생성할 수 있는 가능한 값의 범위의 하한값입니다.
- `b`: 분포가 생성할 수 있는 가능한 값의 범위의 상한값입니다.

`b`는 `a`보다 크거나 같아야 합니다 (`a < b`).

`int` 또는 크기 `nb`의 `ivector`를 반환합니다.

#### 균등 실수

`uniform_real()` 함수는 `a`와 `b`로 지정된 분포 매개변수를 채택하여 `uniform_real_distribution` 객체를 생성합니다.

매개변수: `a`, `b`
- `a`: 분포가 생성할 수 있는 가능한 값의 범위의 하한값입니다.
- `b`: 분포가 생성할 수 있는 가능한 값의 범위의 상한값입니다.

범위는 `a`를 포함하지만 `b`를 포함하지 않습니다. 또한 `b`는 `a`보다 크거나 같아야 합니다 (`a <= b`).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

### 베르누이 분포

#### 베르누이 분포

`bernoulli_distribution()` 함수는 `t`와 `p`로 지정된 분포 매개변수를 채택하여 `bernoulli_distribution` 객체를 생성합니다.

매개변수: `t`, `p`
- `t`: 분포가 생성할 수 있는 가능한 값의 범위 `[0, t]`입니다. 이는 각 생성된 값이 모의하는 독립적인 베르누이 분포 실험의 수를 나타냅니다.
- `p`: 성공 확률입니다. 각 생성된 값이 모의하는 독립적인 베르누이 분포 실험의 성공 확률을 나타냅니다. 이는 0.0과 1.0 사이의 값이어야 합니다 (양쪽 값을 모두 포함).

크기 `nb`의 부울 값 (0 또는 1)의 `ivector`를 반환합니다.

#### 이항 분포

`binomial_distribution()` 함수는 `t`와 `p`로 지정된 분포 매개변수를 채택하여 `binomial_distribution` 객체를 생성합니다.

매개변수: `t`, `p`
- `t`: 분포가 생성할 수 있는 가능한 값의 범위 `[0, t]`입니다. 이는 각 생성된 값이 모의하는 독립적인 베르누이 분포 실험의 수를 나타냅니다.
- `p`: 성공 확률입니다. 각 생성된 값이 모의하는 독립적인 베르누이 분포 실험의 성공 확률을 나타냅니다. 이는 0.0과 1.0 사이의 값이어야 합니다 (양쪽 값을 모두 포함).

`int` 또는 크기 `nb`의 `ivector`를 반환합니다.

#### 음이항 분포

`negative_binomial_distribution()` 함수는 `k`와 `p`로 지정된 분포 매개변수를 채택하여 `negative_binomial_distribution` 객체를 생성합니다.

매개변수: `k`, `p`
- `k`: 음이항 분포의 매개변수 `k`입니다. 이는 생성된 각 값이 모의하는 성공한 베르누이 분포 실험의 수를 나타냅니다.
- `p`: 성공 확률입니다. 각 생성된 값이 모의하는 독립적인 베르누이 분포 실험의 성공 확률을 나타냅니다. 이는 0.0과 1.0 사이의 값이어야 합니다 (양쪽 값을 모두 포함).

`int` 또는 크기 `nb`의 `ivector`를 반환합니다.

#### 기하 분포

`geometric_distribution()` 함수는 `p`로 지정된 분포 매개변수를 채택하여 `geometric_distribution` 객체를 생성합니다.

매개변수: `p`
- `p`: 성공 확률입니다. 각 생성된 값이 모의하는 독립적인 베르누이 분포 실험의 성공 확률을 나타냅니다. 이는 0.0과 1.0 사이의 값이어야 합니다 (양쪽 값을 모두 포함).

`int` 또는 크기 `nb`의 `ivector`를 반환합니다.

### 포아송 분포

#### 포아송 분포

`poisson_distribution()` 함수는 `mean`으로 지정된 분포 매개변수를 채택하여 `poisson_distribution` 객체를 생성합니다.

매개변수: `mean`
- `mean`: 구간에서 예상되는 이벤트의 수 (μ). 이는 평균적으로 관찰되는 이벤트 수입니다. 그 값은 양수여야 합니다 (μ > 0).

`int` 또는 크기 `nb`의 `ivector`를 반환합니다.

#### 지수 분포

`exponential_distribution()` 함수는 `lambda`로 지정된 분포 매개변수를 채택하여 `exponential_distribution` 객체를 생성합니다.

매개변수: `lambda`
- `lambda`: 발생률의 평균 (λ). 이는 평균적으로 간격마다 랜덤 이벤트가 관찰되는 횟수입니다. 그 값은 양수여야 합니다 (λ > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 감마 분포

`gamma_distribution()` 함수는 `alpha`와 `beta`로 지정된 분포 매개변수를 채택하여 `gamma_distribution` 객체를 생성합니다.

매개변수: `alpha`, `beta`
- `alpha`: 분포의 형태를 정의하는 매개변수 `alpha` (α). 이는 양수 값이어야 합니다 (α > 0).
- `beta`: 분포의 척도를 정의하는 매개변수 `beta` (β). 이는 양수 값이어야 합니다 (β > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 와이블 분포

`weibull_distribution()` 함수는 `a`와 `b`로 지정된 분포 매개변수를 채택하여 `weibull_distribution` 객체를 생성합니다.

매개변수: `a`, `b`
- `a`: 분포의 형태를 정의하는 분포 매개변수 `a`. 이는 양수 값이어야 합니다 (a > 0).
- `b`: 분포의 척도를 정의하는 분포 매개변수 `b`. 이는 양수 값이어야 합니다 (b > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 극값 분포

`extreme_value_distribution()` 함수는 `a`와 `b`로 지정된 분포 매개변수를 채택하여 `extreme_value_distribution` 객체를 생성합니다.

매개변수: `a`, `b`
- `a`: 분포의 위치 (이동)를 정의하는 분포 매개변수 `a`.
- `b`: 분포의 척도를 정의하는 분포 매개변수 `b`. 이는 양수 값이어야 합니다 (b > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

### 정규 분포

#### 정규 분포

`normal_distribution()` 함수는 `mean`과 `stddev`로 지정된 분포 매개변수를 채택하여 `normal_distribution` 객체를 생성합니다.

매개변수: `mean`, `stddev`
- `mean`: 분포의 평균 (기대값, μ)으로서, 분포의 최고점에 해당합니다.
- `stddev`: 표준 편차 (σ), 분포 평균값으로부터의 값들의 분산을 나타냅니다. 그 값은 양수여야 합니다 (σ > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 로그 정규 분포

`lognormal_distribution()` 함수는 `m`과 `s`로 지정된 분포 매개변수를 채택하여 `lognormal_distribution` 객체를 생성합니다.

매개변수: `m`, `s`
- `m`: 이 분포의 가능한 값들의 로그 변환으로 형성된 기저 정규 분포의 평균입니다.
- `s`: 이 분포의 가능한 값들의 로그 변환으로 형성된 기저 정규 분포의 표준 편차입니다. 그 값은 양수여야 합니다 (s > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 카이 제곱 분포

`chi_squared_distribution()` 함수는 `n`으로 지정된 분포 매개변수를 채택하여 `chi_squared_distribution` 객체를 생성합니다.

매개변수: `n`
- `n`: 자유도의 수로, 분포에 의해 시뮬레이션되는 독립 변수의 수를 지정합니다. 그 값은 양수여야 합니다 (n > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 코시 분포

`cauchy_distribution()` 함수는 `a`와 `b`로 지정된 분포 매개변수를 채택하여 `cauchy_distribution` 객체를 생성합니다.

매개변수: `a`, `b`
- `a`: 분포의 최빈값 (모드)을 지정하는 분포 매개변수 `a`.
- `b`: 분포의 척도를 정의하는 분포 매개변수 `b`. 그 값은 양수여야 합니다 (b > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 피셔 F-분포

`fisher_distribution()` 함수는 `m`과 `n`으로 지정된 분포 매개변수를 채택하여 `fisher_f_distribution` 객체를 생성합니다.

매개변수: `m`, `n`
- `m`: 분자의 자유도를 지정하는 분포 매개변수 `m`. 그 값은 양수여야 합니다 (m > 0).
- `n`: 분모의 자유도를 지정하는 분포 매개변수 `n`. 그 값은 양수여야 합니다 (n > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

#### 스튜던트 T-분포

`student_distribution()` 함수는 `n`으로 지정된 분포 매개변수를 채택하여 `student_t_distribution` 객체를 생성합니다.

매개변수: `n`
- `n`: 자유도입니다. 그 값은 양수여야 합니다 (n > 0).

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

### 표본 분포

#### 이산 분포

`discrete_distribution()` 함수는 `il`의 순서를 가중치로 사용하여 0부터 `(n - 1)`까지의 각 정수 값에 대한 가중치로 사용합니다. 여기서 `n`은 이니셜라이저 리스트의 크기입니다.

`int` 또는 크기 `nb`의 `ivector`를 반환합니다.

예시:
```cpp
ivector iv = discrete_distribution(10, [40, 10, 10, 40]);
```

#### 조각별 상수 분포

`piecewise_constant_distribution()` 함수는 `firstB`에서 정의된 범위의 값을 하위 구간 (`bi`)의 경계로, `firstW`에서 시작하는 시퀀스를 하위 구간의 가중치 (`wi`)로 사용합니다.

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

예시:
```cpp
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res = piecewise_constant_distribution(100, intervals, weights);
```

#### 조각별 선형 분포

`piecewise_linear_distribution()` 함수는 `firstB`에서 정의된 범위의 값을 하위 구간 (`bi`)의 경계로, `firstW`에서 시작하는 시퀀스를 하위 구간 경계의 가중치 (`wi`)로 사용합니다.

`float` 또는 크기 `nb`의 `fvector`를 반환합니다.

예시:
```cpp
fvector intervals = [0.0, 4.5, 9.0];
fvector weights = [10.0, 0.0, 10.0];
fvector res = piecewise_linear_distribution(100, intervals, weights);
```