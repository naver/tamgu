# wtable

`Tamgu ltk` 라이브러리는 특정 유형을 제공하여 테이블에 값을 표시하고 일부 요소를 선택할 수 있습니다. 이 테이블은 다음과 같은 시그니처를 가진 콜백 함수로 생성해야 합니다:

```lua
function callback_table(table x, map values, myobject obj)
```

테이블을 생성하려면 다음 구문을 사용하세요:

```lua
table t(obj) with callback_table
```

`values` 매개변수는 다음과 같은 키를 포함하는 맵입니다:

- `"top"`: 맨 위 행
- `"bottom"`: 맨 아래 행
- `"left"`: 왼쪽 열
- `"right"`: 오른쪽 열
- `"values"`: 행과 열을 "r:c" 형식의 문자열로 키로 하는 맵입니다.

이 객체는 다음 메서드를 노출합니다:

## 메서드

1. `add(int R, int C, string v)`: 행 `R`과 열 `C`에 값을 추가합니다. 테이블의 크기는 추가된 값의 수에 따라 달라집니다.
2. `boxtype(int boxtype)`: 상자 유형을 설정합니다.
3. `cell(int R, int C)`: 행 `R`과 열 `C`의 값을 반환합니다.
4. `cellalign(align)`: 셀 내에서 정렬을 설정합니다.
5. `cellalignheaderrow(align)`: 행 헤더 셀에서 정렬을 설정합니다.
6. `cellalignheadercol(align)`: 열 헤더 셀에서 정렬을 설정합니다.
7. `clear()`: 테이블을 지웁니다.
8. `colorbg(int c)`: 셀 배경색을 설정하거나 반환합니다.
9. `colorfg(int c)`: 셀 전경색을 설정하거나 반환합니다.
10. `column()`: 열의 수를 반환합니다.
11. `column(int nb)`: 열의 수를 정의합니다.
12. `columnheader(int C, string label)`: 열 `C`의 열 헤더 레이블을 설정합니다.
13. `columnheaderwidth(int sz)`: 열 헤더의 크기(픽셀 단위)를 설정합니다.
14. `columnwidth(int width)`: 열의 너비(픽셀 단위)를 정의합니다.
15. `create(int x, int y, int w, int h, string label)`: 객체의 테이블을 생성하고 추가 작업을 시작합니다.
16. `font(int s)`: 텍스트 글꼴을 설정하거나 반환합니다.
17. `fontsize(int c)`: 텍스트 글꼴 크기를 설정하거나 반환합니다.
18. `row()`: 행의 수를 반환합니다.
19. `row(int nb, int height)`: 행의 수를 정의합니다.
20. `rowheader(int R, string label)`: 행 `R`의 행 헤더 레이블을 설정합니다.
21. `rowheaderheight(int sz)`: 행 헤더의 크기(픽셀 단위)를 설정합니다.
22. `rowheight(int height)`: 행의 높이(픽셀 단위)를 정의합니다.
23. `selectioncolor(int color)`: 선택된 요소의 색상을 설정합니다.
24. `when(string when)`: 콜백을 트리거하는 이벤트 유형을 정의합니다.

## 예시

```lua
use("gui")
window w

function callback_table(wtable x, map V, window w)
    println(V)
end

wtable t(w) with callback_table

int i, j

-- 창 생성
w.begin(300, 200, 1300, 500, "Fenetre")

-- 테이블 생성
t.create(20, 20, 500, 400, "table")

-- 글꼴 크기 설정
t.fontsize(12)

-- 선택된 요소의 색상을 파란색으로 설정
t.selectioncolor(FL_BLUE)

-- 테이블 채우기
for i = 0, 10 do
    -- 행 헤더 설정
    t.rowheader(i, "R" .. i)
    -- 열 헤더 설정
    t.columnheader(i, "C" .. i)
    for j = 0, 10 do
        -- 테이블에 값 추가
        t.add(i, j, "R" .. i .. "C" .. j)
    end
end

-- 테이블 채우고 난 후 행 높이 설정
t.rowheight(20)

-- 테이블 채우고 난 후 열 너비 설정
t.columnwidth(60)

w.end()
w.run()
```