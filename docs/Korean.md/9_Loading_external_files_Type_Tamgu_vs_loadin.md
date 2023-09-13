## 외부 파일 로딩: 타입 Tamgu vs 로딩

`tamgu` 타입은 특정 Tamgu 프로그램을 동적으로 로드하는 데 사용됩니다. 반면에 `loadin`은 파일의 내용을 현재 프로그램에 로드하는 데 사용됩니다.

### 9.1 메소드

1. `tamgu var(string Tamgu pathname)`: Tamgu 프로그램을 생성하고 로드합니다.

### 9.2 외부 함수 실행

Tamgu 파일에서 사용 가능한 함수는 `tamgu` 변수를 통해 호출할 수 있습니다.

#### 예제

```cpp
// 우리의 프로그램 test.tmg에서 함수 Read를 구현합니다.
// test.Tamgu에서
function Read(string s) {
    return (s + "_toto");
}
//-------------------------------------------
// call.Tamgu에서
// 호출하는 프로그램에서 먼저 test.Tamgu를 로드한 다음 Read를 실행합니다.
tamgu kf('c:\test.tmg'); // Read를 구현하는 프로그램을 로드합니다.
string s = kf.Read("xxx"); // 우리의 로컬 프로그램에서 Read를 실행할 수 있습니다.
```

### 비공개 함수

외부 프로그램이 특정 함수에 액세스하지 못하도록 하려면 이러한 함수를 비공개로 선언할 수 있습니다.

#### 예제

```cpp
// 외부에서 호출할 수 없는 함수를 구현합니다.
private function CannotBeCalled(string s) { ... }
```

### `loadin(string pathname)`

Tamgu는 또 다른 방법으로 다른 프로그램에서 파일을 로드할 수 있습니다. `loadin(pathname)`은 파일의 내용을 현재 프로그램에 로드합니다. 다시 말해, `loadin`은 현재 파일에 `loadin`으로 로드된 내용의 코드가 있는 것처럼 동작합니다.

코드는 `loadin` 호출이 작성된 위치에서 로드됩니다.

#### 예제

```cpp
// 우리의 프로그램 test.tmg에서 함수 Read를 구현합니다.
// test.Tamgu에서
function Read(string s) {
    return (s + "_toto");
}
//-------------------------------------------
// call.Tamgu에서
// 호출하는 프로그램에서 먼저 test.Tamgu를 로드한 다음 Read를 실행합니다.
loadin('c:\test.tmg'); // Read를 구현하는 코드를 로드합니다.
string s = Read("xxx"); // 우리의 로컬 프로그램에서 Read를 직접 실행할 수 있습니다.
```