## 메소드

1. `accuracy(vector words, int threshold)`: 여러 번 발생할 수 있는 단어 벡터에 대한 정확도를 찾습니다. fmap을 반환합니다. 임계값이 제공되지 않으면 기본값은 30000입니다.

2. `analogy(svector words)`: 단어 그룹에 대한 유추를 찾습니다. fmap을 반환합니다.

3. `distance(svector words)`: 단어 벡터에서 거리를 찾습니다. fmap을 반환합니다.

4. `features()`: 특징 값과 함께 어휘의 맵을 반환합니다.

5. `initialization(map m)`: word2vec 훈련 세트를 초기화합니다.

6. `loadmodel(string filename, bool normalize)`: 모델을 로드합니다.

7. `trainmodel(vector v)`: 훈련을 시작합니다. `v`가 제공되지 않으면 시스템은 초기화 옵션에서 제공된 입력 파일을 사용합니다.

8. `vocabulary()`: 훈련에 포함된 어휘의 itreemap을 반환합니다.