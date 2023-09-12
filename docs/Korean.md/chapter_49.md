# xml 라이브러리: Tamgu에서 xml 작성하기

`xml` 타입은 XML 노드를 처리하는 메소드를 제공합니다.

**중요:** 이 타입은 libxml2 라이브러리의 `xmlNodePtr` 타입을 위한 플레이스홀더로 구현되었습니다 (자세한 내용은 [http://xmlsoft.org/](http://xmlsoft.org/)를 참조하십시오). 따라서 현재 변수에 대한 새 객체를 생성하기 위해 `new` 메소드가 필요합니다.

이 타입은 `xml` 라이브러리를 통해 액세스할 수 있습니다: `use("xml");`

## 메소드

1. `child()`: 현재 노드 아래의 첫 번째 자식 노드를 반환합니다.
2. `child(xml)`: XML 노드를 자식으로 추가합니다.
3. `content()`: 노드의 내용을 반환합니다.
4. `content(string n)`: 노드의 내용을 변경합니다.
5. `delete()`: 현재 내부 노드를 삭제합니다.
6. `line()`: 현재 노드의 줄 번호를 반환합니다.
7. `id()`: 현재 노드의 ID를 반환합니다 (호출 함수와 함께만 사용 가능).
8. `name()`: XML 노드 이름을 반환합니다.
9. `name(string n)`: XML 노드 이름을 변경합니다.
10. `namespace()`: 현재 노드의 네임스페이스를 벡터로 반환합니다.
11. `new(string n)`: 새로운 내부 노드를 생성합니다.
12. `next()`: 다음 XML 노드를 반환합니다.
13. `next(xml)`: 현재 노드 뒤에 XML 노드를 추가합니다.
14. `parent()`: 현재 노드 위의 부모 노드를 반환합니다.
15. `previous()`: 이전 XML 노드를 반환합니다.
16. `previous(xml)`: 현재 노드 앞에 XML 노드를 추가합니다.
17. `properties()`: XML 노드의 속성을 반환합니다.
18. `properties(map props)`: 속성은 속성/값으로 맵에 저장됩니다.
19. `root()`: XML 트리의 루트 노드를 반환합니다.
20. `xmlstring()`: XML 서브트리를 문자열로 반환합니다.
21. `xmltype()`: XML 노드의 타입을 문자열로 반환합니다 (XML 노드 이름).

## 예제

```tamgu
use("xml");

function Test(xml n, self nn) {
    map m = n.properties();
    println(n.name(), m, n.content());
}

xmldoc doc with Test;
doc.load("resTamgu.xml");
xml nd = doc.node();
println(nd);

while (nd != null) {
    println(nd.content(), nd.namespace());
    nd = nd.child();
}

xmldoc nouveau;
nouveau.create("TESTAGE");
xml nd = nouveau.node();
xml n("toto");
nd.child(n);
n.new("titi");
n.content("Toto is happy");
nd.child(n);
nouveau.save("mynewfile.xml");
```