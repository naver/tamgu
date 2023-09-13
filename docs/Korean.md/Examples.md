## 예제
### 훈련 예제
```python
use("linear")
# 라이브러리를 불러옵니다.
use("liblinear")
trainFile = "output.dat"
# liblinear 변수를 선언합니다.
train = liblinear()
# 옵션을 설정합니다.
options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01}
# 모델을 로드하고, 훈련 결과를 model_test 파일에 저장합니다.
train.load(trainFile, options, "model_test")
```

### 예측 예제
```python
use("linear")
# 입력 파일
testFile = "trainData.dat"
# 모델을 가진 liblinear 변수를 선언합니다. (loadmodel을 사용할 수도 있습니다.)
```