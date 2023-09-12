# wprogress

Tamgu ltk는 최소값부터 최대값까지의 진행을 표시할 수 있는 진행 막대 위젯을 제공합니다.

wprogress는 값을 수정하는 콜백 함수에 연결하여 사용할 수 있습니다. 함수는 다음과 같은 형식을 가져야 합니다:

```javascript
function callback_progress(wprogress s, myobj obj) {
    // 진행 값은 value()로 반환됩니다
    println(s.value());
}
```

`obj`와 함께 진행 객체 `s`를 만들고 `callback_progress` 함수를 연결하세요.

진행 객체는 다음과 같은 함수를 노출합니다:

메소드:
1. `backgroundcolor(int color)`: 배경색을 설정하거나 반환합니다
2. `barcolor(string code|int code)`: 막대 색상을 설정합니다
3. `create(int x, int y, int w, int h, int alignment, string label)`: 진행 막대를 생성합니다
4. `minimum(float x)`: 진행 막대의 최소값을 정의하거나 반환합니다
5. `maximum(float x)`: 진행 막대의 최대값을 정의하거나 반환합니다
6. `value(float)`: 진행 값 설정 또는 값 반환

예시:
```javascript
use("gui");
window w;
wprogress c;

thread progressing() {
    for (int i in <0, 100, 1>) {
        for (int j in <0, 100000, 1>) {}
        c.value(i);
    }
    printlnerr("End");
}

function launch(button b, self e) {
    progressing();
}

button b with lance;

w.begin(50, 50, 500, 500, "test");
c.create(30, 30, 300, 30, "progression");
b.create(100, 100, 50, 50, "Ok");
c.minimum(0);
c.maximum(100);
c.barcolor(FL_BLUE);
w.end();
w.run();
```