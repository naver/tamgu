# 36 타입 호출

이 객체는 함수를 추적하는 데 사용되며, 그 함수를 실행할 수 있습니다. 호출은 변수 이름을 함수로 사용하여 이루어집니다.

## 예제

```c++
int display(int e) {
    print("DISPLAY:", e, "\n");
    e += 10;
    return e;
}

int myfunc;
myfunc = display;
int i = myfunc(100);   // display: DISPLAY:100
print("I=", i, "\n");   // display: I=110
```

위 예제에서 `display` 함수는 정수 매개변수 `e`를 받아들이고, "DISPLAY:" 다음에 `e`의 값을 출력하고, `e`를 10 증가시킨 다음 업데이트된 `e`의 값을 반환합니다. 변수 `myfunc`에는 `display` 함수가 할당됩니다. 마지막으로, `myfunc` 변수는 인수 `100`으로 호출되고, 결과는 `i` 변수에 저장됩니다. 출력은 "DISPLAY:100"과 "I=110"이 될 것입니다.