# 유형 문법

`grammar` 유형은 복잡한 문자열 구조를 설명하는 강력한 방법을 개발자에게 제공하기 위해 설계되었습니다. 텍스트에서 숫자, 대문자 또는 구두점이 엄격한 순서로 포함된 특정 하위 문자열을 감지하는 데 특히 유용합니다.

## 메소드

이 유형에서는 두 가지 함수가 노출됩니다:

1. `apply(string|vector)`: 텍스트에 문법을 적용하여 문자 또는 토큰의 벡터로 변환합니다.
2. `load(rule, int skipblanks)`: 규칙을 문자열 또는 규칙의 벡터로로드합니다. `skipblanks` 매개변수는 선택 사항이며 다음 값을 가질 수 있습니다:
   - 0: 문법에서 모든 문자가 고려됩니다. 이것은 `skipblanks`가 생략되었을 때의 기본값입니다.
   - 1: 대상을 하위 문자열에 적용하기 전에 공백과 탭이 자동으로 건너뛰어집니다. 문자열 끝의 불필요한 문자도 건너뜁니다.
   - 2: 개행을 포함한 모든 공백이 건너뜁니다.

문법과 함께 "in" 연산자를 사용하여 문자열이 문법과 호환되는지 여부를 감지할 수도 있습니다.

## 규칙

규칙은 단일 텍스트 또는 규칙의 벡터로 구현할 수 있으며, 각 문자열은 규칙을 나타냅니다.

### 규칙 형식

규칙의 형식은 다음과 같습니다:

```
head := (~) element [,;] element .
```

여기서 `element`는 다음과 같을 수 있습니다:
- 문자열: 따옴표나 작은 따옴표로 둘러싸여 있습니다.
- ? : 모든 문자.
- %a : 모든 알파벳 문자.
- %c : 모든 소문자 문자.
- %C : 모든 대문자 문자.
- %d : 숫자.
- %e : 이모지 문자.
- %H : 한글 문자.
- %r : 캐리지 리턴.
- %s : 공백 문자.
- %S : 구분 기호 문자 (공백 또는 캐리지 리턴).
- %p : 구두점.
- %? : "?" 문자.
- %% : "%" 문자.
- 0, 1, 2, ..., 9 : 문자 코드로 나타낸 임의의 숫자.
- $string : 임의 길이의 문자열 ("string"과 동일).
- head : 다른 규칙의 헤드.

부정: 헤드를 제외한 모든 요소는 "~"로 부정 할 수 있습니다.

분리: 두 요소 사이에 분리가 필요한 경우 ";"을 사용하고, 그렇지 않으면 ","를 사용합니다.

Kleene star: 각 요소에 대해 루프를 반복하려면 "+" 또는 "*"를 사용합니다.

가장 긴 일치: "++" 또는 "**"를 사용하면 루프가 가장 도달 가능한 요소까지 문자열을 소비합니다.

선택 사항: 선택 사항 문자 또는 헤드에 대해 "(element)"를 사용합니다.

모든 규칙은 "."로 끝나야 합니다.

헤드 이름이 "_"로 시작하는 경우 문자열은 추출되지만 레이블은 저장되지 않습니다.

특정 경우:
- ?_ : 문자, 저장하지 않음.
- %a_ : 알파벳 문자, 저장하지 않음.
- %c_ : 소문자 문자, 저장하지 않음.
- %C_ : 대문자 문자, 저장하지 않음.
- %d_ : 숫자, 저장하지 않음.
- %e_ : 이모지, 저장하지 않음.
- %H_ : 한글 문자, 저장하지 않음.
- %r_ : 캐리지 리턴, 저장하지 않음.
- %s_ : 공백 문자, 저장하지 않음.
- %S_ : 구분 기호 문자, 저장하지 않음.
- %p_ : 구두점, 저장하지 않음.
- label_ : 저장하지 않고 규칙을 호출합니다.

이러한 옵션의 끝에 "_"를 추가하면 최종 결과에 저장되지 않는 문자 또는 문자 그룹을 인식할 수 있습니다.

### 예시

```python
# 이 문법은 한 개의 문자열에 대해서만 단어 또는 숫자를 인식합니다.
string r = @"
bloc := word;number.
word := %a+.
number := %d+.
"@;

# 문법을 로드합니다.
grammar g(r);

# 문자열 "the"에 적용합니다.
map m = g.apply("the")
# 출력: {'bloc':[{'word':['the']}]}

m = g.apply("123")
# 출력: {'bloc':[{'number':['123']}]}

# 그러나 "Test 123"에 이 문법을 적용하면 실패합니다. 문법에 두 가지를 추가해야 합니다:
# a) 공백을 고려합니다.
# b) 문자열의 모든 토큰을 인식하기 위해 루프를 추가합니다.
string r = @"
base := bloc+.
bloc := word;number;%s.
word := %a+.
number := %d+.
"@;

# %s와 함께 새로운 분리를 추가했습니다. 그런 다음 bloc에서 루프를 하는 "base" 규칙을 추가했습니다.
m = g.apply("Test 123")
# 출력: {'base':[{'bloc':[{'word':['Test']}]},{'bloc':[' ']},{'bloc':[{'number':['123']}]}]}

# 출력에서 "bloc"과 같은 불필요한 정보를 제거하기 위해 "_" 연산자를 사용할 수 있습니다.
string r = @"
base := _bloc+.
_bloc := word;number;%s.
word := %a+.
number := %d+.
"@;

m = g.apply("Test 123")
# 출력: {'base':[{'word':['Test']},' ',{'number':['123']}]}
```

