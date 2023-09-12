# Tamgu 편집기

Tamgu ltk는 텍스트 편집을 처리하기 위한 특정 유형을 제공합니다. 이 편집기는 사용자가 텍스트를 수정할 수 있도록 하며, 텍스트가 수정될 때마다 트리거되는 콜백 함수를 제공합니다.

## 편집기 콜백 함수

편집기 콜백 함수는 다음과 같은 형식을 갖습니다:

```cpp
void editorcallback(editor e, int pos, int ninserted, int ndeleted, int restyled, string del, myobj obj);
```

콜백 함수의 인수는 다음과 같습니다:

- `editor e`: 편집기 객체 자체
- `pos`: 문서에서 현재 커서 위치
- `ninserted`: 삽입된 문자 수
- `ndeleted`: 삭제된 문자 수
- `restyled`: 스타일이 변경된 문자 수
- `del`: 삭제된 문자
- `obj`: `with` 명령을 통해 편집기와 연결된 객체

편집기 콜백 함수는 `with` 명령을 사용하여 편집기 객체와 연결됩니다:

```cpp
editor e(obj) with editorcallback;
```

## 편집기 메서드

편집기 객체는 다음 메서드를 노출합니다:

1. `addstyle(map styles)`: 텍스트 청크에 대한 스타일을 초기화합니다. `styles` 인수는 편집기 내에서 사용 가능한 스타일을 설명하는 맵입니다.
2. `annotate(string s, string keystyle, bool matchcase)`: 텍스트에서 문자열 `s`의 각 발생에 특정 스타일(`keystyle`)을 할당합니다. `matchcase` 매개변수는 선택 사항입니다. `s`가 정규 표현식인 경우 `matchcase` 매개변수는 무시됩니다.
3. `append(string s)`: 편집기 텍스트의 끝에 문자열을 추가합니다.
4. `byteposition(int pos)`: 문자 위치를 바이트 위치로 변환합니다. 특히 UTF8 문자열에 유용합니다.
5. `charposition(int pos)`: 바이트 위치를 문자 위치로 변환합니다. 특히 UTF8 문자열에 유용합니다.
6. `color(int c)`: 텍스트 색상을 설정하거나 반환합니다.
7. `copy()`: 선택된 텍스트를 클립보드에 복사합니다.
8. `copy(string s)`: 문자열 `s`를 클립보드에 복사합니다.
9. `count(string s, int bg)`: 위치 `bg`에서 시작하는 문자열 `s`의 발생 횟수를 계산합니다.
10. `create(int x, int y, int w, int h, string label)`: 편집기를 생성합니다.
11. `cursor()`: 커서의 현재 위치를 바이트 단위로 반환합니다.
12. `cursor(int i)`: 커서를 `i`번째 바이트로 이동합니다.
13. `cursorchar()`: 커서의 현재 위치를 문자 단위로 반환합니다.
14. `onhscroll(function f, object o)`: 가로 스크롤링 시 콜백을 설정합니다.
15. `onkey(int action, function f, object o)`: 세로 스크롤링 시 콜백을 설정합니다.
16. `onmouse(int action, function f, object o)`: 마우스 처리 시 콜백을 설정합니다.
17. `onvscroll(function f, object o)`: 세로 스크롤링 시 콜백을 설정합니다.
18. `paste()`: 클립보드에서 선택된 텍스트를 붙여넣습니다.
19. `rfind(string s, int i)`: 위치 `i`에서 시작하여 편집기 텍스트에서 문자열을 찾습니다.
20. `save(string filename)`: 편집기의 내용을 파일로 저장합니다.
21. `selection()`: 편집기에서 선택된 텍스트를 반환합니다.
22. `setstyle(int start, int end, string keystyle)`: `addstyle`로 인스턴스화된 스타일 테이블에서 주어진 스타일로 텍스트 청크를 설정합니다.
23. `unhighlight()`: 하이라이팅을 제거합니다.
24. `value(string v)`: 편집기의 텍스트를 반환하거나 주어진 문자열로 편집기를 초기화합니다.
25. `word(int pos)`: 위치 `pos`에서 단어를 반환합니다.
26. `wrap(bool w)`: `w`가 `true`인 경우 텍스트를 편집기 창 내에 자동 줄 바꿈합니다.

## 커서 스타일

Tamgu는 `cursorstyle` 메서드를 사용하여 설정할 수 있는 다양한 커서 스타일을 제공합니다:

- `FL_NORMAL_CURSOR`
- `FL_CARET_CURSOR`
- `FL_DIM_CURSOR`
- `FL_BLOCK_CURSOR`
- `FL_HEAVY_CURSOR`

## 스타일 추가

편집기에서 특정 텍스트 섹션을 다른 글꼴, 색상 및 크기로 표시하려면, FLTK에서 이러한 스타일에 대한 설명이 미리 필요합니다. 각 스타일은 `[color, font, size]`의 세 요소로 표시되며, 해당 스타일 항목을 참조하기 위해 사용되는 키와 연결됩니다.

다음은 편집기 내에서 사용 가능한 스타일을 설명하는 맵의 예입니다:

```cpp
map m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'A': [FL_BLUE, FL_COURIER_BOLD, FL_NORMAL_SIZE],
    'B': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'C': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'D': [FL_BLUE, FL_COURIER, FL_NORMAL_SIZE],
    'E': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE],
    'F': [FL_DARK_RED, FL_COURIER_BOLD, FL_NORMAL_SIZE]
};
```

Tamgu 애플리케이션에서 이러한 메서드와 스타일을 사용하는 방법에 대한 자세한 내용은 FLTK 문서를 참조하십시오.