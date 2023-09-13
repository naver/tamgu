## 규칙

규칙은 단일 텍스트로 구현할 수도 있고, 각 문자열이 규칙을 나타내는 문자열 벡터로 구현할 수도 있습니다.

### 규칙 형식

규칙의 형식은 다음과 같습니다:

```
head := (~) element [,;] element .
```

여기서 `element`는 다음과 같을 수 있습니다:
- `word := %a+`
- `number := %d+;$billion;$millions;$thousand`

### 예시

이 문법을 문장 "Test millions of cows"에 적용하면 다음과 같은 구조를 얻을 수 있습니다:

```
{
  'base': [
    {'word': ['Test']},
    ' ',
    {'number': ['millions']},
    ' ',
    {'word': ['of']},
    ' ',
    {'word': ['cows']}
  ]
}
```

### 복잡한 구조 인식

대문자로 시작하고 숫자가 뒤따르는 코드와 같은 복잡한 구조를 인식하려면 다음 문법을 구현할 수 있습니다:

```
string  r=@" 
base := _bloc+.  
_bloc  := code;word;number;%s.  
word := %a+.  
number := %d+.  
code := %C,%d+,%c.  
"@;
```

이 문법을 문장 "Test 123 T234e"에 적용하면 다음과 같은 구조를 얻을 수 있습니다:

```
{
  'base': [
    {'word': ['Test']},
    ' ',
    {'number': ['123']},
    ' ',
    {'code': ['T234e']}
  ]
}
```