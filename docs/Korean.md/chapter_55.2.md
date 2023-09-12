## 방법

1. `cleandata()`: 내부 데이터를 정리합니다.
2. `crossvalidation()`: 새로운 매개변수로 교차 검증을 다시 실행합니다. 결과는 `fmap`입니다.
3. `loadmodel(string filename)`: 모델을 불러옵니다. 생성자로도 모델을 자동으로 불러올 수 있습니다.
4. `options(smap actions)`: 훈련 옵션을 설정합니다 (아래 참조).
5. `predict(fvector labels, vector data, bool predict_probability, bool infos)`: `treemapif` 벡터에서 예측합니다. `labels`는 선택 사항입니다. 제공되면 예측된 레이블을 `labels`에 저장된 대상 레이블과 비교하는 데 사용됩니다. `infos`가 true인 경우, 이 벡터의 첫 번째 요소는 정보 맵입니다.
6. `predictfromfile(string input, bool predict_probability, bool infos)`: 파일 `input`에서 예측합니다. 결과는 벡터입니다. `infos`가 true인 경우, 이 벡터의 첫 번째 요소는 정보 맵입니다.
7. `savemodel(string outputfilename)`: 모델을 파일에 저장합니다.
8. `trainingset(fvector labels, vector data)`: `treemapif` 벡터에서 훈련 세트를 생성합니다.
9. `train(string inputdata, smap options, string outputfilename)`: 옵션을 사용하여 훈련 데이터를 훈련합니다. `outputfilename`은 선택 사항입니다. 제공된 경우 최종 모델을 저장하는 데 사용됩니다 (이 메서드는 `load`라는 이름으로도 호출할 수 있습니다).