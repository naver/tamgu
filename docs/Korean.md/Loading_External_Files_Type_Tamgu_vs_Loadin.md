# 외부 파일 로딩: 타입 탐구 vs 로딩

`tamgu` 타입은 특정 탐구 프로그램을 동적으로 로드하는 데 사용됩니다. 반면에 `loadin`은 파일의 내용을 현재 프로그램에 로드하는 데 사용됩니다.

## 9.1 메소드

1. `tamgu var(string Tamgu pathname)`: 탐구 프로그램을 생성하고 로드합니다.

## 9.2 외부 함수 실행

탐구 파일에서 사용 가능한 함수는 `tamgu` 변수를 통해 호출할 수 있습니다.

예시:

```cpp
// 우리의 프로그램 test.tmg에서 함수 Read를 구현합니다.
// test.Tamgu에 다음과 같이 구현합니다.
function Read(string s) {
    return (s + "_toto");
}

// call.Tamgu에 다음과 같이 구현합니다.
// 우리의 호출 프로그램에서 먼저 test.Tamgu를 로드한 다음 Read를 실행합니다.
tamgu kf('c:\\test.tmg'); // Read를 구현한 프로그램을 로드합니다.
string s = kf.Read("xxx"); // 우리의 로컬 프로그램에서 Read를 실행할 수 있습니다.
```

## 비공개 함수

외부 프로그램이 특정 함수에 접근하지 못하도록 하려면 해당 함수를 비공개로 선언하면 됩니다.

예시:

```cpp
// 외부에서 호출할 수 없는 함수를 구현합니다.
private function CannotBeCalled(string s) { ... }
```

## `loadin(string pathname)`

탐구는 또한 다른 프로그램에서 파일을 로드하는 또 다른 방법을 제공합니다. `loadin(pathname)`은 파일의 내용을 현재 프로그램에 로드합니다. 다시 말해, `loadin`은 현재 파일이 `loadin`으로 로드하는 내용의 코드를 포함하는 것과 정확히 같은 방식으로 작동합니다. 실제로 코드는 `loadin` 호출이 작성된 위치에 로드됩니다.

예시:

```cpp
// 우리의 프로그램 test.tmg에서 함수 Read를 구현합니다.
// test.Tamgu에 다음과 같이 구현합니다.
function Read(string s) {
    return (s + "_toto");
}

// call.Tamgu에 다음과 같이 구현합니다.
// 우리의 호출 프로그램에서 먼저 test.Tamgu를 로드한 다음 Read를 실행합니다.
loadin('c:\\test.tmg'); // Read를 구현한 코드를 로드합니다.
string s = Read("xxx"); // 우리의 로컬 프로그램에서 바로 Read를 실행할 수 있습니다.
```