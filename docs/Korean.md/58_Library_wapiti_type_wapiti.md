# 라이브러리 wapiti

libwapiti는 [http://wapiti.limsi.fr](http://wapiti.limsi.fr)에서 사용 가능한 wapiti 라이브러리의 캡슐화입니다.

저작권 (c) 2009-2013 CNRS. 판권 소유.

Wapiti는 태깅 및 개체 추출을 위한 CRF 방법의 효율적인 구현을 제공합니다. 시스템에 대한 자세한 정보가 필요한 경우 [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html)에서 매뉴얼을 참조하십시오.

## 사용법

라이브러리를 사용하려면 wapiti 모듈을 로드해야 합니다:

```python
use('wapiti')
```

라이브러리는 다음과 같은 메서드를 노출합니다:

### 메서드

1. `loadmodel(string crfmodel)`: CRF 모델을 로드합니다.
2. `options(svector options)`: 옵션을 설정합니다. 사용 가능한 옵션에 대해서는 아래를 참조하십시오. 옵션은 wapiti의 명령 줄에서 사용되는 것과 동일한 방식으로 svector에 배치되어야 합니다.
3. `train()`: 훈련을 시작합니다. 미리 옵션을 설정해야 합니다.
4. `label(vector words)`: 토큰 벡터에 대한 라벨링을 시작합니다. 각 토큰에 대한 라벨의 벡터를 반환합니다.
5. `lasterror()`: 발생한 마지막 오류를 반환합니다.

### 옵션

Wapiti는 시스템에 내재된 모든 가능성을 다루기 위해 몇 가지 옵션을 노출합니다. 아래는 이러한 옵션의 목록입니다. 이러한 옵션은 wapiti의 명령 줄 버전과 동일한 방식으로 svector로 제공되어야 합니다.

#### 훈련 모드:

- `train [options] [input data] [model file]`
  - `--me`: maxent 모드 강제
  - `--type STRING`: 훈련할 모델의 유형
  - `--algo STRING`: 사용할 훈련 알고리즘
  - `--pattern FILE`: 특징 추출을 위한 패턴
  - `--model FILE`: 미리 로드할 모델 파일
  - `--devel FILE`: 개발 데이터 세트
  - `--rstate FILE`: 복원할 옵티마이저 상태
  - `--sstate FILE`: 저장할 옵티마이저 상태
  - `--compact`: 훈련 후 모델을 압축
  - `--nthread INT`: 작업자 스레드 수
  - `--jobsize INT`: 작업자 스레드의 작업 크기
  - `--sparse`: 희소 전/후진을 활성화
  - `--maxiter INT`: 최대 반복 횟수
  - `--rho1 FLOAT`: L1 패널티 매개변수
  - `--rho2 FLOAT`: L2 패널티 매개변수
  - `--objwin INT`: 수렴 창 크기
  - `--stopwin INT`: 중지 창 크기
  - `--stopeps FLOAT`: 중지 엡실론 값
  - `--clip`: 그라디언트 클리핑 (l-bfgs)
  - `--histsz INT`: 히스토리 크기 (l-bfgs)
  - `--maxls INT`: 최대 라인 서치 반복 횟수 (l-bfgs)
  - `--eta0 FLOAT`: 학습률 (sgd-l1)
  - `--alpha FLOAT`: 지수 감소 매개변수 (sgd-l1)
  - `--kappa FLOAT`: 안정성 매개변수 (bcd)
  - `--stpmin FLOAT`: 최소 스텝 크기 (rprop)
  - `--stpmax FLOAT`: 최대 스텝 크기 (rprop)
  - `--stpinc FLOAT`: 스텝 증분 계수 (rprop)
  - `--stpdec FLOAT`: 스텝 감소 계수 (rprop)
  - `--cutoff`: 대체 투영 (rprop)

#### 라벨 모드:

- `label [options] [input data] [output data]`
  - `--me`: maxent 모드 강제
  - `--model FILE`: 로드할 모델 파일
  - `--label`: 라벨만 출력
  - `--check`: 입력이 이미 라벨링됨
  - `--score`: 출력에 점수 추가
  - `--post`: 사후 확률을 사용하여 라벨링
  - `--nbest INT`: n개의 최상위 리스트 출력
  - `--force`: 강제 디코딩 사용

## 훈련

CRF를 훈련시키려면 각 줄이 탭으로 구분된 태그를 가진 토큰인 주석이 포함된 텍스트 파일이 필요합니다.

예시:

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

이 예시에서 각 줄은 두 가지 다른 태그와 연관된 토큰을 나타냅니다. 이 예시에서 "null" 태그는 이 특정 예시를 제외하고는 특정 해석이 없는 단순한 문자열입니다.

또한 CRF++의 매뉴얼 또는 [Wapiti 매뉴얼](http://wapiti.limsi.fr/manual.html)에 설명된대로 구현되어야 하는 "pattern" 파일이 필요합니다.

예시 패턴 파일:

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

## 프로그램

다음은 라이브러리를 사용하여 모델을 훈련하고 개체에 라벨을 지정하는 작은 프로그램입니다.

```python
use('wapiti')

wapiti tst

# 모델 로드
tst.loadmodel("model")

# 토큰 벡터에 라벨 지정
words = ['Growth NOUN', '& CONJ', 'Income NOUN', 'Fund NOUN', '( PUNCT', 'Exact ADJ', 'name NOUN', 'of PREP', 'registrant NOUN', 'as PREP', 'specified ADJ', 'in PREP', 'charter NOUN']
res = tst.label(words)

# 라벨 목록 출력
println(res)
```

결과는 `['ORGANISATION_b', 'ORGANISATION_i', 'ORGANISATION_i', 'ORGANISATION_i', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null', 'null']`입니다.