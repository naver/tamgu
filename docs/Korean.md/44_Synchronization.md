# 동기화

Tamgu는 스레드를 대기 상태로 만드는 간단한 방법을 제공합니다. 이 과정은 구현하기 매우 간단합니다. Tamgu는 다음과 같은 목적으로 다양한 함수를 제공합니다.

## 메소드

1. `cast(string)`: 지정된 문자열에서 대기 중인 모든 스레드의 실행을 해제합니다.
2. `cast()`: 문자열 상태와 관계없이 모든 스레드를 해제합니다.
3. `lock(string s)`: 코드 일부에 잠금을 설정하여 두 개의 스레드가 동시에 동일한 줄에 액세스하는 것을 방지합니다.
4. `unlock(string s)`: 잠금을 해제하여 다른 스레드가 함수의 내용에 액세스할 수 있도록 합니다.
5. `waitonfalse(var)`: 변수 `var`의 값이 false(또는 0 또는 false를 반환하는 값)로 설정될 때까지 스레드를 대기 상태로 만듭니다.
6. `waitonjoined()`: 현재 스레드 내에서 시작된 스레드가 종료될 때까지 기다립니다. 이러한 스레드는 `join` 플래그로 선언되어야 합니다.
7. `wait(string)`: 문자열을 트리거로 사용하여 스레드를 대기 상태로 만듭니다. `cast`가 해당 문자열에 대해 수행되면 대기 모드가 해제됩니다.

### 예제 1

```java
// "test" 문자열을 트리거로 사용합니다.
joined thread waiting() {
    wait("test");
    println("Released");
}

// 일부 작업을 수행한 다음 대기 중인 스레드를 해제합니다.
joined thread counting() {
    int nb = 0;
    while (nb < 10000)
        nb++;
    cast("test");
    println("End counting");
}

waiting();
counting();
waitonjoined();
println("Out");
```

실행 결과:

위의 프로그램을 실행하면 Tamgu는 다음과 같은 순서로 출력합니다:

```
End counting
Released
Out
```

### 예제 2

```java
int nb = 1000;

joined thread decompte() {
    while (nb > 1) {
        nb--;
    }
    printlnerr("End counting", nb);
    nb--;
}

joined thread attend() {
    waitonfalse(nb);
    printlnerr("Ok");
}

attend();
decompte();
waitonjoined();
printlnerr("End");
```

### 뮤텍스: lock과 unlock

특정 스레드가 동시에 동일한 줄에 액세스하지 못하도록 막는 것이 필요한 경우가 있습니다. 예를 들어, 다른 스레드가 제어를 가져가기 전에 두 개의 함수 호출이 완전히 적용되도록 보장해야 할 때입니다. 함수 내에서 잠금이 설정되면 해당 함수의 다음 줄은 잠금이 해제될 때까지 다른 스레드가 액세스할 수 없습니다.

예제:

```java
// 스레드를 구현합니다.
thread launch(string n, int m) {
    int i;
    println(n);
    // 모든 값을 표시합니다.
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // 스레드를 시작합니다.
    launch("Premier", 2);
    launch("Second", 4);
}
```

실행하면 스레드가 커널에서 알려진 순서로 실행되기 때문에 상당히 무작위한 출력을 얻습니다.

```
Premier
Second
0 1
0 1 2 3
```

이러한 순서는 동일한 코드 묶음을 커널에서 동시에 실행하지 못하도록 잠그는 잠금을 사용하여 강제로 설정할 수 있습니다.

시스템이 끔찍한 출력으로 줄을 병합하는 것을 방지하기 위해 코드에 잠금을 추가해야 합니다:

```java
// 잠금이 포함된 스레드를 다시 구현합니다.
thread launch(string n, int m) {
    lock("launch"); // 여기에서 잠금을 설정합니다. 더 이상 통과할 수 없습니다.
    int i;
    println(n);
    // 모든 값을 표시합니다.
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
    unlock("launch"); // 동일한 문자열로 잠금을 해제하여 통과를 허용합니다.
}
```

그런 다음 이 코드 조각을 다시 실행하면 기대하는 대로 완전히 다른 출력이 생성됩니다:

```
Premier
0 1
Second
0 1 2 3
```

이번에는 코드에서의 순서대로 줄이 표시됩니다.

중요: 잠금 문자열은 전체 코드에 대해 전역적으로 유지되므로 어디에서든 잠금을 해제할 수 있습니다. 또한 특정 문자열에 대한 잠금은 해당 문자열을 사용하여 자신의 줄을 잠그는 다른 코드 부분을 차단할 수 있습니다. 따라서 서로 다른 잠금을 구분하기 위해 매우 구체적인 문자열을 사용하는 것이 좋습니다.

