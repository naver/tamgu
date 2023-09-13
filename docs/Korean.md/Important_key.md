# 중요: 키 "#"

맵은 항상 "#" 키를 가져야하며, 이 키는 기본 스타일을 정의하는 데 사용됩니다. 이 키가 제공되지 않으면 예외가 발생합니다.

이 맵을 설계한 후에는 `addstyle(m)` 명령을 사용하여 시스템에 전달해야합니다.

텍스트의 일부에이 스타일을 사용하려면 `setstyle`을 사용하고 위의 키 중 하나를 선택하는 방법으로 올바른 스타일을 선택하십시오.

## 예제

```python
use("gui")

# 편집기 내에서 사용 가능한 스타일을 설명하는 맵
m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'A': [FL_BLUE, FL_COURIER_BOLD, FL_NORMAL_SIZE],
    'B': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'C': [FL_DARK_GREEN, FL_COURIER_ITALIC, FL_NORMAL_SIZE],
    'D': [FL_BLUE, FL_COURIER, FL_NORMAL_SIZE],
    'E': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE],
    'F': [FL_DARK_RED, FL_COURIER_BOLD, FL_NORMAL_SIZE]
}

w = window()
e = editor()

w.begin(300, 200, 1300, 700, "수정")
w.sizerange(10, 20, 0, 0)
e.create(200, 220, 1000, 200, "편집기")
e.addstyle(m)
e.value("이것은 흥미로운 스타일입니다.")
# "흥미로운"에 키 C의 스타일을 사용합니다.
e.setstyle(10, 22, 'C')
e.annotate("a", 'E') # 각 "a"에는 E 스타일이 지정됩니다.
w.end()
w.run()

# 스타일 수정

실제로 특정 편집기에 대해 스타일을 재정의 할 수 있습니다. `addstyle` 함수를 다시 호출해야합니다.

```python
# 편집기 내에서 사용 가능한 스타일을 설명하는 맵
m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'truc': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE]
}

# 맵에서 하나의 항목을 수정합니다. 같은 키를 유지합니다.
# 텍스트의 'truc'에 기반한 섹션이 수정됩니다.
function test(button b, editor e) {
    m["truc"] = [FL_DARK_GREEN, FL_COURIER, FL_NORMAL_SIZE];
    e.addstyle(m);
}

w = window()
e = editor()
b = button(e, test)

w.begin(300, 200, 1300, 700, "수정")
w.sizerange(10, 20, 0, 0)
e.create(200, 220, 1000, 200, "편집기")
e.addstyle(m)
e.value("이것은 흥미로운 스타일입니다.")
e.setstyle(10, 22, 'truc')
b.create(1230, 20, 30, 30, FL_Regular, FL_NORMAL_BUTTON, "확인")
w.end()
w.run()

# 스타일 메시지

특정 메시지와 스타일을 연결하는 것도 가능합니다. 이 메시지는 해당 스타일을 가진 요소 위로 마우스를 가져 가면 표시됩니다. 필요한 수정은 스타일 설명 각 항목에 하나 또는 두 개의 요소를 추가하는 것뿐입니다.

스타일 설명은 다음으로 구성됩니다: `[항목색상, 글꼴, 글꼴크기]`.

그 항목에 메시지를 추가 할 수 있습니다: `[항목색상, 글꼴, 글꼴크기, "메시지"]`.

그리고 메시지에 대한 배경색으로 사용 될 색상을 추가 할 수도 있습니다: `[항목색상, 글꼴, 글꼴크기, "메시지", 배경색상]`.

배경색상이 제공되지 않으면 스타일에서 정의된 색상 항목색상이 사용됩니다.

## 예제

```python
use("gui")

m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'truc': [FL_DARK_RED, FL_COURIER, FL_NORMAL_SIZE, "이것은 트뤼크입니다", FL_YELLOW]
}

# 스타일 truc을 가진 텍스트 위로 마우스를 가져 가면 노란색 상자와 메시지 THIS IS A TRUC가 표시됩니다.

# 콜백: 스크롤, 마우스 및 키보드

콜백은 다음과 같은 서명을 가져야합니다.

```python
w = window()
e = editor()
num = woutput()

w.begin(300, 200, 1300, 700, "창")
w.sizerange(10, 20, 0, 0)
num.create(100, 100, 30, 40, "라인")
e.create(200, 220, 1000, 200, "편집기")
e.onmouse(FL_RELEASE, cmouse, num)
e.onvscroll(cvscroll, num)
e.onkey(FL_KEYUP, ckey, num)
w.end()
w.run()

# 메모지

다음 예제는 편집기에서 특정 스타일을 가진 단어에 작은 메모지를 표시하는 방법을 보여줍니다.

```python
# 편집기 내에서 사용 가능한 스타일을 설명하는 맵
m = {
    '#': [FL_BLACK, FL_COURIER, FL_NORMAL_SIZE],
    'movement': [FL_RED, FL_COURIER, FL_NORMAL_SIZE]
}

# 텍스트에서 인식하려는 단어를 정의합니다.
mvt = ["이동", "실행", "걷기", "걷기", "운전"]

# 텍스트가 수정 될 때마다 위의 단어를 확인합니다.
function modified(editor e, int pos, int ninserted, int ndeleted, int restyled, string del, self obj) {
    # 먼저 모든 것을 해제합니다.
    e.setstyle(0, e.size(), "#")
    # 그런 다음 우리의 이동 단어를 모두 표시합니다.
    e.annotate(mvt, "movement")
}

w = window()
e = editor()

e.create(200, 220, 1000, 200, "편집기")
e.addstyle(m)
# 마우스 콜백도 필요합니다.
e.onmouse(vmouse, null)
w.end()
w.run()