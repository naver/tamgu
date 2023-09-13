# 탐구에서의 Chrono

탐구의 `chrono` 타입은 `time`과 유사하지만 `std::chrono::high_resolution_clock`을 기반으로 구현되었습니다. 이는 동일한 `reset` 메서드를 제공하지만 더 정확한 시간 계산을 제공합니다.

`chrono` 객체를 생성할 때, 다음 플래그를 사용하여 지속 시간 처리를 지정해야 합니다:
- `c_second`
- `c_millisecond`
- `c_microsecond`
- `c_nanosecond`

기본값은 `c_second`입니다.

예제 사용법:

```cpp
ring r;
chrono c1(c_nanosecond);

for (int i = 0; i < 100000; ++i) {
    r.pushlast(i);
}

chrono c2(c_nanosecond);
float d;
d = c2 - c1; // 나노초 단위로 표시됨

println(d);
```

이 예제에서 `chrono` 객체 `c1`은 지속 시간 처리가 `c_nanosecond`로 설정되어 생성됩니다. 그런 다음 `ring` `r`에 정수가 채워집니다. 같은 지속 시간 처리로 다른 `chrono` 객체 `c2`가 생성됩니다. `c2`와 `c1`의 차이가 계산되어 `d`에 저장되며, 이는 나노초 단위로 경과한 시간을 나타냅니다. 마지막으로, `d`가 출력됩니다.