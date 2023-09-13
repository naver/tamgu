## 시스템 기능

시스템 기능은 Tamgu 언어의 다른 연산자를 프레임 메소드에 매핑하는 데 사용됩니다.

### 비교 함수

Tamgu는 다른 프레임 요소 간에 특정 비교 함수를 정의하는 데 도움이 되는 방법을 제공합니다. 이러한 함수는 특정한 이름을 가지고 있지만 다음 연산자에 의해 트리거됩니다: `>`, `<`, `==`, `!=`, `<=`, `>=`.

각 함수는 현재 요소와 비교되는 하나의 매개변수를 가지고 있습니다.

다음은 이러한 함수의 목록입니다:

1. `equal`: function ==(frame b);
2. `different`: function !=(frame b);
3. `inferior`: function <(frame b);
4. `superior`: function >(frame b);
5. `inferior equal`: function <=(frame b);
6. `superior equal`: function >=(frame b);

이러한 함수 각각은 테스트에 따라 true 또는 false를 반환해야 합니다.

예시:

```cpp
// 프레임에서 비교 연산자의 구현
frame comp {
    int k;
    // < 연산자를 구현합니다.
    function <(frame autre) {
        if (k < autre)
            return true;
        return false;
    }
}

// 두 개의 요소를 생성합니다.
comp one;
comp two;
// one은 10이고 two는 20입니다.
one.k = 10;
two.k = 20;
// one은 two보다 작으며 위의 inferior 메소드가 호출됩니다.
if (one < two)
    println("OK");
```

### 산술 함수

Tamgu는 다양한 숫자 연산자에 대한 특정 함수를 구현하는 메커니즘도 제공합니다. 이러한 함수는 인자로 받은 프레임과 동일한 프레임의 요소를 반환해야 합니다. `++`와 `--`를 제외한 나머지 함수는 두 개의 연산자를 가져야 합니다.

1. `plus`: function +(frame a, frame b);
2. `minus`: function -(frame a, frame b);
3. `multiply`: function *(frame a, frame b);
4. `divide`: function /(frame a, frame b);
5. `power`: function ^^(frame a, frame b);
6. `shift left`: function <<(frame a, frame b);
7. `shift right`: function >>(frame a, frame b);
8. `mod`: function %(frame a, frame b);
9. `or`: function |(frame a, frame b);
10. `xor`: function ^(frame a, frame b);
11. `and`: function &(frame a, frame b);
12. `++`: function ++();
13. `--`: function --();

예시:

```cpp
frame test {
    int k;
    function ++() {
        k++;
    }
    // 함수에서 반환되는 새로운 값을 생성하는 것이 중요합니다.
    function +(test a, test b) {
        test res;
        res.k = a.k + b.k;
        return res;
    }
}

test a, b, c;
c = a + b; // 위의 plus 구현이 호출됩니다.
```

### 숫자와 문자열 함수

프레임 객체를 문자열이나 숫자로 해석할 수도 있습니다. 이 경우, 프레임 인스턴스가 해석될 원하는 객체의 유형으로 이름을 가진 함수를 구현할 수 있습니다.

예시:

```cpp
frame test {
    int k;
    string s;
    // "test" 인스턴스를 문자열로 반환합니다.
    function string() {
        return s;
    }
    // "test" 인스턴스를 float으로 반환합니다.
    function float() {
        return k;
    }
    // "test" 인스턴스를 int로 반환합니다.
    function int() {
        return k;
    }
}

Nota bene: 숫자의 경우, 사용 가능한 유형의 수만큼 함수를 정의할 수 있다는 점에 유의하십시오: short, int, long, decimal, float. 그러나 처음 정의된 함수가 모든 유형에 대한 기본 숫자 함수가 됩니다. 특별한 함수 구현을 추가하지 않는 한, 위의 예시에서는 int를 제외한 모든 숫자 유형 해석이 기본적으로 float이 됩니다.

### 간격과 인덱스

프레임 객체를 벡터나 맵으로 사용할 수도 있습니다. 이 경우, 개발자는 다음 함수를 구현하여 간격을 통해 요소에 액세스하거나 인덱스를 통해 값을 설정할 수 있습니다.