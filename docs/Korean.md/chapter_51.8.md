# winput (입력 영역)

`winput` 객체는 창 내에서 입력 영역을 정의하며, 해당 영역이 해제될 때 호출되는 콜백 함수와 함께 사용할 수 있습니다.

## 메소드

1. `i[a]`: `a` 위치의 입력에서 문자를 추출합니다.
2. `i[a:b]`: `a`와 `b` 사이의 문자를 추출합니다.
3. `color(int c)`: 텍스트 색상을 설정하거나 반환합니다.
4. `create(int x, int y, int w, int h, boolean multiline, string label)`: 멀티라인이면 `true`로 설정하여 입력 영역을 생성합니다.
5. `font(string s)`: 텍스트 폰트를 설정하거나 반환합니다.
6. `fontsize(int c)`: 텍스트 폰트 크기를 설정하거나 반환합니다.
7. `insert(string s, int p)`: 입력에 `s`를 `p` 위치에 삽입합니다.
8. `selection()`: 입력에서 선택된 텍스트를 반환합니다.
9. `value()|(string v)`: 입력 버퍼를 반환하거나 초기 버퍼를 설정합니다.
10. `word(int pos)`: `pos` 위치의 단어를 반환합니다.

## 예제

```java
use("gui");
frame block {
    // 먼저 창 객체를 선언합니다.
    window w;
    string finalResult;
    
    function result(winput txt, block bb) {
        // 이 필드의 내용을 변수에 저장하여 나중에 사용합니다.
        finalResult = txt.value();
    }
    
    // result와 연결된 winput을 먼저 선언합니다.
    winput txt(this) with result;
    
    function launch() {
        // 창 인스턴스화를 시작합니다.
        w.begin(300, 200, 1300, 150, "수정");
        // 창의 크기 조정이 가능하도록 설정합니다.
        w.sizerange(10, 20, 0, 0);
        // 현재 창 내부에 배치된 멀티라인 winput을 생성합니다.
        txt.create(200, 20, 1000, 50, true, "선택");
        // 일부 텍스트로 입력을 초기화합니다.
        txt.value("일부 입력 텍스트");
        // 텍스트는 파란색으로 표시됩니다.
        txt.color(FL_BLUE);
        // 더 이상 객체가 없으므로 세션을 종료합니다.
        w.end();
        // 텍스트가 메인 창의 크기를 따르도록 설정합니다.
        w.resizable(txt);
        // 창을 실행합니다.
        w.run();
    }
}

// 블록을 엽니다.
block b;
// 입력을 표시할 블록입니다.
b.launch();
// b.finalResult에 입력된 문자열이 포함됩니다.
println("결과:", b.finalResult);
```