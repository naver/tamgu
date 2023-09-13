# 라이브러리 LINEAR : 선형 타입

Tamgu는 분류기를 구현하기 위해 사용되는 liblinear 라이브러리의 캡슐화를 제공합니다. 이 라이브러리에 대한 자세한 정보는 [http://www.csie.ntu.edu.tw/~cjlin/liblinear/](http://www.csie.ntu.edu.tw/~cjlin/liblinear/)를 참조하십시오.

## 메소드

이 라이브러리는 다음 메소드를 노출합니다:

1. `cleandata()`: 내부 데이터를 정리합니다.
2. `crossvalidation()`: 새로운 매개변수로 교차 검증을 다시 실행합니다. 결과는 fmap입니다.
3. `loadmodel(string filename)`: 모델을 로드합니다. 모델은 생성자로 자동으로 로드될 수도 있습니다.
4. `options(smap actions)`: 훈련 옵션을 설정합니다 (아래 참조).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: treemapif 벡터에서 예측합니다. `labels` 매개변수는 선택적입니다. 제공되면 이는 `labels`에 저장된 목표 레이블과 예측된 레이블을 테스트하는 데 사용됩니다. `infos`가 true이면, 이 벡터의 첫 번째 요소는 정보 맵입니다.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: 파일 입력에서 예측합니다. 결과는 벡터입니다. `infos`가 true이면, 이 벡터의 첫 번째 요소는 정보 맵입니다.
7. `savemodel(string outputfilename)`: 모델을 파일에 저장합니다.
8. `trainingset(fvector labels, vector data)`: treemapif 벡터에서 훈련 세트를 생성합니다.
9. `train(string inputdata, smap options, string outputfilename)`: 옵션을 사용하여 훈련 데이터를 훈련합니다. `outputfilename`은 선택 사항입니다. 제공된 경우 최종 모델을 저장하는 데 사용됩니다 (이 메소드는 `load`라는 이름으로도 호출할 수 있습니다).

## 훈련 옵션

훈련 옵션은 다음 키를 사용하여 smap으로 제공해야 합니다: `s`, `e`, `c`, `p`, `B`, `wi`, `M`, `v`.

1. `s` type: 솔버 유형을 설정합니다 (기본값 1). 가능한 값은 다음과 같습니다:
   - 다중 클래스 분류의 경우:
     - 0: L2 정규화 로지스틱 회귀 (원시)
     - 1: L2 정규화 L2 손실 서포트 벡터 분류 (듀얼)
     - 2: L2 정규화 L2 손실 서포트 벡터 분류 (원시)
     - 3: L2 정규화 L1 손실 서포트 벡터 분류 (듀얼)
     - 4: Crammer와 Singer에 의한 서포트 벡터 분류
     - 5: L1 정규화 L2 손실 서포트 벡터 분류
     - 6: L1 정규화 로지스틱 회귀
     - 7: L2 정규화 로지스틱 회귀 (듀얼)
   - 회귀의 경우:
     - 11: L2 정규화 L2 손실 서포트 벡터 회귀 (원시)
     - 12: L2 정규화 L2 손실 서포트 벡터 회귀 (듀얼)
     - 13: L2 정규화 L1 손실 서포트 벡터 회귀 (듀얼)
2. `c` cost: 매개변수 C를 설정합니다 (기본값 1).
3. `p` epsilon: SVR 손실 함수의 epsilon을 설정합니다 (기본값 0.1).
4. `e` epsilon: 종료 기준의 허용 오차를 설정합니다. 값은 유형 `s`에 따라 다릅니다:
   - `s` 0 및 2의 경우: `|f'(w)|_2 <= eps*min(pos,neg)/l*|f'(w0)|_2`, 여기서 `f`는 원시 함수이고 `pos/neg`는 양수/음수 데이터의 수입니다 (기본값 0.01).
   - `s` 11의 경우: `|f'(w)|_2 <= eps*|f'(w0)|_2` (기본값 0.001).
   - `s` 1, 3, 4 및 7의 경우: 듀얼 최대 위반 <= eps; libsvm과 유사 (기본값 0.1).
   - `s` 5 및 6의 경우: `|f'(w)|_1 <= eps*min(pos,neg)/l*|f'(w0)|_1`, 여기서 `f`는 원시 함수입니다 (기본값 0.01).
   - `s` 12 및 13의 경우: `|f'(alpha)|_1 <= eps |f'(alpha0)|`, 여기서 `f`는 듀얼 함수입니다 (기본값 0.1).
5. `B` bias: bias >= 0이면 인스턴스 x는 [x; bias]가 됩니다. < 0이면 바이어스 항이 추가되지 않습니다 (기본값 -1).
6. `wi` weight: 가중치는 다른 클래스의 매개변수 C를 조정합니다. 여기서 'i'는 인덱스를 나타냅니다. 키는 예를 들어 "w10"일 수 있습니다.
7. `M` type: 다중 클래스 분류의 유형을 설정합니다 (기본값 0). 가능한 값은 다음과 같습니다:
   - `M` 0: one-versus-all
   - `M` 1: one-versus-one
8. `v` n: n-fold 교차 검증 모드.

다음 문자열을 정수 대신 사용할 수 있습니다:
- "L2R_LR"은 0입니다.
- "L2R_L2LOSS_SVC_DUAL"은 1입니다.
- "L2R_L2LOSS_SVC"은 2입니다.
- "L2R_L1LOSS_SVC_DUAL"은 3입니다.
- "MCSVM_CS"은 4입니다.
- "L1R_L2LOSS_SVC"은 5입니다.
- "L1R_LR"은 6입니다.
- "L2R_LR_DUAL"은 7입니다.
- "L2R_L2LOSS_SVR = 11"은 8입니다.
- "L2R_L2LOSS_SVR_DUAL"은 9입니다.
- "L2R_L1LOSS_SVR_DUAL"은 10입니다.

## 예제

### 훈련 예제

```cpp
use("linear");
// 라이브러리 로드
use("liblinear");

string trainFile = "output.dat";
// liblinear 변수 선언
liblinear train;
// 옵션 설정
map options = {"c": 100, "s": 'L2R_LR', "B": 1, "e": 0.01};
// 모델 로드 및 훈련 출력을 "model_test" 파일에 저장
train.load(trainFile, options, "model_test");
```

### 예측 예제

```cpp
use("linear");
// 입력 파일
string testFile = "trainData.dat";
// 모델을 가진 liblinear 변수 선언 (loadmodel을 사용할 수도 있음)
liblinear predict("model_test");
// 파일에서 예측 수행
vector result = predict.predictfromfile(testFile, true);
```