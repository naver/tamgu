## 주석 규칙

주석 규칙은 본문과 연결된 라벨입니다. 본문은 선택적 요소, 요소의 분리, 정규 표현식, 건너뛴 단어 등과 같은 다양한 요소를 통합할 수 있습니다. 이는 원본 텍스트에서 라벨과 그 범위를 나타내는 오프셋으로 구성된 라벨 목록을 반환합니다.

### 출력

규칙이 토큰 시퀀스 또는 토큰에 적용되면 각 토큰은 해당 라벨과 연결됩니다.

다시 말해, 라벨은 클래스로 볼 수 있습니다. 규칙이 적용되면 초기 시퀀스에서 이 규칙에 의해 범위 지정된 토큰은 해당 규칙의 라벨 또는 클래스를 따라 분산됩니다.

#### 초기 시퀀스:
The chicken was delicious. The chicken was very soft.

#### 규칙:
@food <- chicken.
good <- #food, ?, delicious.

#### 측면은 이제 위의 규칙의 적용을 통해 이익을 얻을 수 있습니다.
good과 soft 사이의 토큰 수를 최대 세 개로 제한합니다...

aspect <- #good, ?+:3, soft.

규칙 "good"이 적용되면 "chicken" 토큰은 "good" 라벨을 받고, 이 새로운 라벨을 통해 후속 규칙에서 식별할 수 있습니다.

### 예시

선택적 요소, #food은 규칙 라벨이나 어휘 라벨과 일치합니다.

food ← (#positive), #food +.

단어를 건너뛸 수 있습니다: >..<

단어가 여러 라벨과 일치할 수 있도록 지정할 수도 있습니다.

service ← #personnel, >?*:5<, #[positive,service].

### 함수 콜백

규칙은 현재 토큰을 보다 복잡한 일치 유형을 통해 조사할 수 있는 함수를 호출할 수 있습니다. 예를 들어, 함수는 현재 토큰을 특정 어휘나 단어 임베딩과 비교할 수 있습니다.

함수는 적어도 두 개의 매개변수를 가져야 합니다. 첫 번째는 현재 토큰이고, 두 번째는 현재 주석 변수입니다. 그러나 필요한 경우 더 많은 변수를 제공할 수도 있습니다.

```python
function distance(string currenttoken, annotator a) {
    if (currenttoken.editdistance('is') <= 2)
        return true;
    return false;
}
```

label1 <- this, <distance>, a, thing.

이 규칙은 다음 발화에 대해 label1을 생성합니다:
- this is a thing
- this ist a thing
- this sis a thing

이 함수는 임의의 동작을 구현할 수 있습니다. 예를 들어, 토큰은 단어 임베딩이나 두 단어 간의 코사인 거리와 비교될 수 있습니다.

### 전역 규칙

"#" 또는 "~"로 시작하는 전역 규칙은 주석을 처리하기 위해 텍스트가 완전히 처리된 후에 적용되어 새로운 주석을 추가하거나 기존 주석을 제거하는 역할을 합니다.

```python
function test(annotator a) {
    if (a.word("bread"))
        return true;
    return false;
}

#label ← #lab1, #lab2, <test>.
```

예를 들어, 위의 규칙은 lab1과 lab2가 생성되었고 호출이 true를 반환하는 경우 주석 라벨을 생성합니다.

이 경우 호출은 텍스트와 독립적으로 적용되므로 매개변수가 하나뿐입니다.

### 코드 예시

어휘 목록...

단순히 "어휘 라벨"과 연결된 단어

@positive ← great.
@positive ← better.
@positive ← good.
@positive ← accommodating.

원형으로 단어를 비교합니다.

@negative ← $overrate.
@negative ← $disappoint.

다른 범주입니다.

@food ← sushi.

이것은 정규 표현식이며, "s"는 선택 사항입니다.
".."은 TAMGU 정규 표현식입니다.
'..'는 POSIX 정규 표현식입니다.

@food ← "food(s)".

이것은 여러 단어 표현식으로, 어휘와 결합하여 여러 단어의 토큰화를 수행합니다.

@food ← "lemon chicken".
@food ← "honey walnut prawn(s)".

@service ← accommodating.

@personnel ← staff.
@personnel ← personnel.
@personnel ← $waiter.