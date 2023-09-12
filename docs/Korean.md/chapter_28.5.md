## 입력은 문자열 또는 벡터입니다

입력이 문자열인 경우, 감지된 각 문자는 출력 문자열에 추가됩니다. 그러나 입력이 문자들의 벡터인 경우, 출력 결과는 문자들의 벡터로 유지됩니다.

### 예제

```python
# 이 문법은 단어 또는 숫자를 인식합니다
grammar = """
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"""

# 문법을 불러옵니다
g = Grammar(grammar)

# 문자열을 문자 벡터로 분할합니다
s = "Test 123 T234e"
vs = list(s)

# 문자 벡터에 문법을 적용합니다
v = g.apply(vs)

print(v)
```

이 경우 출력은 다음과 같습니다:

```python
['base', ['word', 'T', 'e', 's', 't'], ' ', ['number', '1', '2', '3'], ' ', ['code', 'T', '2', '3', '4', 'e']]
```