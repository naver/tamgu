# w2vector

`w2vector` 클래스는 입력 문서에서 추출된 각 단어를 특정 벡터와 연결하는 데 사용됩니다. 벡터의 크기는 훈련 중에 결정되며 `size` 옵션을 사용하여 설정할 수 있습니다. 이 예제에서 크기는 200으로 설정됩니다.

훈련 어휘에서 특정 벡터를 추출하여 `w2vector` 객체에 저장하는 것이 가능합니다.

## 메서드

1. `dot(element)`: 두 단어 사이의 내적을 반환합니다. `element` 매개변수는 문자열이나 `w2vector` 객체 일 수 있습니다.
2. `cosine(element)`: 두 단어 사이의 코사인 거리를 반환합니다. `element` 매개변수는 문자열이나 `w2vector` 객체 일 수 있습니다.
3. `distance(element)`: 두 단어 사이의 거리를 반환합니다. `element` 매개변수는 문자열이나 `w2vector` 객체 일 수 있습니다.
4. `threshold(element)`: 임계값을 반환하거나 설정합니다.
5. `norm(element)`: 벡터의 노름을 반환합니다.

## 생성

`w2vector` 객체를 초기화하려면 먼저 모델을 로드하고 토큰 문자열과 임계값을 제공해야 합니다.

예제:

```python
use("word2vec")
wrd = w2vector()
# 훈련을 통해 얻은 모델 로드
wrd.loadmodel("output.bin")
w = wrd["tsunami": 0.5]
ww = wrd["earthquake": 0.5]
println(w.distance(ww))
```

임계값은 필수가 아닙니다. 두 개의 `w2vector` 요소를 비교하여 서로 가까운지 여부를 결정하는 데 사용됩니다. 임계값은 두 요소 사이의 거리가 임계값보다 큰지 여부를 감지하는 데 사용됩니다.

다시 말해, `if (w == ww)`는 `if (w.distance(ww) >= w.threshold())`와 동일합니다.

예제:

```python
if (w == ww)
    println("ok")
if (w == "earthquake")  # 간단한 문자열과 비교할 수 있습니다.
    println("ok")
```

## fvector

`w2vector`에서 내부 float 벡터를 검색하는 것도 가능합니다.

예제:

```python
fvector vvv = w
vvv is:
[0.049775, -0.0498451, -0.0722533, 0.0536649, -0.000515156, -0.0947062, 0.0294775, -0.0146792, -0.100351, 0.0480318, 0.071128, 0.0268629...]
```