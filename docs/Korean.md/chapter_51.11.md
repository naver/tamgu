# 버튼

버튼 객체는 사용자가 GUI와 상호 작용할 수 있도록하는 데 필수적입니다. 버튼은 다음 시그니처를 갖는 콜백 함수와 함께 생성되어야 합니다:

```javascript
function callback_button(button b, myobj obj) { ... }
```

버튼 객체는 다음 메서드를 노출합니다:

## 메서드

1. `align(int)`: 버튼 레이블의 정렬을 정의합니다.
2. `bitmap(bitmap im, int color, string label, int labelalign)`: 비트맵을 버튼 이미지로 사용합니다.
3. `color(int code)`: 버튼의 색상을 설정합니다.
4. `create(int x, int y, int w, int h, string type, int shape, string label)`: 버튼을 생성합니다. 사용 가능한 유형 및 모양에 대한 목록은 아래를 참조하십시오.
5. `image(image im, string label, int labelalign)`: 이미지를 버튼 이미지로 사용합니다.
6. `shortcut(string keycode)`: 키보드에서 버튼을 활성화하는 바로 가기를 설정합니다. 사용 가능한 바로 가기에 대한 목록은 아래를 참조하십시오.
7. `value()`: 현재 버튼의 값을 반환합니다.
8. `when(int when1, int when2, ...)`: 콜백을 트리거하는 이벤트 유형을 지정합니다. 이벤트 목록은 아래를 참조하십시오.

## 버튼 유형

- FL_Check
- FL_Light
- FL_Repeat
- FL_Return
- FL_Round
- FL_Regular
- FL_Image

## 버튼 모양

- FL_NORMAL_BUTTON
- FL_TOGGLE_BUTTON
- FL_RADIO_BUTTON
- FL_HIDDEN_BUTTON

## 이벤트 (when)

다음 이벤트는 콜백 함수와 연결할 수 있습니다:

- FL_WHEN_NEVER
- FL_WHEN_CHANGED
- FL_WHEN_RELEASE
- FL_WHEN_RELEASE_ALWAYS
- FL_WHEN_ENTER_KEY
- FL_WHEN_ENTER_KEY_ALWAYS

## 바로 가기

다음 바로 가기는 버튼과 연결할 수 있습니다:

- FL_Button
- FL_BackSpace
- FL_Tab
- FL_Enter
- FL_Pause
- FL_Scroll_Lock
- FL_Escape

```javascript
w.close();
}

function launch(string ph) {
    final = ph;
    // 윈도우 인스턴스화를 시작합니다
    w.begin(300, 200, 1300, 150, "수정");
    // 윈도우 크기 조정 가능하도록 설정합니다
    w.sizerange(10, 20, 0, 0);
    // 현재 윈도우 내에 배치된 winput을 생성합니다
    txt.create(200, 20, 1000, 50, true, "선택");
    txt.value(ph);
    // 버튼을 gettext 메서드와 연결합니다
    button b(this) with gettext;
    b.create(1230, 20, 30, 30, FL_Regular, FL_NORMAL_BUTTON, "확인");
    // 더 이상 객체가 없으므로 세션을 종료합니다
    w.end();
    w.resizable(txt);
    // 윈도우를 실행합니다
    w.run();
}

block b;
b.launch("내 문장");
```

## 이미지

먼저 이미지를 로드한 다음 `FL_Image` 플래그를 사용하여 버튼을 생성합니다.

```javascript
image myimage;
// GIF 이미지를 로드합니다
image.loadgif('c:\\...');
// 버튼을 gettext 메서드와 연결합니다
button b(this) with gettext;
// 이미지 버튼을 생성합니다
b.create(1230, 20, 30, 30, FL_Image, FL_NORMAL_BUTTON, "확인");
// 이미지 내부 레이블과 버튼을 연결합니다
b.image(myimage, "내부", FL_ALIGN_CENTER);
```