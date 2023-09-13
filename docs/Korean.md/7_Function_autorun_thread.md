# 함수, Autorun, Thread

Tamgu에서 함수는 `function` 키워드를 사용하여 선언되며, 이름과 매개변수를 따라옵니다. `return` 키워드를 사용하여 값을 반환할 수 있습니다. 매개변수는 항상 값으로 전달되며, 타입이 `self`인 경우를 제외하고는 예외적으로 참조로 전달됩니다. 함수는 반환 값에 대한 타입 정보를 제공하지 않는다는 점에 유의해야 합니다.

## 7.1 반환 타입 강제

함수는 인수 목록 뒤에 `::` 기호를 사용하여 반환 타입을 지정할 수 있습니다. 예를 들어:

```
function toto(int i) :: int {
    i += 10;
    return i;
}
```

이 예제에서 함수 `toto`는 `int`를 반환하도록 선언되었습니다.

## 7.2 Autorun

`autorun` 함수는 선언 후 자동으로 실행됩니다. Autorun 함수는 메인 파일에서만 실행됩니다. 다른 파일에서 호출되는 파일에 autorun 함수가 있는 경우 해당 함수는 실행되지 않습니다.

예제:

```
autorun waitonred() {
    println("로딩 중:", _paths[1]);
}
```

## 7.3 Thread

`thread` 함수가 실행되면 독립적인 시스템 스레드에서 실행됩니다.

예제:

```
thread toto(int i) {
    i += 10;
    return i;
}

int j = toto(10);
print("J=" + j + "\n");
```

출력:

```
J=20
```

### 보호된 스레드

`protected` 스레드는 두 개의 스레드가 동시에 동일한 코드 라인에 액세스하는 것을 방지합니다. 보호된 스레드는 실행이 시작될 때 잠금을 설정하고, 함수가 실행된 후에 잠금을 해제합니다. 이렇게 함으로써 보호된 함수에 대한 다른 호출은 순차적으로 수행되고 동시에 수행되지 않도록 보장됩니다. 보호된 스레드는 재진입(reentrant)이 아닌 코드에 사용되어야 합니다.

예제:

```
// 동기화된 스레드를 구현합니다.
protected thread launch(string n, int m) {
    int i;
    println(n);
    // 모든 값을 표시합니다.
    for (i = 0; i < m; i++)
        print(i, " ");
    println();
}

function principal() {
    // 스레드를 실행합니다.
    launch("첫 번째", 2);
    launch("두 번째", 4);
    println("끝");
}

principal();
```

출력:

```
끝
첫 번째
0 1
두 번째
0 1 2
```

### 배타적 스레드

`exclusive` 스레드는 보호된 스레드와 유사하지만 한 가지 차이가 있습니다. 보호된 스레드의 경우 보호는 메서드 수준에서 이루어지지만, 배타적 스레드의 경우 프레임 수준에서 보호가 이루어집니다. 이는 배타적 스레드가 한 번에 한 스레드만 프레임 객체에 액세스할 수 있도록 합니다.

예제:

```
// 이 프레임은 두 개의 메서드를 노출합니다.
frame disp {
    // 배타적
    exclusive thread edisplay(string s) {
        println("배타적:", s);
    }
    // 보호된
    protected thread pdisplay(string s) {
        println("보호된:", s);
    }
}

// 작업 프레임도 구현합니다.
frame task {
    string s;
    // 특정 "disp" 인스턴스와 함께
    disp cx;

    function _initial(string x) {
        s = x;
    }

    // 보호된 로컬 인스턴스를 호출합니다.
    function pdisplay() {
        cx.pdisplay(s);
    }

    // 배타적 로컬 인스턴스를 호출합니다.
    function edisplay() {
        cx.edisplay(s);
    }

    // 전역 인스턴스를 배타적으로 호출합니다.
    function display(disp c) {
        c.edisplay(s);
    }
}

// 공통 인스턴스
disp c;
vector v;
int i;
string s = "T";

for (i = 0; i < 100; i++) {
    s = "T" + i;
    task t(s);
    v.push(t);
}

// 이 경우, 표시는 보호된 것이 재진입되지 않으므로 순서대로 수행됩니다.
// 하나의 pdisplay만 동시에 실행할 수 있습니다.
for (i = 0; i < 100; i++)
    v[i].pdisplay();

// 이 경우, 표시는 모든 배타적인 edisplay가 병렬로 작동하는 혼합이 될 것입니다.
// 배타적인 것은 한 인스턴스 내의 메서드만 보호하므로 이 경우에는 다른 인스턴스가 있습니다...
for (i = 0; i < 100; i++)
    v[i].edisplay();

// 마지막으로, 모든 "task"에서 공통 "disp" 개체를 공유합니다.
// 표시는 이번에도 보호된 것처럼 순서대로 될 것입니다. 이번에는 동일한
// "c disp" 인스턴스에 도달합니다.
for (i = 0; i < 100; i++)
    v[i].display(c);
```

### Joined와 Waitonjoined

스레드는 joined로 선언될 수 있으며, 이는 주 스레드가 자신의 코드를 완료하기 전에 시작된 모든 스레드의 완료를 기다리도록 합니다. `waitonjoined()`를 사용하여 이러한 스레드의 완료를 기다릴 수 있습니다. 여러 개의 `waitonjoined()`를 필요한 만큼 다른 스레드에서 사용할 수 있습니다. `waitonjoined()`는 특정 스레드 내에서 시작된 joined 스레드에 대해서만 대기합니다.

