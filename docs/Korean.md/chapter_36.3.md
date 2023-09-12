## 분포

첫 번째 매개변수인 `nb`는 벡터로 반환되는 요소의 수를 정의합니다. `nb`가 비어있는 경우, 단일 값 (정수 또는 부동 소수점)이 반환됩니다. "=" 기호 뒤에 정의된 값은 기본값입니다.

### 랜덤 선택
`random_choice(int nb, vector v)`

Tamgu는 `v`의 값에 기반하여 값 목록을 반환하는 함수를 제공합니다. `random_choice` 함수는 각 값이 발생할 확률이 동일한 이산 분포를 기반으로 합니다.

**예시:**
```
random_choice(10, ["a", "b", "c"])
```
반환:
```
['c', 'a', 'b', 'a', 'b', 'c', 'b', 'c', 'a', 'a']
```

### 균일 분포
`uniform_int(int nb, int a = 0, int b = max_value)`

`a`와 `b` 사이의 닫힌 구간에서 균일하게 분포된 정수값 `i`를 생성합니다. 이는 이산 확률 함수에 따라 분포됩니다.

매개변수: `a`, `b`

### 기하 분포
`geometric_distribution(float p)`

`p`로 지정된 분포 매개변수를 채택하는 `geometric_distribution` 객체를 생성합니다.

매개변수: `p`

성공 확률. 각 생성된 값이 시뮬레이션하는 독립적인 베르누이 분포 실험에서의 성공 확률을 나타냅니다. 이 값은 0.0과 1.0 사이의 값이어야 합니다 (둘 다 포함).

### 포아송 분포
`poisson_distribution(float mean = 1)`

`mean`으로 지정된 분포 매개변수를 채택하는 `poisson_distribution` 객체를 생성합니다.

매개변수: `mean`

구간 내 예상 이벤트 수 (μ). 이는 평균적으로 관찰되는 이벤트의 발생률을 나타냅니다. 이 값은 양수여야 합니다 (μ > 0).

### 지수 분포
`exponential_distribution(float lambda = 1)`

`lambda`로 지정된 분포 매개변수를 채택하는 `exponential_distribution` 객체를 생성합니다.

매개변수: `lambda`

발생률의 평균 (λ). 이는 평균적으로 구간마다 랜덤 이벤트가 관찰되는 횟수를 나타냅니다. 이 값은 양수여야 합니다 (λ > 0).

### 정규 분포
`normal_distribution(float mean = 0, float stddev = 1)`

`mean`과 `stddev` 또는 `parm` 객체로 지정된 분포 매개변수를 채택하는 `normal_distribution` 객체를 생성합니다.

매개변수: `mean`, `stddev`

- `mean`: 분포의 평균 (기대값, μ), 최고점의 위치와 일치합니다.
- `stddev`: 표준 편차 (σ), 분포 평균으로부터 값들의 분산을 나타냅니다. 이 값은 양수여야 합니다 (σ > 0).

### 로그 정규 분포
`lognormal_distribution(float m = 0, float s = 1)`

`m`과 `s`로 지정된 분포 매개변수를 채택하는 `lognormal_distribution` 객체를 생성합니다.

매개변수: `m`, `s`

- `m`: 이 분포의 가능한 값들의 로그 변환으로 형성된 기본 정규 분포의 평균입니다.
- `s`: 이 분포의 가능한 값들의 로그 변환으로 형성된 기본 정규 분포의 표준 편차입니다. 이 값은 양수여야 합니다 (s > 0).

### 카이 제곱 분포
`chi_squared_distribution(float n = 1)`

`n`으로 지정된 분포 매개변수를 채택하는 `chi_squared_distribution` 객체를 생성합니다.

매개변수: `n`

자유도의 수. 이 값은 양수여야 합니다 (n > 0).

### 샘플링 분포
`discrete_distribution(ivector il)`

리스트 `il`의 순서는 초기화 리스트의 크기인 (n - 1)까지의 각 정수 값에 대한 가중치로 사용됩니다.

**예시:**
```
ivector iv = discrete_distribution(10, [40, 10, 10, 40]);
```

`piecewise_constant_distribution(fvector firstb, fvector firstw)`

`firstB` 내에 정의된 범위의 값은 하위 구간 (bi)의 경계로 사용되고, `firstW`에서 시작하는 순서열은 각 하위 구간의 가중치 (wi)로 사용됩니다.

**예시:**
```
fvector intervals = [0.0, 2.0, 4.0, 6.0, 8.0, 10.0];
fvector weights = [2.0, 1.0, 2.0, 1.0, 2.0];
fvector res;
res = piecewise_constant_distribution(100, intervals, weights);
```

`piecewise_linear_distribution(fvector firstb, fvector firstw)`

`firstB` 내에 정의된 범위의 값은 하위 구간 (bi)의 경계로 사용되고, `firstW`에서 시작하는 순서열은 각 하위 구간 경계의 가중치 (wi)로 사용됩니다.

**예시:**