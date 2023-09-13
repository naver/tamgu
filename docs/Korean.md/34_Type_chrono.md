# 시간 측정 타입 Chrono

`Chrono` 타입은 `Time`과 유사하지만, `std::chrono::high_resolution_clock`을 기반으로 구현되었습니다. 동일한 `reset` 메서드를 제공하지만, 더 정밀한 시간 계산이 가능합니다.

`Chrono` 객체를 생성할 때, 다음 플래그를 사용하여 기간을 처리하는 방법을 지정해야 합니다:
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

기본값은 `c_second`입니다.

## 예제

```cpp
Ring r;
Chrono c1(c_nanosecond);
for (int i = 0; i < 100000; i++) {
    r.pushLast(i);
}
Chrono c2(c_nanosecond);
float d;
d = c2 - c1; // 나노초 단위로 계산됨
println(d);
```

이 예제에서는 `Ring` 객체 `r`을 생성하고, `c_nanosecond` 기간으로 `Chrono` 객체 `c1`을 생성합니다. 그런 다음, 반복문을 실행하여 링에 값을 추가합니다. 그 후, 동일한 기간으로 다른 `Chrono` 객체 `c2`를 생성합니다. `c2`와 `c1`의 차이를 계산하여 `d`에 저장하고, 경과된 시간을 나노초 단위로 표시합니다. 마지막으로, `d`의 값을 출력합니다.