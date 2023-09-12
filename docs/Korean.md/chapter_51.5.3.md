## README

이 README는 원본 텍스트의 개선된 버전을 제공합니다.

### 키보드 동작

키보드 동작을 콜백 함수와 연결하려면 다음 함수를 사용하세요:

`onkey(action, callback, myobject)`

- `action`은 다음 값 중 하나여야 합니다:
  - `FL_KEYUP`: 키를 누를 때
  - `FL_KEYDOWN`: 키를 놓을 때

`callback` 함수는 다음 형식을 가져야 합니다:

`function callback_key(window w, string skey, int ikey, int combine, myobject object)`

- 첫 번째 매개변수는 윈도우 자체입니다.
- 두 번째 매개변수는 눌린 키와 일치하는 텍스트입니다.
- 세 번째 매개변수는 키 코드입니다.
- 네 번째 매개변수는 현재 키와 함께 눌린 모든 명령 키의 조합입니다.
- 마지막 매개변수는 키 함수와 함께 제공된 객체입니다.

### 조합 값

조합 값은 다음 가능한 값을 가진 이진 코드 정수입니다:
- 1: ctrl 키가 눌렸음을 나타냅니다.
- 2: alt 키가 눌렸음을 나타냅니다.
- 4: command 키가 눌렸음을 나타냅니다.
- 8: shift 키가 눌렸음을 나타냅니다.

### 예제

```javascript
use("gui");

// 윈도우, 그리기 함수, 객체를 선언합니다.
function pushed(window w, string skey, int ikey, int comb, mycoord o) {
    // "J" 키가 눌리면 사각형을 10픽셀 위아래로 이동합니다.
    if (skey == "J") {
        o.x += 10;
        o.y += 10;
        // 좌표를 업데이트하기 위해 전체 그림을 다시 그립니다.
        w.redraw();
    }
}

window wnd(coords) with display;

wnd.begin(100, 100, 300, 300, "Drawing");
wnd.sizerange(10, 10, 0, 0);
// 키 콜백을 추가하면 키를 누를 때마다 "pushed" 함수가 호출됩니다.
// 동일한 객체 coords를 윈도우와 공유합니다.
wnd.onkey(FL_PUSH, pushed, coords);
wnd.end();
wnd.run();
```

### 메뉴 추가

윈도우에 메뉴를 추가하려면 인터페이스의 다른 요소에 비해 더 많은 작업이 필요합니다. 메뉴는 일련의 최상위 메뉴 항목으로 구성되며, 각 최상위 메뉴 항목에 대해 특정 설명을 제공해야 합니다.

### 사각형 이동

FLTK는 이벤트 기반으로 작동하기 때문에 애니메이션은 적절한 함수로 수행할 수 있습니다. 유일한 방법은 윈도우 환경과 독립적으로 실행되는 스레드를 사용하는 것입니다.

```javascript
use("gui");

// 데이터를 기록하기 위한 작은 프레임
frame mycoord {
    int x, y;
    function _initial() {
        x = 0;
        y = 0;
    }
}

// 데이터를 기록하기 위한 객체를 선언합니다.
mycoord coords;
bool first = true;

// 화면에 표시하는 함수
// 윈도우가 수정될 때마다 이 함수가 mycoord 객체와 함께 호출됩니다.
function display(window w, mycoord o) {
    if (first) {
        w.drawcolor(FL_RED);
        w.drawtext("Press T", 20, 20);
    } else {
        // 다음 객체에 적용할 색상을 선택합니다.
        w.cursorstyle(FL_CURSOR_CROSS, FL_BLACK, FL_WHITE);
        w.drawcolor(FL_RED);
        w.rectangle(o.x, o.y, 60, 60);
        // 텍스트를 추가합니다.
        w.drawtext("TEST", o.x + 20, o.y + 20);
    }
}

wnd.run();
```

### 스레드에서 윈도우 생성

특정 주의사항을 지키면 스레드 내에서 윈도우를 생성할 수 있습니다. FLTK는 스레드 내에서 윈도우를 직접 생성하는 것을 허용하지 않지만, 윈도우 생성 또는 확장 요청을 게시하기 위해 사용할 수 있는 메시지 메커니즘이 제공됩니다.

- 문제를 피하기 위해 윈도우 생성 자체를 잠그세요.
- 윈도우 생성이 실패할 경우 내부 잠금을 피하기 위해 타임아웃을 정의하세요.
- 위젯 생성 중에 문제가 발생하면 해당 범위의 윈도우를 닫으세요.
- 사용자에 의해 윈도우가 이동되거나 수정될 때마다, 윈도우의 표시와 업데이트는 메인 스레드 내에서만 수행될 수 있으므로 다른 스레드 표시가 잠시 멈출 수 있습니다.

### 예제

```javascript
use("gui");

int px = 300;
int py = 400;
int nb = 1;

// 이 스레드는 카운터를 표시합니다.
thread bouge() {
    int i = 0;
    window wx;
    woutput wo;
    string err;

    // 모든 하위 객체와 공유할 타임아웃이 있는 주 메인 윈도우를 초기화합니다.
    wx.timeout(0.1);

    // 동시에 하나의 스레드만 윈도우를 생성할 수 있도록 메인 잠금을 설정합니다.
    lock("creation");

    function pressed(button b, self n) {
        bouge();
    }

    // 윈도우, 연결된 그리기 함수, 객체 coord를 선언합니다.
    window wnd;

    // 마우스 콜백을 인스턴스화합니다.
    wnd.begin(100, 50, 500, 300, "Drawing");
    button b with pressed;
    b.create(430, 20, 60, 60, FL_Regular, FL_NORMAL_BUTTON, "Ok");
    wnd.sizerange(10, 10, 0, 0);
    // 마우스 콜백을 추가합니다.
    wnd.end();
    wnd.run();
}
```