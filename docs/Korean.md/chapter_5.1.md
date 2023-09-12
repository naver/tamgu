# Tamgu에서의 함수, Autorun 및 스레드

Tamgu에서 함수는 `function` 키워드를 사용하여 선언되며, 이름과 매개변수를 따릅니다. 함수에서 값을 반환하려면 `return` 키워드를 사용합니다. 매개변수는 항상 값에 의해 전달되며, 타입이 `self`인 경우에만 참조에 의해 전달됩니다. Tamgu에서 함수는 반환 값의 타입을 지정하지 않는다는 점에 유의해야 합니다.

## Autorun

Tamgu는 autorun 개념도 지원합니다. 이는 프로그램이 시작될 때 함수가 자동으로 실행될 수 있음을 의미합니다. Autorun 함수를 정의하려면 `autorun`이라는 이름으로 함수를 선언하면 됩니다. 이 함수는 프로그램의 시작 부분에서 자동으로 실행됩니다.

## 스레드

Tamgu는 스레드를 통해 멀티스레딩을 지원합니다. 스레드를 사용하면 프로그램의 여러 부분을 동시에 실행할 수 있습니다. 스레드를 생성하려면 `thread` 키워드를 사용하고, 그 뒤에 함수 이름과 필요한 매개변수를 지정하면 됩니다. 스레드는 주 프로그램과 동시에 지정된 함수를 실행합니다.

예시:

```tamgu
function myFunction(param1, param2) {
    // 함수 본문
    return result;
}

autorun() {
    // autorun 함수 본문
}

thread myThread(myFunction, param1, param2);
```

위의 예시에서 `myFunction`은 언제든지 프로그램에서 호출할 수 있는 일반 함수입니다. `autorun` 함수는 프로그램이 시작될 때 자동으로 실행됩니다. `myThread` 스레드는 주 프로그램과 동시에 `myFunction`을 실행합니다.

Tamgu에서 함수, autorun 및 스레드를 사용하여 더 다양하고 효율적인 프로그램을 만들 수 있습니다.