## 다중 정의

Tamgu는 매개변수 정의가 다른 동일한 이름의 함수에 대해 다중 정의를 허용합니다. 예를 들어, `display(string s)`와 `display(int s)` 함수를 구현할 수 있습니다.

이 경우, 동일한 이름으로 여러 함수가 구현되면 매개변수 제어가 단일 구현에 비해 엄격해집니다. 시스템은 함수 호출의 인수 목록을 기반으로 가장 적합한 함수를 찾으려고 합니다. 결과적으로, 인수를 함수 매개변수에 맞는 값으로 변환하는 메커니즘이 더 이상 사용할 수 없게 됩니다.

### 예제

```javascript
function testMultipleDeclaration(string s, string v) {
    println("String:", s, v);
}

function testMultipleDeclaration(int s, int v) {
    println("Int:", s, v);
}

// 변수 선언
int i;
int j;
string s1 = "s1";
string s2 = "s2";

// 시스템은 인수 목록을 기반으로 적절한 함수를 선택합니다.
testMultipleDeclaration(s1, s2); // 문자열 구현을 호출합니다.
testMultipleDeclaration(i, j); // 정수 구현을 호출합니다.
```

위의 예제에서, `testMultipleDeclaration`에 전달된 인수에 따라 시스템은 적절한 함수를 선택합니다. 첫 번째 함수는 문자열 인수가 제공될 때 호출되며, 두 번째 함수는 정수 인수가 제공될 때 호출됩니다.