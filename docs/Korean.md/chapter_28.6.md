# 기능

문법과 함수를 연결할 수도 있습니다. 함수의 서명은 다음과 같습니다:

```python
function grammarcall(string head, self structure, int pos)
```

이 함수는 주어진 헤드에 대해 계산된 각 새로운 구조에 대해 호출됩니다. 이 함수가 false를 반환하면 해당 규칙의 분석이 실패합니다. `pos`는 파싱이 수행된 문자열의 마지막 위치입니다.

## 예제

```python
# 이 문법은 단어나 숫자를 인식합니다.
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"@;

# 이 함수는 성공한 각 규칙마다 호출됩니다.
function callgrm(string head, self v, int ps) {
    println(head, v, ps);
    return true;
}

# 문법을 로드합니다.
grammar g(r) with callgrm;

# 문자열을 문자 벡터로 분할합니다.
string s = "Test 123 T234e";

# 문자 벡터에 문법을 적용합니다.
map m = g.apply(s);
println(m);
```

결과:

```
word ['Test']
_bloc [{'word':['Test']}]
_bloc [' ']
number ['123']
_bloc [{'number':['123']}]
_bloc [' ']
code ['T234e']
_bloc [{'code':['T234e']}]
{'base':[{'word':['Test']},' ',{'number':['123']},' ',{'code':['T234e']}]}
```

## 구조의 수정

이 함수에서 구조를 수정할 수도 있지만, 수정할 때 주의해야 합니다.

```python
function callgrm(string head, self v, int ps) {
    # head가 단어인 경우 내부 문자열을 수정합니다.
    if (head == "word") {
        println(head, v);
        v[0] += "_aword";
    }
    return true;
}
```

이 경우 출력은 다음과 같습니다:

```
word ['Test']
{'base':[{'word':['Test_aword']},' ',{'number':['123']},' ',{'code':['T234e']}]}
```

## 규칙 내부에서 호출하기

함수는 규칙 내부에서도 호출될 수 있습니다. 서명은 다음과 같습니다:

```python
function rulecall(self structure, int pos)
```

이 함수는 코드 규칙 내부에서 호출됩니다. 반환 값이 false이면 코드 규칙이 실패합니다.

```python
function callcode(self v, int ps) {
    println(head, v);
    return true;
}

# 이 문법은 단어나 숫자를 인식합니다.
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c,callcode.
"@;

# 문법을 로드합니다.
grammar g(r);

# 문자열을 문자 벡터로 분할합니다.
string s = "Test 123 T234e";

# 문자 벡터에 문법을 적용합니다.
map m = g.apply(s);
println(m);
```

## 예제: HTML 파싱

`evaluate`는 모든 HTML 엔티티를 해당 UTF8 상응물로 대체하는 기본 메서드입니다.