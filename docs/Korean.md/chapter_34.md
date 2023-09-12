# 36 타입 호출하기 (Type call) 탐구

이 객체는 나중에 실행할 수 있는 함수를 추적하는 데 사용됩니다. 호출은 변수 이름을 함수로 사용하여 수행됩니다.

## 예제

```tamgu
function display(int e) {
    print("DISPLAY:", e, "\n");
    e += 10;
    return e;
}

call myfunc;
myfunc = display;
int i = myfunc(100); // 출력: DISPLAY:TEST
print("I=", i, "\n"); // 출력: I=110
```

이 예제에서 `display` 함수는 정수 `e`를 인수로 받습니다. `e`의 값을 "DISPLAY:" 접두사와 함께 출력하고, `e`에 10을 더한 후, 업데이트된 값을 반환합니다.

`call` 객체는 `myfunc` 변수를 저장하는 데 사용되며, `myfunc` 변수에 `display` 함수가 할당됩니다. 마지막으로, `myfunc` 변수가 인수 `100`과 함께 호출되고, 반환된 값이 `i` 변수에 저장됩니다. `i`와 `e`의 값이 출력됩니다.