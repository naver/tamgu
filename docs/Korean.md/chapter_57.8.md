# 사용법

만들어진 모델을 사용하려면 다음 단계를 따르면 됩니다:

1. `loadmodel` 함수를 사용하여 모델을 로드합니다.
2. `distance`, `analogy`, 또는 `accuracy` 중 하나의 메소드를 선택합니다.
3. 각각의 메소드는 입력 벡터와의 거리에 따라 단어 목록을 반환합니다.
4. 비교에 사용되는 어휘는 입력 문서에서 추출됩니다.
5. `vocabulary` 함수를 사용하여 어휘의 모든 단어에 접근할 수 있습니다.

다음은 예시입니다:

```python
import word2vec

# word2vec 모델 로드
model = word2vec.loadmodel("output.bin")

# 입력 벡터 생성
input_vector = ["단어"]

# 입력 벡터와 모델의 단어들 사이의 거리 계산
result = model.distance(input_vector)
```

참고: `"output.bin"`을 훈련된 모델 파일의 경로로 대체하세요.