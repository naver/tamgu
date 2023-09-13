# 라이브러리 word2vec

**word2vec** 라이브러리는 Tamgu에서 word2vec 도구를 캡슐화합니다. word2vec에 대한 자세한 정보는 [Word2Vec 웹사이트](https://code.google.com/p/word2vec/)를 참조하십시오.

이 라이브러리를 사용하면 말뭉치에서 시스템을 훈련시키고 훈련된 모델을 사용하여 단어 간의 거리나 유추를 찾을 수 있습니다.

## 메소드

1. `accuracy(vector words, int threshold)`: 이 메소드는 단어 벡터의 정확도를 찾습니다. 빈도 맵(frequency map)을 반환합니다. 임계값이 제공되지 않으면 기본값은 30000입니다.
2. `analogy(svector words)`: 이 메소드는 단어 그룹에 대한 유추를 찾습니다. 빈도 맵을 반환합니다.
3. `distance(svector words)`: 이 메소드는 벡터 내의 단어 간의 거리를 찾습니다. 빈도 맵을 반환합니다.
4. `features()`: 이 메소드는 단어장과 해당 특성 값의 맵을 반환합니다.
5. `initialization(map m)`: 이 메소드는 word2vec 훈련 세트를 초기화합니다.
6. `loadmodel(string filename, bool normalize)`: 이 메소드는 word2vec 모델을 로드합니다.
7. `trainmodel(vector v)`: 이 메소드는 훈련을 시작합니다. 벡터가 제공되지 않으면 초기화 옵션에서 지정된 입력 파일을 사용합니다.
8. `vocabulary()`: 이 메소드는 훈련에 사용된 단어장의 이터레이터 맵을 반환합니다.

## 옵션

라이브러리의 옵션은 맵으로 제공됩니다. 이 옵션은 word2vec 도구에서 예상하는 옵션과 동일합니다.

예시 옵션:
```cpp
map options = {
    "train": "input.txt",
    "output": "output.txt",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};
```

이러한 옵션에 대한 자세한 설명은 Word2Vec 웹사이트에서 적절한 정보를 읽으십시오. 가장 중요한 옵션은 다음과 같습니다:
- "train": 이 옵션은 훈련 자료로 사용될 파일과 연결되어야 합니다.
- "output": 이 키의 값은 최종 훈련 모델이 저장될 출력 파일입니다.
- "window": 이 값은 주어진 토큰에 대한 적절한 문맥으로 고려할 단어의 수를 정의합니다.
- "threads": Word2Vec은 프로세스를 가속화하기 위해 스레드를 사용합니다. 시스템에서 사용할 스레드 수를 정의할 수 있습니다.
- "size": 이 값은 각 토큰에 연결된 벡터의 크기를 정의합니다.
- "iter": 이 값은 모델을 구축하기 위한 반복 횟수를 정의합니다.

이러한 옵션을 제공한 후 `initialization` 메소드를 호출하여 설정합니다.

예시:
```cpp
use("word2vec");
word2vec wrd;

// 주요 단어 주변에 5개의 단어가 있을 것입니다.
// 각 단어의 벡터 크기는 200입니다.
// 시스템은 20개의 스레드를 사용하여 15번의 반복으로 최종 모델을 계산합니다.
map options = {
    "train": "input.txt",
    "output": "output.bin",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};

wrd.initialization(options);
wrd.trainmodel();
```

## 사용법

훈련된 모델을 사용하려면 `loadmodel` 메소드를 사용하여 모델을 로드해야 합니다. 그 후에 `distance`, `analogy`, 또는 `accuracy` 메소드를 사용하여 단어 간의 거리, 유추 또는 정확도를 찾을 수 있습니다. 이러한 모든 메소드는 입력 벡터의 단어와의 거리를 가진 단어 목록을 반환합니다. 단어와 비교되는 어휘는 입력 문서에서 추출된 어휘입니다. `vocabulary` 메소드를 사용하여 이러한 단어에 모두 액세스할 수 있습니다.

예시:
```cpp
use("word2vec");
word2vec wrd;

// 훈련을 통해 얻은 모델을 로드합니다.
wrd.loadmodel("output.bin");

svector v = ["word"];
fmap res = wrd.distance(v);
```

## 타입 w2vector

입력 문서에서 추출된 각 단어는 "size" 옵션으로 훈련 시 정의된 특정 벡터와 연결됩니다. 예시에서는 크기가 200으로 설정됩니다.

훈련 어휘에서 특정 벡터를 추출하고 w2vector라는 특정 객체에 저장하는 것이 가능합니다.

### 메소드

1. `dot(element)`: 이 메소드는 두 단어 사이의 내적을 반환합니다. element는 문자열 또는 w2vector일 수 있습니다.
2. `cosine(element)`: 이 메소드는 두 단어 사이의 코사인 거리를 반환합니다. element는 문자열 또는 w2vector일 수 있습니다.
3. `distance(element)`: 이 메소드는 두 단어 사이의 거리를 반환합니다. element는 문자열 또는 w2vector일 수 있습니다.
4. `threshold(element)`: 이 메소드는 임계값을 반환하거나 설정합니다.
5. `norm(element)`: 이 메소드는 벡터의 노름을 반환합니다.

### 생성

w2vector 객체를 생성하려면 먼저 모델을 로드한 다음 토큰 문자열과 임계값을 모두 제공해야 합니다.

예시:
```cpp
use("word2vec");
w2vector wrd;

// 훈련을 통해 얻은 모델을 로드합니다.
wrd.loadmodel("output.bin");

w2vector w = wrd["tsunami": 0.5];
w2vector ww = wrd["earthquake": 0.5];
println(w.distance(ww));
```

임계값은 필수적이지 않습니다. 두 w2vector 요소를 비교하여 서로 가까운지 확인하는 데 사용됩니다. 임계값은 두 요소 간의 거리가 임계값보다 크거나 같은지 여부를 감지하는 데 사용됩니다.

다시 말해, `if (w == ww)`는 `if (w.distance(ww) >= w.threshold())`와 동일합니다.

예시:
```cpp
if (w == ww)
    println("ok");
if (w == "earthquake") // 간단한 문자열과 비교할 수 있습니다.
    println("ok");
```