# 라이브러리 xml: 타입 xmldoc

이 타입은 XML 문서를 처리하는 데 사용됩니다. 새로운 XML 문서를 생성하거나 구문 분석하는 데 사용할 수 있습니다. 문서를 구문 분석할 때 xmldoc 변수와 함수를 연결하여 각 노드에 접근할 수 있습니다. 이 타입은 xml 라이브러리를 통해 액세스됩니다: `use("xml")`.

## 50.1 메소드

1. `close()`: 현재의 XML 문서를 닫고 모든 XML 값으로부터 메모리를 정리합니다.
2. `create(string topnode)`: topnode를 이름으로 가지는 새로운 XML 문서를 생성합니다. topnode가 완전한 XML 구조인 경우, 해당 구조를 사용하여 문서를 생성합니다.
3. `load(string filename)`: XML 파일을 로드합니다.
4. `node()`: 문서의 최상위 노드를 반환합니다.
5. `onclosing(function f, myobject o)`: 닫는 태그가 발견되었을 때 호출할 함수입니다 (아래의 연관 함수를 참조).
6. `parse(string buffer)`: XML 버퍼를 로드합니다.
7. `save(string filename, string encoding)`: XML 문서를 저장합니다. encoding이 생략된 경우, encoding은 "utf-8"입니다.
8. `serialize(object)`: Tamgu 객체를 XML 문서로 직렬화합니다.
9. `serializestring(object)`: Tamgu 객체를 XML 문서로 직렬화하고 해당하는 문자열을 반환합니다. 이 과정에서 문서도 정리됩니다.
10. `xmlstring()`: XML 문서를 문자열로 반환합니다.
11. `xpath(string myxpath)`: XPath를 평가하고 XML 노드의 벡터를 반환합니다.

## 50.2 연관 함수

연관 함수는 다음과 같은 서명을 가져야 합니다:

```lua
function xmlnode(xml n, object)
```

다음과 같이 선언되어야 합니다:

```lua
use("xml");
xmldoc mydoc(obj) with xmlnode;
```