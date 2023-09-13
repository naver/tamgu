# wtree와 wtreeitem

이 두 개체는 클릭 가능한 트리를 처리하는 데 사용됩니다. 첫 번째 개체는 트리 자체이며, 여러 wtreeitem 개체로 구성됩니다. 트리는 노드의 계층 구조로 표시되며, 각 노드는 콜백 함수를 통해 선택할 수 있습니다.

## wtree 메서드

1. `add(string path)`: 트리 아이템을 추가하고 새로운 wtreeitem을 반환합니다.
2. `add(wtreeitem e, string n)`: e 다음에 트리 아이템을 추가하고 새로운 wtreeitem을 반환합니다.
3. `clear()`: 모든 트리 아이템을 삭제합니다.
4. `clicked()`: 클릭된 요소를 반환합니다.
5. `close(string path)`: 지정된 요소를 닫습니다.
6. `close(wtreeitem e)`: 지정된 요소를 닫습니다.
7. `connectorcolor(int c)`: 커넥터 색상을 설정하거나 반환합니다.
8. `connectorstyle(int s)`: 커넥터 스타일을 설정하거나 반환합니다(아래 참조).
9. `connectorwidth(int s)`: 커넥터 너비를 설정하거나 반환합니다.
10. `create(int x, int y, int h, int w, string label)`: 트리를 생성합니다.
11. `find(string path)`: 지정된 경로와 일치하는 요소를 반환합니다.
12. `first()`: 첫 번째 요소를 반환합니다.
13. `insert(wtreeitem e, string label, int pos)`: e 다음에 지정된 위치에 지정된 레이블을 가진 요소를 삽입합니다.
14. `insertabove(wtreeitem e, string label)`: e 위에 지정된 레이블을 가진 요소를 삽입합니다.
15. `isclosed(string path)`: 지정된 요소가 닫혀 있는 경우 true를 반환합니다.
16. `isclosed(wtreeitem e)`: 지정된 요소가 닫혀 있는 경우 true를 반환합니다.
17. `itembgcolor(int c)`: 항목 배경색을 설정하거나 반환합니다.
18. `itemfgcolor(int c)`: 전경색을 설정하거나 반환합니다.
19. `itemfont(int c)`: 항목 글꼴을 설정하거나 반환합니다.
20. `itemsize(int c)`: 항목 글꼴 크기를 설정하거나 반환합니다.
21. `last()`: 마지막 요소를 wtreeitem으로 반환합니다.
22. `marginleft(int s)`: 위젯의 왼쪽 테두리와 트리 내용 사이에 나타나는 공백(픽셀) 양을 설정하거나 가져옵니다.
23. `margintop(int s)`: 위젯의 상단 테두리와 트리 내용의 상단 사이에 나타나는 공백(픽셀) 양을 설정하거나 가져옵니다.
24. `next(wtreeitem e)`: e 다음 요소를 wtreeitem으로 반환합니다.
25. `open(string path)`: 지정된 요소를 엽니다.
26. `open(wtreeitem e)`: 지정된 요소를 엽니다.
27. `previous(wtreeitem e)`: e 이전 요소를 wtreeitem으로 반환합니다.
28. `remove(wtreeitem e)`: 트리에서 지정된 요소를 제거합니다.
29. `root()`: 루트 요소를 wtreeitem으로 반환합니다.
30. `rootlabel(string r)`: 루트 레이블을 설정합니다.
31. `selectmode(int s)`: 선택 모드를 설정하거나 반환합니다(아래 참조).
32. `sortorder(int s)`: 정렬 순서를 설정하거나 반환합니다(아래 참조).

## wtreeitem 메서드