### 원자값

대부분의 데이터 구조(맵, 벡터, 문자열 등)는 락을 통해 동시 액세스로부터 보호됩니다. 그러나 이러한 락은 시간과 공간 면에서 비용이 많이 들 수 있습니다. 이를 해결하기 위해 Tamgu는 `a_int`, `a_string` 또는 `a_mapii`와 같은 락 없는 데이터 구조(원자형)를 제공합니다. 이러한 데이터 구조는 기계의 효율적인 사용을 가능하게 하므로 이러한 타입에 액세스하는 스레드는 대기하지 않습니다. 그러나 구현 방식 때문에 비스스로운 타입에서는 비순차적인 컨텍스트에서 느릴 수 있습니다. 이러한 타입은 오직 스레드 간에 공유될 때에만 유용합니다.

### 스트림 연산자 '<<<'

스레드를 시작할 때, `=` 연산자를 사용하여 스레드의 결과를 직접 변수에 저장할 수 없습니다. 스레드는 호출한 코드와의 연결이 없이 자체적인 생명주기를 가지기 때문입니다. Tamgu는 이 작업을 위해 특정 연산자인 `<<<`(스트림 연산자)를 제공합니다. 스트림은 스레드와 연결된 변수로, 스레드에서 반환된 값들을 해당 변수에 저장할 수 있도록 연결됩니다. 물론, 변수는 스레드와 독립적으로 존재해야 합니다.

예제:

```cpp
// "join" 스레드로 스레드를 생성하여 waitonjoined를 사용할 수 있도록 합니다.
// 이 스레드는 단순히 2 * x를 반환합니다.
joined thread Test(int i) {
    return (i * 2);
}

// 10개의 스레드를 시작할 launch 함수를 정의합니다.
function launch() {
    // 이 함수 내에서 맵 저장 변수를 먼저 선언합니다.
    treemapi m;
    int i = 0;

    // 그런 다음 10개의 스레드를 시작하고 각각의 결과를 m의 특정 위치에 저장합니다.
    for (i in <0,10,1>)
        m[i] <<< Test(i);

    // 모든 스레드가 완료될 때까지 기다립니다.
    waitonjoined();

    // 최종 값을 표시합니다.
    println(m); // {0:0,1:2,2:4,3:6,4:8,5:10,6:12,7:14,8:16,9:18}
}

launch();
```

## 7.4 다중 정의

Tamgu는 동일한 이름을 가진 함수의 다중 정의를 허용합니다. 이 경우, 매개변수 정의가 다른 `display(string s)`와 `display(int s)`를 구현할 수 있습니다. 이 경우, 시스템은 함수 호출의 인수 목록에 기반하여 가장 적합한 함수를 선택합니다.

예제:

```cpp
function testmultipledeclaration(string s, string v) {
    println("문자열:", s, v);
}

function testmultipledeclaration(int s, int v) {
    println("정수:", s, v);
}

// 변수를 선언합니다.
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// 이 경우, 시스템은 인수 목록에 따라 올바른 함수를 선택합니다.
testmultipledeclaration(s1, s2); // 문자열 구현
testmultipledeclaration(i, j); // 정수 구현
```

## 7.5 기본 인수

Tamgu는 함수에서 기본 인수를 선언할 수 있는 메커니즘을 제공합니다. 호출에서 생략될 수 있는 매개변수에 대한 값을 정의할 수 있습니다.

예제:

```cpp
function acall(int x, int y = 12, int z = 30, int u = 43) {
    println(x + y + z + u);
}

acall(10, 5); // 결과: 88 = 10 + 5 + 30 + 43
```

참고: 선언 목록에서는 마지막 매개변수만 선택적일 수 있습니다.

## 7.6 특정 플래그: Private & Strict

함수는 두 가지 특정 플래그인 `private`와 `strict`로 선언될 수도 있습니다.

### Private

함수가 `private`로 선언되면 현재 Tamgu 파일 외부에서 액세스할 수 없습니다. Tamgu 프로그램이 다른 Tamgu 프로그램에서 로드된 경우, private 함수는 로더에서 액세스할 수 없습니다.

예제:

```cpp
// 이 함수는 외부 로더에서는 보이지 않습니다.
private function toto(int i) {
    i += 10;
    return i;
}
```

### Strict

기본적으로 Tamgu에서 함수가 선언되면 언어는 호출 함수의 각 인수를 함수 구현에서 예상하는 매개변수로 변환하려고 시도합니다. 그러나 경우에 따라 더 엄격한 매개변수 검사가 필요할 수 있습니다. `strict` 플래그는 함수에 대해 엄격한 매개변수 제어를 요구하여 이 문제를 해결하는 데 도움을 줍니다.

예제:

```cpp
strict function teststrictdeclaration(int s, int v) {
    println("정수:", s, v);
}

// 변수를 선언합니다.
string s1 = "s1";
string s2 = "s2";

// 이 경우, 시스템은 이러한 매개변수에 대해 올바른 함수를 찾지 못하고
// 오류 메시지를 반환합니다.
teststrictdeclaration(s1, s2); // 문자열 구현 없음
```