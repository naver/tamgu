# 지속 변수

GUI 컨텍스트에서는 프로그램을 반복해서 실행할 때 특정 값을 추적할 수 있는 지속 변수를 생성할 수 있습니다. 사용 가능한 다양한 유형의 지속 변수가 있습니다: `ithrough`, `fthrough`, `uthrough`, `sthrough`, `vthrough`, 그리고 `mthrough`. 이러한 유형 중 하나로 변수를 선언하여 다른 실험을 추적할 수 있습니다. 이러한 변수들은 실행 사이에 다시 초기화되지 않습니다.

## 예시

```java
// 이 변수는 이 프로그램이 실행된 횟수를 추적합니다.
ithrough icount;
icount += 1;
println(icount);
```