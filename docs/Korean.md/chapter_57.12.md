## 사용법

만들어진 모델을 사용하려면 다음 단계를 따르세요:

1. `loadmodel` 함수를 사용하여 모델을 로드합니다.
2. `distance`, `analogy`, 또는 `accuracy` 중 하나의 메소드를 사용합니다. 이러한 메소드는 입력 벡터의 단어와 해당 단어와의 거리를 포함한 단어 목록을 반환합니다.
3. 비교에 사용되는 어휘는 입력 문서에서 추출됩니다. `vocabulary` 함수를 사용하여 이 어휘에 액세스할 수 있습니다.

### 예제

```python
from word2vec import Word2Vec

# word2vec 모델 로드
w2v = Word2Vec()
w2v.loadmodel("output.bin")

# 입력 벡터 생성
input_vector = ["단어"]

# 입력 벡터와 모델 내 단어 간의 거리 계산
result = w2v.distance(input_vector)
```

위 예제에서는 먼저 "output.bin" 파일에서 word2vec 모델을 로드합니다. 그런 다음 "단어"라는 단어로 입력 벡터를 생성합니다. 마지막으로 `distance` 메소드를 사용하여 입력 벡터와 모델 내 단어 간의 거리를 계산합니다.