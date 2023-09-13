# Tamgu 특정 지침

Tamgu는 if, else, elif, switch, for, while 문 등을 포함하여 다양한 알고리즘을 처리하기 위한 모든 필요한 연산을 제공합니다. 또한 and (&&), or (||), xor와 같은 부울 연산자도 지원합니다. 이러한 부울 연산자는 부울 값으로 다른 요소를 결합합니다. and 및 or 연산자는 각각 && 및 ||로 작성할 수도 있습니다.

- `and` 연산자는 모든 요소가 true인 경우 성공합니다.
- `or` 연산자는 최소한 하나의 요소가 true인 경우 성공합니다.
- `xor` 연산자는 최소한 하나의 요소가 true인 경우 성공하지만, 모두 true인 경우 실패합니다.

## if-elif-else 문

Tamgu의 if-elif-else 문은 조건 분기를 수행하는 데 사용됩니다. 구문은 다음과 같습니다:

```python
if (부울식) {
    // 코드 블록
}
elif (부울식) {
    // 코드 블록
}
...
else {
    // 코드 블록
}
```

## ifnot 문

Tamgu의 ifnot 문은 다른 명령을 연결하고, false, null 또는 빈 값 이외의 첫 번째 명령의 값을 반환하는 데 사용됩니다. 복잡한 연산 순서를 처리하는 데 일반적으로 사용됩니다.

예시:

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

Tamgu의 switch 문은 단일 객체에 대한 여러 테스트를 나열하는 데 사용됩니다. 구문은 다음과 같습니다:

```python
switch (식) (함수와 함께) {
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

`v1`, `v2` 등은 문자열 또는 정수 또는 부동 소수점일 수 있습니다. 식은 한 번 평가되고 `v1`, `v2` 등과 비교됩니다. 또한 간단한 요소 간의 비교를 요소의 호출로 대체하는 것도 가능합니다. 호출은 `true` 또는 `false`를 반환해야 합니다.

예시:

```python
// 한 값이 다른 값보다 큰지 테스트합니다.
function tst(int i, int j) {
    if (j >= i)
        return true;
    return false;
}

int s = 10;

// test를 통해 테스트합니다.
switch (s) with tst {
    1: println("1");
    2: println("2");
    20: println("20"); // 이 부분이 선택됩니다.
}
```

## for 문

Tamgu에는 다양한 종류의 `for` 루프가 있습니다. 각각에 대한 설명은 다음과 같습니다.

### `for (식; 부울식; 다음)`

이 `for` 루프는 세 부분으로 구성됩니다: 초기화, 부울식, 다음 부분입니다. 구문은 다음과 같습니다:

```python
for (식; 부울식; 다음) {
    // 코드 블록
}
```

`continue` 또는 `break`를 사용하여 다음 요소로 이동하거나 루프 중간에서 중단할 수 있습니다.

예시:

```python
for (i = 0; i < 10; i += 1)
    print("I=", i, "\n");
```

### `for (변수 in 컨테이너)`

이 유형의 `for` 루프는 문자열이나 파일과 같은 컨테이너를 반복하는 데 사용됩니다. 구문은 다음과 같습니다:

```python
for (변수 in 컨테이너) {
    // 코드 블록
}
```

예시:

```python
// 파일을 반복합니다.
file f('myfile.txt', "r");
string s;
for (s in f)
    println(s);

// int 벡터를 반복합니다.
vector v = [1, 2, 3, 4, 5, 6];
int i;
for (i in v)
    println(l);
```

### `for (i in <시작, 끝, 증가>)`

이 유형의 `for` 루프는 해당하는 동등한 것보다 약 30%에서 50% 더 빠른 빠른 루프입니다. 범위의 각 값은 변수를 통해 인스턴스화될 수 있습니다. 그러나 루프가 시작되면 다른 요소, 변수를 포함하여 수정할 수 없습니다. 구문은 다음과 같습니다:

```python
for (i in <시작, 끝, 증가>) {
    // 코드 블록
}
```

예시:

```python
int i, j = 1;
int v;
time t1;

// 0부터 100000까지 루프, 증가는 1입니다.
for (i in <0, 100000, j>)
    v = i;

time t2;
float diff = t2 - t1;
println("빠른 'for'에 대한 경과 시간:", diff);

time t3;
for (i = 0; i < 100000; i += j)
    v = i;

