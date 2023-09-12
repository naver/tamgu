# Word2Vec 사용법

Word2Vec 모델을 사용하려면 다음 단계를 따르세요:

1. 적절한 훈련 데이터를 사용하여 모델을 생성하세요.
2. `loadmodel` 함수를 사용하여 모델을 로드하세요.
3. `distance`, `analogy`, 또는 `accuracy` 메서드를 사용하여 단어를 비교하고 그 거리를 검색하세요.
4. 위에서 언급한 메서드는 입력 벡터와의 거리를 포함한 단어 목록을 반환합니다.
5. 단어가 비교되는 어휘는 입력 문서에서 추출됩니다.
6. 어휘의 모든 단어에 액세스하려면 `vocabulary` 함수를 사용하세요.

## 예제

```python
from word2vec import Word2Vec

# Word2Vec의 인스턴스 생성
w2v = Word2Vec()

# 사전 훈련된 모델 로드
w2v.load_model("output.bin")

# 입력 벡터 정의
input_vector = ["단어"]

# 입력 벡터와 모델 내의 단어 사이의 거리 계산
result = w2v.distance(input_vector)
```

참고: 코드에서 `word2vec`을(를) 적절한 Word2Vec 클래스 이름으로 대체하세요.