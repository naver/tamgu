# 라이브러리 xml: 탐구에서 xmldoc 타입

이 라이브러리는 탐구에서 XML 문서를 처리하는 기능을 제공합니다. 이를 통해 새로운 XML 문서를 생성하고 기존 문서를 파싱하며 다양한 작업을 수행할 수 있습니다. 라이브러리는 `xml` 모듈을 사용하여 액세스할 수 있습니다.

## 메서드

1. `close()`: 현재 XML 문서를 닫고 모든 XML 값에 대한 메모리를 해제합니다.
2. `create(string topnode)`: 지정된 최상위 노드로 새로운 XML 문서를 생성합니다. `topnode` 매개변수가 완전한 XML 구조인 경우 문서 생성에 사용됩니다.
3. `load(string filename)`: XML 파일을 로드합니다.
4. `node()`: 문서의 최상위 노드를 반환합니다.
5. `onclosing(function f, myobject o)`: 파싱 중에 닫는 태그가 발견될 때 호출될 `xmldoc` 변수와 함수 `f`를 연결합니다.
6. `parse(string buffer)`: XML 버퍼를 파싱합니다.
7. `save(string filename, string encoding)`: XML 문서를 파일로 저장합니다. `encoding` 매개변수가 생략된 경우 인코딩은 "utf-8"로 설정됩니다.
8. `serialize(object)`: Tamgu 객체를 XML 문서로 직렬화합니다.
9. `serializestring(object)`: Tamgu 객체를 XML 문서로 직렬화하고 해당하는 문자열을 반환합니다. 이 과정에서 문서도 정리됩니다.
10. `xmlstring()`: XML 문서를 문자열로 반환합니다.
11. `xpath(string myxpath)`: XPath 식을 평가하고 XML 노드의 벡터를 반환합니다.

## 연결된 함수

연결된 함수는 다음과 같은 서명을 가져야 합니다: