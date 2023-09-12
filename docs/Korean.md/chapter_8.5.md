## 초기화 순서

아이템들이 프레임 내에서 선언될 때, `_initial` 함수 호출은 부모 프레임부터 시작하여 자식 프레임까지 위에서 아래로 진행됩니다.

또한, 프레임 F 내의 아이템이 프레임 F의 `_initial` 함수 내에서 인스턴스화되면, 이 선언은 다른 선언들보다 우선합니다.

### 예시

```java
// 두 개의 프레임을 선언합니다.
frame within {
    int i;
    
    // 특정 생성자 함수와 함께
    function _initial(int j) {
        i = j * 2;
        println("within _initial", i);
    }
}

// 그 프레임을 위한 _initial 함수...
function _initial(int k) {
    i = k;
    // 이전 설명을 새로운 것으로 대체합니다.
    // 이 선언은 위의 다른 선언을 대체합니다.
    w = within(100);
    println("test _initial", k);
}

// 초기값 20으로 테스트 인스턴스 t1을 생성합니다.
test t1(20);
```

### 실행

실행 결과는 다음과 같습니다:

```
test _initial 20
within _initial 200
```

이 예시에서 보듯이, `_initial`에서 "w"의 명시적 초기화는 "within w(i);" 선언을 대체하며, 이는 중복된 선언이 됩니다.