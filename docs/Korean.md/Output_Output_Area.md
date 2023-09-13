# 출력 (출력 영역)

이 클래스는 창 내에서 특정 출력 영역을 생성하는 데 사용됩니다. 다음과 같은 메서드를 제공합니다:

## 메서드

1. `color(int c)`: 텍스트 색상을 설정하거나 반환합니다.
2. `create(int x, int y, int w, int h, boolean multiline, string label)`: `multiline` 매개변수가 `true`로 설정되면 여러 줄 기능이 있는 출력 영역을 생성합니다.
3. `font(string s)`: 텍스트 글꼴을 설정하거나 반환합니다.
4. `fontsize(int c)`: 텍스트 글꼴 크기를 설정하거나 반환합니다.
5. `value(string v)`: 버퍼를 초기화합니다.