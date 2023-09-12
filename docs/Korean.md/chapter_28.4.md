## 하위 문법

하위 문법은 대괄호 내에서 소개됩니다. 이 대괄호 내에서 문자 정규식 문자열의 선택을 정의할 수 있습니다. 이러한 표현식은 문자열을 문자 수준에서 표현식과 일치시키는 경우에 특히 유용합니다. 각 표현식은 "|"로 다음 표현식과 분리되어야 합니다.

대괄호 내에서 규칙을 호출할 수 없으므로 "dog"와 같은 문자열은 "$dog"와 동일합니다.

예시:

```python
string dico = @"
test := %a, wrd, %a.
wrd := [%C, (" - "), %c+ | test | be | dog | cat].
"@;

grammar g(dico);
ustring s = "The C -at drinks";
uvector v = s.tokenize();
vector res = g.apply(v);
println(res);
```

## 벡터 vs. 맵

수신 변수를 벡터로 대체하면 출력이 다릅니다. 최종 구조에는 헤드 규칙 이름이 첫 번째 요소로 삽입됩니다. 따라서 위의 문법을 동일한 문자열에 대해 벡터로 출력으로 적용하면 다음과 같이 얻을 수 있습니다:

```python
['base', ['word', 'Test'], ' ', ['number', '123'], ' ', ['code', 'T234e']]
```