time t4;
diff = t4 - t3;
println("일반 'for'에 대한 경과 시간:", diff);
```

## while 문

Tamgu의 `while` 문은 단일 부울 식으로 구성됩니다. 구문은 다음과 같습니다:

```python
while (부울식) {
    // 코드 블록
}
```

`continue` 또는 `break`를 사용하여 다음 요소로 이동하거나 루프 중간에서 중단할 수 있습니다.

예시:

```python
int i = 10;
while (i > 0) {
    print("I=", i, "\n");
    i -= 1;
}
```

## do-while 문

Tamgu의 `do-while` 문은 `while`과 유사하지만 첫 번째 반복은 부울 테스트 이전에 수행됩니다. 구문은 다음과 같습니다:

```python
do {
    // 코드 블록
}
while (부울식);
```

예시:

```python
int i = 10;
do {
    print("I=", i, "\n");
    i -= 1;
}
while (i > 0);
```

## UUID(Universally Unique Identifier): `uuid()`

Tamgu의 `uuid()` 함수는 UUID (Universally Unique Identifier)를 반환합니다. UUID는 UUID RFC 4122 버전 4 (완전히 무작위)와 일치하도록 설계되었습니다. 16진수 값을 포함하는 문자열 값을 반환합니다.

예시:

```python
string s = uuid(); // 예시 출력: 80c67c4d-4c4d-14cd-e58d-eb3364cd
```

## 평가: `eval(string code)`

Tamgu의 `eval()` 함수는 Tamgu 코드를 동적으로 평가하고 실행할 수 있습니다. 평가 결과는 평가된 내용에 따라 반환됩니다.

## JSON 평가: `evaljson(string code)`

Tamgu의 `evaljson()` 함수는 JSON 문자열을 입력으로 받아 내부 구조에 따라 벡터 또는 맵으로 컴파일합니다.

## 출력 지침

Tamgu는 현재 디스플레이 포트에 결과를 표시하기 위해 여러 출력 지침을 제공합니다. `print` 및 `println` 지침은 결과를 표시하는 데 사용되며, `printerr` 및 `printlnerr` 지침은 결과를 표준 오류 출력에 표시합니다. `println` 및 `printlnerr` 버전은 출력에 대해 두 가지 기능을 추가합니다: `,`로 구분된 값에 대해 추가 공백이 추가되고, 줄의 끝에 개행이 추가됩니다.

## 조인 출력 지침

`printj`, `printjln`, `printjerr`, `printjlnerr` 지침은 미리 "조인"된 컨테이너 값을 표시하는 데 사용됩니다. 두 개, 세 개 또는 네 개의 인수를 허용합니다. 첫 번째 매개변수는 컨테이너이어야 하며, 두 번째 매개변수는 구분자 문자열이어야 합니다. 컨테이너가 맵인 경우 세 번째 매개변수로 키 구분자를 제공할 수도 있습니다. 네 번째 숫자 매개변수는 n 개의 값마다 개행을 추가하기 위해 제공될 수 있습니다.

예시:

```python
ivector v = [1..10];
printj(v, "-"); // 출력: 1-2-3-4-5-6-7-8-9-10

map m = {1: 2, 2: 3, 4: 5, 6: 7};
printjln(m, ",", "-"); // 출력: 1-2,2-3,4-5,6-7
```

## IO 리디렉션 함수

`ioredirect` 및 `iorestate` 함수는 stderr 또는 stdout의 출력을 파일로 리디렉션하는 데 사용됩니다. `ioredirect` 함수는 stderr를 리디렉션하는 경우 err이 true이고, stdout을 리디렉션하는 경우 err이 false입니다. 이 함수는 파일을 리디렉션하고 ID를 반환합니다. `iorestate` 함수를 사용하여 출력을 정상 상태로 설정하는 데 사용됩니다.

예시:

```python
int o = ioredirect('C:\Tamgu\test\test.txt', true);
printlnerr("This string is now stored in file: test.txt");
iorestate(o, true); // 정상 상태로 복원
```

## 일시 정지 및 대기 함수

`pause` 및 `sleep` 함수는 스레드를 일시 정지 또는 대기 모드로 설정하는 데 사용됩니다. `pause` 함수는 스레드의 실행을 중지하지 않지만, `sleep` 함수는 중지합니다. `pause` 함수는 초 단위로 지정된 일시 정지 기간을 나타내는 부동 소수점을 입력으로 받습니다. 두 번째 부울 매개변수를 사용하여 작은 애니메이션을 표시할 수도 있습니다. `sleep` 함수는 OS sleep 명령을 기반으로 하며, 동작은 로컬 구현에 따라 다릅니다. 정수를 입력으로 받으며, 대기 기간을 초 단위로 나타냅니다.

예시:

```python
pause(0.1); // 스레드는 10ms 동안 일시 중지됩니다.
pause(2, true); // 스레드는 작은 애니메이션과 함께 2초 동안 일시 중지됩니다.
sleep(1); // 스레드는 1초 동안 대기합니다 (플랫폼에 따라 다름).
```

## 이모지 함수

`emojis()` 함수는 표준 v5.0 베타에 따라 모든 이모지 문자 목록을 반환합니다.

## GPS 거리 함수

Tamgu의 `GPSdistance` 함수는 경도와 위도를 사용하여 두 개의 GPS 지점 사이의 거리를 계산하는 데 사용됩니다. 다섯 번째 매개변수는 선택 사항이며, 지구 반지름을 나타내며 기본값은 6371km입니다. 이 값은 최종 거리를 계산하는 데 사용되는 단위를 정의합니다.

예시:

```python
float d = GPSdistance(46.9001, 7.1201, 47.01, 6.1);
// 출력: d는 78.382km입니다.
```

## 키보드에서 읽기: `kget()`

Tamgu의 `kget()` 함수는 키보드에서 문자열을 읽는 데 사용됩니다.

예시:

```python
string message = kget("Message:");
```

## 지속 변수

GUI의 컨텍스트에서 지속 변수를 생성하여 프로그램을 반복해서 실행할 때 특정 값을 추적할 수 있습니다. 다음과 같은 지속 유형이 사용할 수 있습니다: `ithrough`, `fthrough`, `uthrough`, `sthrough`, `vthrough`, `mthrough`. 이러한 변수는 실행 간에 다시 초기화되지 않습니다.

예시:

```python
// 이 변수는 프로그램이 실행된 횟수를 추적합니다.
ithrough icount;
icount += 1;
println(icount);
```