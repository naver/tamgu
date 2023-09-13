# 탐구에서의 특정 지침

탐구는 if, else, elif, switch, for, while 문을 포함한 다양한 알고리즘을 처리하기 위한 포괄적인 작업 집합을 제공합니다.

## 불리언 연산자

탐구는 다음과 같은 불리언 연산자를 지원합니다: and (&&), or (||) 및 xor. 이러한 연산자는 불리언 값을 가진 다른 요소를 결합하는 데 사용됩니다. and 및 or 연산자는 각각 && 및 ||로도 작성할 수 있습니다.

- `and`: 모든 요소가 true인 경우 성공합니다.
- `or`: 최소한 하나의 요소가 true인 경우 성공합니다.
- `xor`: 최소한 하나의 요소가 true인 경우 성공하지만, 모든 요소가 true인 경우 실패합니다.

## if-elif-else 문

탐구는 조건 분기를 위한 if-elif-else 문을 지원합니다. 구문은 다음과 같습니다:

```python
if (불리언_식) {
    // 코드 블록
}
elif (불리언_식) {
    // 코드 블록
}
...
else {
    // 코드 블록
}
```

## ifnot 문

`ifnot` 문은 여러 명령을 연결하고 false, null 또는 빈 값이 아닌 첫 번째 명령의 값을 반환하는 데 사용됩니다. 이는 복잡한 작업 시퀀스에 일반적으로 사용됩니다.

```python
result = call1() ifnot call2() ifnot call3()...
```

참고로, 복잡한 작업 시퀀스의 경우, 전체 `ifnot` 문 시퀀스는 괄호로 둘러싸여야 합니다.

## 예제:

```python
map m1 = { "a": 1, "b": 2, "c": 3 };
map m2 = { "A": 4, "B": 5, "C": 6 };
map m3 = { "aa": 7, "bb": 8, "cc": 9 };
int i = (m1["A"] ifnot m2["B"] ifnot m3["bb"]) + 24;
println(i); // 출력: 29

function calling(string s) {
    return (m1[s] ifnot m2[s] ifnot m3[s]);
}
println(calling('bb')); // 출력: 8
```

## switch 문

`switch` 문을 사용하면 단일 객체에 대한 일련의 테스트를 나열할 수 있습니다. 구문은 다음과 같습니다:

```python
switch (표현식) {
    v1: {
        // 코드 블록
    }
    v2: {
        // 코드 블록
    }
    default: {
        // 코드 블록
    }
}
```

`v1`, `v2` 및 `vn`은 문자열, 정수 또는 부동 소수점이 될 수 있습니다. `표현식`은 한 번 평가되고 값 `v1`, `v2`, `vn` 등과 비교됩니다. 간단한 요소 간의 단순한 비교 대신 함수를 사용할 수도 있습니다. 함수는 true 또는 false를 반환해야 합니다.

```python
// 예: 한 값이 다른 값보다 큰지 테스트하는 경우
function tst(int i, int j) {
    if (j >= i)
        return true;
    return false;
}

int s = 10;
// `tst` 함수를 통한 테스트
switch (s) with tst {
    1: println("1");
    2: println("2");
    20: println("20"); // 이것이 선택된 발생입니다
}
```