### 보호된 스레드

위의 예제는 보호된 함수를 사용하여 정확히 동일한 동작을하는 방식으로 다시 작성될 수 있습니다.

```java
// 보호된 함수로 스레드를 다시 구현합니다.
protected thread launch(string n, int m) {
    int i;
    println(n);
    // 모든 값을 표시합니다.
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}
```

이 함수는 위의 함수와 정확히 동일한 출력을 생성합니다. 보호된 스레드는 실행이 시작될 때 매우 처음에 잠금을 구현하고 함수가 종료될 때 잠금을 해제합니다. 그러나 잠금 대신 보호된 함수를 사용하는 장점은 보호해야 할 특정 줄을 훨씬 더 정확하게 지정할 수 있다는 것입니다.

### 세마포어: waitonfalse

위의 함수들은 멀티 스레드 환경에서 유용하지만, 일부 경우에는 충분하지 않을 수 있습니다. Tamgu는 스레드를 변수 값에 동기화하는 데 사용되는 함수를 제공합니다. 이러한 함수는 부울, 정수, 부동 소수점 또는 문자열과 같은 간단한 유형과만 연관될 수 있습니다.

이 두 함수의 역할은 스레드가 특정 변수가 false 값을 도달할 때까지 대기 상태에 있도록 만드는 것입니다. 숫자 변수가 0인 경우, 문자열이 비어 있는 경우 또는 부울 변수가 false로 설정되는 경우에는 자동으로 false가 반환됩니다.

`waitonfalse(var)`: 변수 `var`가 false 값을 도달할 때까지 스레드를 대기 상태로 만듭니다.

예제:

```java
// 먼저 stopby 변수를 선언합니다.
// 중요: 초기 값은 0과 다른 값이어야 합니다.
int stopby = 1;

// 스레드를 구현합니다.
thread launch(int m) {
    // stopby를 루프 수로 재설정합니다.
    stopby = m;
    int i;
    // 모든 값을 표시합니다.
    for (i = 0; i < m; i++) {
        print(i, " ");
        // stopby 변수를 감소시킵니다.
        stopby--;
    }
}

function principal() {
    // 스레드를 시작합니다.
    launch(10);
    // stopby가 0이 될 때까지 기다립니다...
    waitonfalse(stopby);
    println("End");
}

principal();
```

실행 결과, "End"가 화면에 모든 `i`가 출력될 때까지 표시되지 않습니다.

```
0 1 2 3 4 5 6 7 8 9 End
```

`waitonfalse`를 제거하면 출력은 다소 다릅니다:

```
End 0 1 2 3 4 5 6 7 8 9
```

이 예제에서 볼 수 있듯이 `waitonfalse`는 `principal`과 `launch`를 함께 동기화합니다.

참고: 위의 예제는 다음과 같이 `wait`와 `cast`를 사용하여 구현할 수 있습니다:

```java
// 스레드를 구현합니다.
thread launch(int m) {
    int i;
    // 모든 값을 표시합니다.
    for (i = 0; i < m; i++)
        print(i, " ");
    cast("end");
}

function principal() {
    // 스레드를 시작합니다.
    launch(10);
    wait("end");
    println("End");
}

principal();
```

그러나 한 번에 하나의 `cast`만 스레드를 해제할 수 있다는 점을 기억해야 합니다. 동기식 변수의 경우 `waitonfalse`는 `cast`를 수행할 수 있는 스레드뿐만 아니라 다른 스레드에 의해 트리거될 수 있습니다.

### `waitonjoined()`와 `join` 플래그

스레드가 계속하기 전에 다른 스레드의 완료를 기다려야 하는 경우, 가장 간단한 해결책은 각 스레드를 `join`으로 선언한 다음 `waitonjoined()` 메소드를 사용하는 것입니다.

다른 스레드는 동시에 다른 집합의 결합된 스레드에 대해 대기할 수 있습니다.

예제:

```java
// 첫 번째 스레드와 결합된 스레드
join thread jdisplay(string s) {
    print(s + "\r");
}

// 이 스레드에서도 "join"으로 시작됩니다.
join thread launch(int x) {
    int i;
    for (i = 0; i < 5000; i++) {
        string s = "Thread:" + x + "=" + i;
        jdisplay(s);
    }
    // 로컬 스레드가 완료될 때까지 기다립니다.
    waitonjoined();
    println("End:" + x);
}

// 두 개를 시작합니다.
launch(0);
launch(1);
// 그리고 완료될 때까지 기다립니다...
waitonjoined();
println("Termination");
```