## 하위 문법

하위 문법은 "[...]" 내에서 소개됩니다. 이 괄호 내에서는 문자 정규식 문자열의 분리를 정의할 수 있습니다. 이러한 표현식은 특히 문자열의 벡터에 문법을 적용할 때 문자 수준에서 문자열과 일치할 수 있습니다. 각 표현식은 "|"로 다음 표현식과 구분되어야 합니다. 괄호 내에서 규칙을 호출할 수 없으므로 "dog"와 같은 문자열은 "$dog"와 동등합니다.

### 예시

```python
string dico = @"
test := %a, wrd,%a.
wrd := [%C,(" -"),%c+|test|be|dog|cat].
"@;

grammar g(dico);
ustring s = "The C -at drinks";
uvector v = s.tokenize();
vector res = g.apply(v);
println(res);
```

## 벡터 대 맵

수신 변수를 벡터로 대체하면 출력 구조가 다릅니다. 헤드 규칙 이름이 최종 구조의 첫 번째 요소로 삽입됩니다. 예를 들어, 동일한 문자열에 대해 문법을 적용하지만 출력이 벡터로 대체되면 `['base',['word','Test'],' ',['number','123'],' ',['code','T234e']]`를 얻습니다.

## 입력은 문자열 또는 벡터입니다

입력이 문자열인 경우 감지된 각 문자가 출력 문자열에 추가됩니다. 그러나 입력이 문자의 벡터인 경우 출력 결과는 문자의 벡터로 유지됩니다.

### 예시

```python
# 이 문법은 단어 또는 숫자를 인식합니다.
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"@;

# 문법을 로드합니다.
grammar g(r);

# 문자열을 문자 벡터로 분할합니다.
string s = "Test 123 T234e";
svector vs = s.split("");

# 문자 벡터에 문법을 적용합니다.
vector v = g.apply(vs);
println(v);
```

## 함수

문법과 함수를 연결할 수 있습니다. 함수의 시그니처는 다음과 같습니다: `function grammarcall(string head, self structure, int pos)`. 이 함수는 주어진 헤드에 대해 계산된 각 새 구조에 대해 호출됩니다. 이 함수가 false를 반환하면 해당 규칙의 분석이 실패합니다. `pos`는 구문 분석이 진행된 문자열의 마지막 위치입니다.

### 예시

```python
# 이 문법은 단어 또는 숫자를 인식합니다.
string r = @"
base := _bloc+.
_bloc := code;word;number;%s.
word := %a+.
number := %d+.
code := %C,%d+,%c.
"@;

# 이 함수는 성공한 각 새 규칙에 대해 호출됩니다.
function callgrm(string head, self v, int ps) {
    println(head,v,ps);
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

## 구조의 수정

문법과 연결된 함수에서 구조를 수정할 수도 있지만 수정할 때 주의가 필요합니다.

### 예시

```python
function callgrm(string head, self v, int ps) {
    # 헤드가 단어인 경우 내부 문자열을 수정합니다.
    if (head == "word") {
        println(head,v);
        v[0] += "_aword";
    }
    return true;
}
```

## 규칙 내에서

규칙 내에서 함수를 호출할 수도 있습니다. 시그니처는 다음과 같습니다: `function rulecall(self structure, int pos)`.

### 예시

```python
# 이 문법은 단어 또는 숫자를 인식합니다.
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

## HTML 구문 분석

```python
# evalue는 모든 HTML 엔티티를 해당 UTF8로 대체하는 기본 메소드입니다.
function evalue(self s, int p) {
    s[1] = s[1].evaluate();
    return true;
}

# 이것은 HTML 문법입니다.
# 태그 사이의 공백 문자를 유지하지 않으므로 %s_로 object에 있습니다.
string htmlgrm = @"
html := _object+.
_object := tag;%s_;text.
tag := "<",?+,">".
text := _characters,evalue.
_characters := ~"<"+.
"@;

# 문법을 컴파일합니다.
grammar ghtml(htmlgrm);

# HTML 텍스트에 문법을 적용합니다.
vector rgram = ghtml.apply(html_text);
```