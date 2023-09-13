# Tamgu LTK

Tamgu LTK는 그래픽 사용자 인터페이스를 만들기 위한 다양한 구성 요소와 기능을 제공하는 라이브러리입니다. 이 README.md 파일은 다른 구성 요소와 그들의 사용법에 대한 개요를 제공합니다.

## 구성 요소

### 폰트

Tamgu LTK는 다음과 같은 폰트 코드를 제공합니다:

- FL_HELVETICA
- FL_HELVETICA_BOLD
- FL_HELVETICA_ITALIC
- FL_HELVETICA_BOLD_ITALIC
- FL_COURIER
- FL_COURIER_BOLD
- FL_COURIER_ITALIC
- FL_COURIER_BOLD_ITALIC
- FL_TIMES
- FL_TIMES_BOLD
- FL_TIMES_ITALIC
- FL_TIMES_BOLD_ITALIC
- FL_SYMBOL
- FL_SCREEN
- FL_SCREEN_BOLD

폰트를 사용하려면 위젯이나 스타일을 생성할 때 지정할 수 있습니다.

### 선 모양

Tamgu LTK는 다음과 같은 선 모양을 제공합니다:

- FL_SOLID
- FL_DASH
- FL_DOT
- FL_DASHDOT
- FL_DASHDOTDOT
- FL_CAP_FLAT
- FL_CAP_ROUND
- FL_CAP_SQUARE
- FL_JOIN_MITER
- FL_JOIN_ROUND
- FL_JOIN_BEVEL

이러한 선 모양은 선이나 도형을 그릴 때 사용할 수 있습니다.

### 커서 모양

Tamgu LTK는 응용 프로그램에서 사용할 수 있는 다양한 커서 모양을 제공합니다:

- FL_CURSOR_DEFAULT: 기본 커서, 일반적으로 화살표입니다.
- FL_CURSOR_ARROW: 화살표 포인터.
- FL_CURSOR_CROSS: 십자가.
- FL_CURSOR_WAIT: 시계나 모래시계.
- FL_CURSOR_INSERT: I 빔.
- FL_CURSOR_HAND: 손 (MSWindows의 위쪽 화살표).
- FL_CURSOR_HELP: 물음표.
- FL_CURSOR_MOVE: 4개의 화살표.
- FL_CURSOR_NS: 위/아래 화살표.
- FL_CURSOR_WE: 왼쪽/오른쪽 화살표.
- FL_CURSOR_NWSE: 대각선 화살표.
- FL_CURSOR_NESW: 대각선 화살표.
- FL_CURSOR_NONE: 보이지 않음.
- FL_CURSOR_N: 이전 버전과의 호환성을 위해.
- FL_CURSOR_NE: 이전 버전과의 호환성을 위해.
- FL_CURSOR_E: 이전 버전과의 호환성을 위해.
- FL_CURSOR_SE: 이전 버전과의 호환성을 위해.
- FL_CURSOR_S: 이전 버전과의 호환성을 위해.
- FL_CURSOR_SW: 이전 버전과의 호환성을 위해.
- FL_CURSOR_W: 이전 버전과의 호환성을 위해.
- FL_CURSOR_NW: 이전 버전과의 호환성을 위해.

이러한 커서 모양은 응용 프로그램에서 커서의 모양을 변경하는 데 사용할 수 있습니다.

### 간단한 창

Tamgu LTK에서는 다음 단계를 따라 간단한 창을 만들 수 있습니다:

1. 창 객체를 선언합니다.
2. 창 인스턴스화를 시작합니다.
3. 창 크기 범위를 설정합니다.
4. 창 내에서 위젯을 생성합니다.
5. 창 세션을 종료합니다.
6. 창을 실행하여 시작합니다.

다음은 예제 코드 스니펫입니다:

```cpp
use("gui");

// 창 객체 선언
window w;

// 창 인스턴스화 시작
w.begin(300, 200, 1300, 150, "수정");

// 창 크기 범위 설정
w.sizerange(10, 20, 0, 0);

// 창 내에 winput 위젯 생성
txt.create(200, 20, 1000, 50, true, "선택");

// 창 세션 종료
w.end();

// 창 실행
w.run();
```

창에 위젯을 저장하지 않으려면 `begin` 호출을 `end`와 `create`로 대체할 수 있습니다.

### 그리기 창

선이나 원과 같은 것을 그리려면 창에 새로운 그리기 함수를 제공할 수 있습니다. Tamgu LTK에서는 `with` 키워드를 사용하여 이를 수행할 수 있습니다.

다음은 예제 코드 스니펫입니다:

```cpp
window wnd(object) with callback_window;

function callback_window(window w, type o) {
    // 그리기 메서드 여기에 작성
}

wnd.run();
```

위의 코드에서 `object`는 연결된 함수가 호출될 때 전달되는 변수입니다. 연결된 함수는 그리기 메서드의 시퀀스여야 합니다.

### 마우스 동작

Tamgu LTK는 콜백 함수를 통해 다양한 마우스 동작을 추적하는 방법을 제공합니다. `onmouse` 메서드는 마우스 동작을 특정 콜백 함수와 연결합니다.

다음은 예제 코드 스니펫입니다:

```cpp
onmouse(action, callback, myobject);
```

`action` 매개변수는 다음 값 중 하나여야 합니다: `FL_PUSH`, `FL_RELEASE`, `FL_MOVE`, `FL_DRAG`, 또는 `FL_MOUSEWHEEL`.

`callback` 함수는 다음 시그니처를 가져야 합니다:

```cpp
function callback_mouse(window w, map coords, type myobject) {
    // 콜백 함수 코드
}
```

`coords` 매개변수는 마우스 동작에 대한 정보를 포함하는 맵입니다. 예를 들어, 눌린 버튼, 마우스 좌표 및 마우스 휠 증분과 같은 정보가 있습니다.

## 결론

Tamgu LTK는 그래픽 사용자 인터페이스를 만들기 위한 다양한 구성 요소와 기능을 제공합니다. 쉬운 문법과 강력한 기능을 갖추고 있어 GUI 애플리케이션 개발에 좋은 선택입니다.