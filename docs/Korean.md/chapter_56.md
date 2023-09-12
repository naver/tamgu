# 라이브러리 wapiti: 텍스트 분류 및 개체 추출

**libwapiti**는 [http://wapiti.limsi.fr](http://wapiti.limsi.fr)에서 찾을 수 있는 wapiti 라이브러리의 캡슐화입니다.

© 2009-2013 CNRS. All rights reserved.

Wapiti는 태깅 및 개체 추출을 위한 CRF 방법의 효율적인 구현을 제공합니다. 시스템에 대한 자세한 정보가 필요한 경우 [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html)에서 매뉴얼을 참조하십시오.

## 사용법

라이브러리의 이름은 **wapiti**입니다. 다음 명령을 사용하여 사용할 수 있습니다: `use('wapiti');`

**libwapiti**는 다음 메서드를 노출합니다:

### 메서드

1. `loadmodel(string crfmodel)`: CRF 모델을 로드합니다.
2. `options(svector options)`: 옵션을 설정합니다. 사용 가능한 옵션에 대해서는 아래를 참조하십시오. 옵션은 wapiti의 명령 줄에서 사용하는 것과 동일한 방식으로 svector에 배치되어야 합니다.
3. `train()`: 훈련을 시작합니다. 미리 옵션을 설정해야 합니다.
4. `label(vector words)`: 토큰 벡터에 대한 라벨링을 시작합니다. 각 토큰에 대한 라벨의 벡터를 반환합니다.
5. `lasterror()`: 발생한 마지막 오류를 반환합니다.

### 옵션

Wapiti는 시스템에 내장된 모든 가능성을 다루기 위해 일부 옵션을 노출합니다. 아래는 이러한 옵션의 목록입니다. 이러한 옵션은 wapiti의 명령 줄 버전과 동일한 방식으로 svector로 제공되어야 합니다.

- 훈련 모드:
  - `train [options] [input data] [model file]`
  - `-s` 또는 `--score`: 출력에 점수 추가.
  - `-p` 또는 `--post`: 사후 확률을 사용하여 라벨링.
  - `-n` 또는 `--nbest INT`: n-최적 목록 출력.
  - `--force`: 강제 디코딩 사용.

## 훈련

CRF를 훈련시키기 위해서는 각 줄이 탭으로 구분된 태그와 함께 토큰이 있는 주석이 포함된 텍스트 파일이 필요합니다.

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

이 예시에서 각 줄은 두 가지 다른 태그와 연관된 토큰을 나타냅니다. 이 예시에서 "null" 태그는 이 특정 예시를 제외하고는 특정 해석이 없는 간단한 문자열입니다.

또한 CRF++의 매뉴얼(기반으로 한) 또는 [http://wapiti.limsi.fr/manual.html](http://wapiti.limsi.fr/manual.html)에 설명된대로 "패턴" 파일이 필요합니다.

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

다음은 훈련 파일과 패턴 파일을 입력으로 사용하여 개체에 라벨을 지정할 모델을 생성하는 예제 프로그램입니다.

```python
use('wapiti');
wapiti tst;

# 패턴 파일과 훈련 파일을 기반으로 모델을 생성할 것입니다
svector v = ["train", "-p", "pattern", "-1", "5", "training", "model"];
tst.options(v);
tst.train();
```

## 라벨링

라벨링은 `label` 메서드를 통해 수행됩니다. 이 메서드를 사용하려면 먼저 훈련을 통해 생성한 모델을 로드해야 합니다. 이 과정은 토큰 목록을 보내고 해당하는 라벨을 포함하는 동일한 크기의 벡터를 출력으로 받는 것으로 이루어집니다. 시스템에 토큰 목록을 제공해야 하며, 각각의 토큰은 특정 태그와 연관되어야 합니다.