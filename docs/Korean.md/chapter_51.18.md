# wcounter

Tamgu ltk의 `wcounter` 모듈은 두 가지 유형의 카운터를 제공합니다. 한 유형은 두 단계의 진행을 표시하고, 다른 유형은 하나의 단계만 표시합니다.

값의 변경을 감지하기 위해 `wcounter`는 콜백 함수와 함께 연결되어야 합니다. 콜백 함수는 다음과 같은 형식을 가져야 합니다:

```javascript
function callback_counter(s, obj) {
    // 카운터 값은 value()로 반환됩니다
    println(s.value());
}
```

콜백 함수를 사용하여 카운터를 생성하려면 다음 구문을 사용하세요:

```javascript
counter s(obj) with callback_counter;
```

`counter` 객체는 다음 메소드를 노출합니다:

1. `bounds(float x, float y)`: 카운터의 경계를 정의합니다.
2. `create(int x, int y, int w, int h, int alignment, string label)`: 카운터를 생성합니다.
3. `font(int s)`: 텍스트 폰트를 설정하거나 반환합니다.
4. `lstep(float)`: 큰 카운터 단계를 정의합니다.
5. `step(float)`: 카운터 단계를 정의합니다.
6. `steps(float)`: 일반적인 카운터 단계와 큰 카운터 단계를 정의합니다.
7. `textcolor(string code|int code)`: 텍스트의 색상을 설정합니다.
8. `textsize(string l)`: 문자열의 너비와 높이를 픽셀로 반환하는 맵을 반환합니다.
9. `type(bool normal)`: `true`인 경우 일반 카운터이고, 그렇지 않으면 간단한 카운터입니다.
10. `value(float)`: 카운터의 값을 정의하거나 값을 반환합니다.

예제 사용법:

```javascript
use("gui");
window w;

function tst(wcounter e, self i) {
    printlnerr(e.value());
}

wcounter c with tst;
w.begin(50, 50, 500, 500, "test");
c.create(30, 30, 300, 100, "Counter");
c.steps(0.01, 0.1);
c.textsize(20);
c.textcolor(FL_RED);
w.end();
w.run();
```