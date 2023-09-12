# Synode (시노드)

시노드는 일련의 특징(맵으로 구현된)과 구성 트리에서의 위치에 의해 정의된 구문 노드입니다. 시노드 클래스는 다음과 같은 메서드를 제공합니다:

1. `initial(map m)`: 지정된 특징을 가진 구문 노드를 생성합니다.
2. `addchild(synode)`: 자식 노드를 추가합니다.
3. `addnext(synode)`: 현재 노드 다음에 노드를 추가합니다.
4. `addprevious(synode)`: 현재 노드 이전에 노드를 추가합니다.
5. `after(synode)`: 동일한 부모 아래에서 지정된 노드 뒤에 있는 경우 true를 반환합니다.
6. `attributes()`: 특징 속성을 벡터로 반환합니다.
7. `before(synode)`: 동일한 부모 아래에서 지정된 노드 앞에 있는 경우 true를 반환합니다.
8. `child()`: 첫 번째 자식 노드를 반환하거나 지정된 매개변수와 일치하는지 확인합니다.
9. `children()`: 주어진 노드의 자식 목록을 반환하거나 노드가 자식인지 확인합니다.
10. `definitions(map)`: 모든 시노드에 대한 유효한 특징 정의를 설정합니다.
11. `last()`: 마지막 자식 노드를 반환하거나 지정된 매개변수와 일치하는지 확인합니다.
12. `nbchildren()`: 직접 자식의 수를 반환합니다.
13. `next(synode)`: 다음 노드를 반환하거나 지정된 매개변수와 일치하는지 확인합니다.
14. `parent()`: 부모 노드를 반환하거나 지정된 매개변수와 일치하는지 확인합니다.
15. `previous(synode)`: 이전 노드를 반환하거나 지정된 매개변수와 일치하는지 확인합니다.
16. `precede(synode)`: 노드가 (트리 어디에서든) 지정된 노드 앞에 있는 경우 true를 반환합니다.
17. `sibling(synode)`: 노드가 형제 노드(자매 또는 자손)인지 확인합니다.
18. `sisters(synode)`: 자매 노드의 목록을 반환하거나 노드가 자매 노드인지 확인합니다.
19. `succeed(synode)`: 노드가 (트리 어디에서든) 지정된 노드 뒤에 있는 경우 true를 반환합니다.
20. `test(string attribute)`: 속성이 특징 구조의 일부인지 확인합니다.
21. `values()`: 특징 값들을 벡터로 반환합니다.

구성 트리 생성:
구성 트리는 위에서 아래로 구축됩니다. `addchild` 함수를 사용할 때, 현재 노드 아래에 자식 노드가 추가됩니다. 이 함수에 대한 연속적인 호출마다 이전 자식 노드 뒤에 새로운 자식이 추가됩니다.

예시:
```cpp
// NP 노드 생성
synode np({"pos": "np"});

// 세 개의 어휘 노드 생성
synode det({"pos": "det", "surface": "the"});
synode adj({"pos": "adj", "surface": "big"});
synode noun({"pos": "noun", "surface": "dog"});

// 어휘 노드들을 np 아래에 하나씩 추가
np.addchild(det);
np.addchild(adj);
np.addchild(noun);

// 들여쓰기된 방식으로 노드들을 출력
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 4);
    Display(x.next(), i);
}

Display(np, 0);
```

결과:
```
#0['pos':'np']
    #0['pos':'det','surface':'the']
    #0['pos':'adj','surface':'big']
    #0['pos':'noun','surface':'dog']
```

참고: "#0"은 시노드가 의존성 변수가 아님을 나타냅니다.