1. `activate()`: 현재 요소를 활성화합니다.
2. `bgcolor(int c)`: 항목 배경색을 설정하거나 반환합니다.
3. `child(int i)`: i 위치의 하위 요소를 반환합니다.
4. `children()`: 하위 요소의 수를 반환합니다.
5. `clean()`: 값과 연결된 개체를 제거합니다.
6. `deactivate()`: 현재 요소를 비활성화합니다.
7. `depth()`: 항목의 깊이를 반환합니다.
8. `fgcolor(int c)`: 전경색을 설정하거나 반환합니다.
9. `font(int c)`: 항목 글꼴을 설정하거나 반환합니다.
10. `fontsize(int c)`: 항목 글꼴 크기를 설정하거나 반환합니다.
11. `isactive()`: 요소가 활성화되어 있는 경우 true를 반환합니다.
12. `isclosed()`: 요소가 닫혀 있는 경우 true를 반환합니다.
13. `isopen()`: 요소가 열려 있는 경우 true를 반환합니다.
14. `isroot()`: 요소가 루트인 경우 true를 반환합니다.
15. `isselected()`: 요소가 선택된 경우 true를 반환합니다.
16. `label()`: 항목 레이블을 반환합니다.
17. `next()`: 다음 요소를 반환합니다.
18. `parent()`: 상위 요소를 반환합니다.
19. `previous()`: 이전 요소를 반환합니다.
20. `value()`: 개체와 연결된 값을 반환합니다.
21. `value(object)`: 항목과 값을 연결합니다.

## 콜백

wtree 개체에 콜백을 연결할 수 있습니다. 이 콜백은 다음 시그니처를 가져야 합니다:

```c
function wtree_callback(wtree t, wtreeitem i, int reason, myobject o);
```

이 함수는 트리에서 항목이 선택될 때마다 호출됩니다. `reason` 매개변수는 다음 값 중 하나를 가질 수 있습니다:

- `FL_TREE_REASON_NONE`: 알 수 없는 이유.
- `FL_TREE_REASON_SELECTED`: 항목이 선택되었습니다.
- `FL_TREE_REASON_DESELECTED`: 항목이 선택 해제되었습니다.
- `FL_TREE_REASON_OPENED`: 항목이 열렸습니다.
- `FL_TREE_REASON_CLOSED`: 항목이 닫혔습니다.

## 반복자

wtree 개체는 반복할 수 있습니다.

## 경로

`add` 또는 `find`와 같은 일부 함수는 경로가 필요합니다. 경로는 Unix 경로와 유사하며 루트에서 리프까지의 경로를 정의합니다. 예를 들어: "/Root/Top/subnode".

## 커넥터 스타일

노드 간의 연결자 스타일은 다음 플래그로 제어됩니다:

- `FL_TREE_CONNECTOR_NONE`: 항목을 연결하는 선을 사용하지 않습니다.
- `FL_TREE_CONNECTOR_DOTTED`: 항목을 연결하는 점선을 사용합니다(기본값).
- `FL_TREE_CONNECTOR_SOLID`: 항목을 연결하는 실선을 사용합니다.

## 선택 모드

트리에서 노드를 선택하는 방식은 다음 플래그로 제어됩니다:

- `FL_TREE_SELECT_NONE`: 항목을 클릭하면 아무것도 선택되지 않습니다.
- `FL_TREE_SELECT_SINGLE`: 항목을 클릭하면 하나의 항목만 선택됩니다(기본값).
- `FL_TREE_SELECT_MULTI`: 항목을 클릭하여 여러 항목을 선택할 수 있습니다.

## 정렬 순서

항목을 트리에 정렬된 방식으로 추가할 수 있습니다. 이는 다음 플래그로 제어됩니다:

- `FL_TREE_SORT_NONE`: 정렬 없음; 항목은 정의된 순서대로 추가됩니다(기본값).
- `FL_TREE_SORT_ASCENDING`: 항목을 오름차순으로 추가합니다.
- `FL_TREE_SORT_DESCENDING`: 항목을 내림차순으로 추가합니다.

## 예제

```c
use("gui");
FL_TREE_SELECT_NONE; // 항목을 클릭하면 아무것도 선택되지 않습니다.
FL_TREE_SORT_NONE; // 정렬 없음; 항목은 정의된 순서대로 추가됩니다(기본값).
```