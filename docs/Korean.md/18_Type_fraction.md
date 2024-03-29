# 분수 타입

Tamgu의 `Fraction` 타입은 사용자가 숫자를 분수로 다룰 수 있도록 해주며, 어떤 계산에서든 어디에서든 사용할 수 있습니다. 정수와 실수에 대한 모든 수학적인 메소드는 여전히 분수에 대해서도 유효하지만, 이 타입은 몇 가지 추가적인 특정 메소드를 제공합니다.

## 메소드

1. `d()`: 분수의 분모를 반환합니다.
2. `d(int v)`: 분수의 분모를 설정합니다.
3. `fraction f(int n, int d)`: 분자와 분모를 가지고 분수를 생성합니다. 기본적으로 분자는 0이고 분모는 1입니다.
4. `invert()`: 분수의 분모와 분자를 바꿉니다.
5. `n()`: 분수의 분자를 반환합니다.
6. `n(int v)`: 분수의 분자를 설정합니다.
7. `nd(int n, int d)`: 분수의 분자와 분모를 설정합니다.

## 변환

분수가 문자열, 정수 또는 실수로 사용될 때, Tamgu는 분수의 간단한 계산을 통해 적절한 실수나 정수를 자동으로 생성합니다. 그러나 이 변환은 정보의 손실을 가져올 수 있습니다. 또한, Tamgu는 분수를 가능한한 작게 유지하기 위해 각 단계에서 분수를 간단하게 만듭니다.

문자열로 사용될 때, Tamgu는 분수를 "NUM/DEN" 형식으로 반환합니다.

## 예제

```cpp
// 두 개의 분수를 생성합니다.
fraction f(10, 3);
fraction g(18, 10);

// g를 f에 더합니다...
f += g;

println(f); // 출력: 77/15
```

참고: 코드 예제는 Tamgu 프로그래밍 언어의 사용을 가정합니다.