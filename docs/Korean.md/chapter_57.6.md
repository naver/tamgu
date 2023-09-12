# 메소드

1. `accuracy(vector words, int threshold)`: 이 메소드는 네 개의 단어 벡터에 대한 정확도를 찾습니다. `fmap`을 반환합니다. 임계값이 제공되지 않으면 기본값은 30000입니다.

2. `analogy(vector words)`: 이 메소드는 단어 그룹에 대한 유추를 찾습니다. `fmap`을 반환합니다.

3. `distance(vector words)`: 이 메소드는 단어 벡터에서 거리를 찾습니다. `fmap`을 반환합니다.

4. `features()`: 이 메소드는 어휘와 그들의 특징 값들로 이루어진 맵을 반환합니다.

5. `initialization(map m)`: 이 메소드는 word2vec 훈련 세트를 초기화합니다.

6. `loadmodel(string filename, bool normalize)`: 이 메소드는 모델을 로드합니다.

7. `trainmodel(vector v)`: 이 메소드는 훈련을 시작합니다. `v`가 제공되지 않으면 시스템은 초기화 옵션에서 제공된 입력 파일을 사용합니다.

8. `vocabulary()`: 이 메소드는 훈련으로 커버되는 어휘의 `itreemap`을 반환합니다.