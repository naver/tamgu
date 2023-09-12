# 슬라이더

Tamgu ltk는 두 가지 유형의 슬라이더를 제공합니다. 한 유형의 슬라이더는 슬라이드 바 자체와 함께 값을 표시합니다.

수정 사항을 감지하려면 슬라이더를 콜백 함수와 연결해야 합니다. 함수는 다음 시그니처를 가져야 합니다:

```javascript
function callback_slider(slider s, myobj obj) {
  // 슬라이더 값은 value()로 반환됩니다
  println(s.value());
}
```

콜백 함수가 있는 슬라이더를 생성하려면 다음 구문을 사용하세요:

```javascript
slider s(obj) with callback_slider;
```

슬라이더는 다음 메소드를 노출합니다:

메소드:
1. `align(int align)`: 슬라이더 정렬을 정의합니다
2. `bounds(int x, int y)`: 슬라이더 경계를 정의합니다
3. `create(int x, int y, int w, int h, int align, bool valueslider, string label)`: 슬라이더 또는 값 슬라이더를 생성합니다 (아래에서 정렬 값 목록을 참조하세요)
4. `resize(object)`: 객체의 크기 조정이 가능하도록 합니다
5. `step(int)`: 슬라이더 단계를 정의합니다
6. `type(int x)`: 값 슬라이더 유형을 설정합니다 (아래에서 슬라이더 유형 목록을 참조하세요)
7. `value()`: 슬라이더 값을 반환합니다
8. `value(int i)`: 슬라이더의 초기 값 설정

슬라이더 유형:
- FL_VERT_SLIDER
- FL_HOR_SLIDER
- FL_VERT_FILL_SLIDER
- FL_HOR_FILL_SLIDER
- FL_VERT_NICE_SLIDER
- FL_HOR_NICE_SLIDER

## 예시

이 예시에서는 슬라이더가 다른 창에서 사각형의 이동을 제어하는 방법을 보여줍니다.

```javascript
use("gui");

// 데이터를 기록할 작은 프레임
frame mycoord {
  int color;
  int x, y;

  function _initial() {
    color = FL_RED;
    x = 0;
    y = 0;
  }
}

// 데이터를 기록할 오브젝트를 선언합니다
mycoord coords;

// 창을 선언하고 그에 연결된 그리기 함수와 coord 객체를 함께 사용합니다
window wnd(coords) with display;

// 전역 변수 wnd를 사용하여 창에 접근하는 것이 조금 부정적입니다
function slidercall(slider s, mycoord o) {
  // 슬라이더 값에 따라 창 X 위치를 설정합니다
  o.x = s.value();
  wnd.redraw();
}

slider vs(coords) with slidercall;

// 마우스 콜백을 인스턴스화해야 합니다
wnd.begin(100, 100, 300, 300, "Drawing");
wnd.sizerange(10, 10, 0, 0);

// 값을 표시하는 슬라이더를 생성합니다
vs.create(10, 10, 180, 20, FL_ALIGN_LEFT, true, "Position");

// 값은 0부터 300까지입니다
vs.bounds(0, 300);

// 초기 값은 100입니다
vs.value(100);

wnd.end();
wnd.run();
```