## 특정 플래그: Private 및 Strict

함수는 두 가지 특정 플래그인 private와 strict를 함수 키워드 앞에 삽입하여 선언할 수도 있습니다.

**참고:**
동일한 정의에서 두 플래그를 모두 사용하려면, private가 strict보다 앞에 와야 합니다.

### Private [function | thread | autorun | polynomial]

함수가 private로 선언되면, 현재 Tamgu 파일 외부에서는 접근할 수 없습니다. Tamgu 프로그램이 다른 Tamgu 프로그램 내에서 로드된 경우, private 함수는 로더에서 접근할 수 없습니다.

**예시:**
```javascript
// 이 함수는 외부 로더에서는 보이지 않습니다...
private function toto(int i) {
    i += 10;
    return i;
}
```

### Strict [function | thread | polynomial]

기본적으로, Tamgu에서 함수가 선언될 때, 언어는 호출하는 함수의 각 인수를 함수 구현에서 예상하는 매개변수로 변환하려고 시도합니다. 그러나 이 메커니즘은 더 엄격한 매개변수 검사가 필요한 경우에는 너무 느슨할 수 있습니다. strict 플래그는 이 문제를 해결하는 데 도움이 되며, 이 플래그로 선언된 모든 함수는 엄격한 매개변수 제어를 요구합니다.

**예시:**
```javascript
strict function testStrictDeclaration(int s, int v) {
    println("Int:", s, v);
}

// 변수를 선언합니다
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// 이 경우, 시스템은 인수 목록에 따라 올바른 함수를 선택합니다...
testMultipleDeclaration(s1, s2); // 문자열 구현
testMultipleDeclaration(i, j); // 정수 구현
```