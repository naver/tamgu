## 창

`Window` 클래스는 이 그래픽 라이브러리의 진입점입니다. 상자, 버튼, 슬라이더 등을 표시하는 데 사용할 수 있는 다양한 메서드를 제공합니다.

### 메서드

1. `alert(string msg)`: 경고 메시지가 포함된 팝업 창을 표시합니다.
2. `arc(float x, float y, float rad, float a1, float a2)`: 호를 그립니다.
3. `arc(int x, int y, int x1, int y1, float a1, float a2)`: 호를 그립니다.
4. `ask(string msg, string buttonmsg2, string buttonmsg1, ...)`: 질문이 포함된 팝업 창을 표시하고 누른 버튼에 따라 값을 반환합니다 (최대 4개의 버튼).
5. `begin(int x, int y, int w, int h, string title)`: 창을 생성하고 초기화를 시작합니다. `w`와 `h`는 선택 사항입니다.
6. `bitmap(bitmap image, int color, int x, int y)`: 지정된 위치에 비트맵을 표시합니다.
7. `bitmap(bitmap image, int color, int x, int y, int w, int h)`: 지정된 상자 내부에 비트맵을 표시합니다.
8. `border(bool b)`: 창에 테두리를 추가하거나 제거합니다. 매개변수 없이 호출하면 창에 테두리가 있는지 여부를 반환합니다.
9. `circle(int x, int y, int r, int color)`: 원을 그립니다. `color` 매개변수는 선택 사항이며 원의 채우기 색상을 결정합니다.
10. `close()`: 창을 닫습니다.
11. `create(int x, int y, int w, int h, string title)`: 위젯이 없는 창을 생성합니다. `w`와 `h`는 선택 사항입니다.
12. `cursor(int cursortype, int color1, int color2)`: 커서 모양을 설정합니다. 커서 모양 목록은 아래에서 확인할 수 있습니다.
13. `drawcolor(int c)`: 다음 그림에 대한 색상을 설정합니다.
14. `drawtext(string l, int x, int y)`: 지정된 위치에 텍스트를 배치합니다.
15. `end()`: 창 생성을 종료합니다.
16. `flush()`: 모든 창을 다시 그립니다.
17. `resizable(object)`: 객체를 크기 조정 가능하게 만듭니다.
18. `rgbcolor(int color)`: 지정된 색상의 RGB 구성 요소 벡터를 반환합니다.
19. `rgbcolor(int r, int g, int b)`: RGB 구성 요소의 조합에 해당하는 색상을 반환합니다.
20. `rgbcolor(vector rgb)`: 벡터에 저장된 RGB 구성 요소의 조합에 해당하는 색상을 반환합니다.
21. `rotation(float x, float y, float distance, float angle, bool draw)`: 주어진 매개변수로부터 회전된 점의 좌표를 계산합니다. 새 좌표가 포함된 벡터를 반환합니다. `draw`가 true이면 선이 실제로 그려집니다.
22. `run()`: GUI를 시작합니다.
23. `scrollbar(int x, int y, int wscroll, int hscroll, int vwscroll, int vhscroll)`: 지정된 크기의 스크롤바 영역을 생성합니다. 이 영역 내에서 그리기를 수행하기 위해 창 콜백 함수가 필요합니다.
24. `size()`: 창 크기를 포함하는 벡터를 반환합니다.
25. `size(int x, int y, int w, int h)`: 창 크기를 조정합니다.
26. `sizerange(int minw, int minh, int maxw, int maxh)`: 창 크기가 변할 수 있는 범위를 정의합니다.
27. `textsize(string l)`: 문자열의 너비와 높이를 픽셀로 나타내는 맵을 반환합니다.
28. `unlock()`: FLTK를 잠금 해제합니다.
29. `onclose()`: 특별한 콜백 함수를 사용하여 창 닫기를 가로챕니다. 함수는 창을 닫는 동작을 처리해야 할 경우 true를 반환해야 합니다.
30. `ontime()`: t초마다 호출되는 함수를 정의합니다. 이 함수는 다음 매개변수를 가져야 합니다: `timeout_callback(window w, object o)`.

### 복잡한 모양 그리기

다음 지침은 이전 지침을 확장합니다. 복잡한 그리기 함수를 사용하면 2D 선형 변환을 사용하여 임의의 모양을 그릴 수 있습니다. 이 기능은 Adobe® PostScript™ 언어에서 찾을 수 있는 기능과 유사합니다. 채워지는 정확한 픽셀은 빠른 그리기 함수보다 정의되지 않으며, FLTK는 그리기 하드웨어를 활용할 수 있습니다. X와 MS Windows 모두에서 변환된 정점은 가까운 위치에 라운딩됩니다.

### 예제

```cpp
use("gui"); 

function closing(window w, bool close) { 
    if (close == false) { 
        println("이 창을 닫을 수 없습니다"); 
        return false; 
    } 
    return true; 
} 

window w; 
bool closed = false; 

w.begin(300, 200, 1300, 150, "수정"); 
w.onclose(closing, closed);  
```

### 색상

Tamgu LTK 라이브러리는 몇 가지 간단한 색상 선택 방법을 구현합니다. 색상은 정수로 표시됩니다.

미리 정의된 색상은 다음과 같습니다:

- FL_GRAY0
- FL_DARK3
- FL_DARK2
- FL_DARK1
- FL_LIGHT1
- FL_LIGHT2
- FL_LIGHT3
- FL_BLACK
- FL_RED
- FL_GREEN
- FL_YELLOW
- FL_BLUE
- FL_MAGENTA
- FL_CYAN
- FL_DARK_RED
- FL_DARK_GREEN
- FL_DARK_YELLOW
- FL_DARK_BLUE
- FL_DARK_MAGENTA
- FL_DARK_CYAN
- FL_WHITE

사용자 정의 색상을 정의하려면 Tamgu가 제공하는 `rgbcolor` 메서드를 사용할 수 있습니다. 다음과 같은 형식을 갖습니다:

- `vector rgb = rgbcolor(int c)`: 지정된 색상의 RGB 구성 요소를 포함하는 벡터를 반환합니다.
- `int c = rgbcolor(vector rgb)`: RGB 구성 요소를 포함하는 벡터를 가져와 해당하는 색상을 반환합니다.
- `int c = rgbcolor(int r, int g, int b)`: 위와 동일하지만 세 가지 구성 요소를 개별적으로 가져옵니다.