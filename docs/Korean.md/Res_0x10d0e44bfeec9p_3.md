## 결과: 0x1.0d0e44bfeec9p -3

```python
println("결과:", 결과);
```

출력은 다음과 같습니다: `결과: 0x1.0d0e44bfeec9p -3`.

## IV [6, 25, 26, 34]

```python
ivector iv = s.scan("%X");
println("IV", iv);
```

출력은 다음과 같습니다: `IV [6, 25, 26, 34]`.

## VS ['0x1.0d0e44bfeec9p -3', '0x2.09p3', '0x3.456aebp -1']

```python
svector vs = s.scan("%X");
println("VS", vs);
```

출력은 다음과 같습니다: `VS ['0x1.0d0e44bfeec9p -3', '0x2.09p3', '0x3.456aebp -1']`와 3개의 요소가 있습니다.

## VS ['0x1.0d0e44bfeec9p -3', '0x2.09p3']

```python
vs = s.scan("%X %X", " ");
println("VS", vs);
```

출력은 다음과 같습니다: `VS ['0x1.0d0e44bfeec9p -3', '0x2.09p3']`와 2개의 요소가 있습니다.

```python
string reste;
fvector fv = s.scan("%X, %X", ",", false, reste);
println("FV", fv, reste);
```

출력은 다음과 같습니다: `FV [0.131375, 16.2812] 0x3.456aebp -1 in here.`

treg 문자열을 사용할 것인지 아닌지?

지금까지 보여준 모든 예제에서 `scan`은 문자열을 입력으로 받아 이를 treg로 컴파일합니다. 그러나 `scan`의 첫 번째 매개변수로 문자열 대신 treg를 제공할 수도 있습니다. treg가 r 문자열로 제공되면 실행 시간이 아닌 구문 분석 시간에 컴파일됩니다. 이렇게 사전 컴파일하는 것은 실행 시간에 문자열 대신 treg를 사용하는 데 약간의 이점을 제공합니다.

토큰화 규칙

`parenthetic`, `tags`, `tokenization` 메서드는 모두 기본 토큰화 규칙 세트를 사용하며, 이는 `rules` 매개변수를 통해 수정할 수 있습니다. 기본 토큰화 규칙 세트는 `_getdefaulttokenizerules`를 사용하여 로드하고 수정할 수 있으며, 이를 통해 토큰화 과정을 변경하거나 향상시킬 수 있습니다.

- `x-`는 문자를 인식하지만 구문 분석 문자열에 저장하지 않아야 함을 의미합니다.
- `%.~..`는 물결표 뒤의 목록을 제외한 모든 문자를 인식함을 의미합니다.

중요: 공백을 추가하지 마십시오. 공백은 테스트할 문자로 간주됩니다.

예시:

```python
svector rules = _getdefaulttokenizerules();
rules.insert(55, "{%a %d}+ : {%a %d}+=0"); // aaa : bbb는 이제 하나의 토큰입니다.
rules.insert(55, "{%a %d}+.{%a %d}+=0"); // aaa.bbb는 이제 하나의 토큰입니다.
rules.insert(38, "->=0"); // ->는 하나의 토큰입니다.

string s = "this is a test.num -> x : 10";
// 규칙 없이
v = s.tokenize(); // ['this', 'is', 'a', 'test', '.', 'num', '-', '>', 'x', ':', '10']
// 규칙을 사용하여
v = s.tokenize(false, false, rules); // ['this', 'is', 'a', 'test.num', '->', 'x : 10']
```

`parenthetics()` 또는 `parenthetics(string opening, string closing)`

Tamgu는 다음과 같은 괄호 표현식을 해독할 수도 있습니다:

```
((S (NP -SBJ Investors)
     (VP are
         (VP appealing
             (PP-CLR to
      (NP-1 the Securities))
             (S-CLR (NP -SBJ * -1)
     not
     (VP to
 (VP limit
     (NP (NP their access)
 (PP to
     (NP (NP information)
 (PP about
     (NP (NP stock purchases)
 (PP by
     (NP "insiders”)
 ))))))))))).))
```

Tamgu는 위와 같은 구조를 벡터로 변환하는 `parenthetics` 메서드를 제공합니다.

```python
vector v = s.parenthetics(); // s에 위와 같은 괄호 표현식이 들어 있음
```

두 번째 함수는 다른 여는 또는 닫는 문자를 사용할 수 있도록 합니다.

예시:

Tamgu는 다음 구조를 분석할 수 있습니다:

```
<S <NP -SBJ They>
     <VP make
         <NP the argument>
  <PP-LOC in
  <NP <NP letters>
      <PP to
  <NP the agency>> > > > > .>
```

다음 명령문을 사용하여:

```python
vector v = s.parenthetics('<', '>');
```

`tags(string opening, string closing)`

`tags`는 `parenthetics` 메서드와 유사하지만, 문자 대신 문자열을 입력으로 사용합니다. XML 출력을 구문 분석하는 데는 `xmldoc`를 사용해야 합니다.

```python
string s = "OPEN This is OPEN a nice OPEN example CLOSE CLOSE CLOSE";
vector v = s.tags('OPEN ', 'CLOSE ');
```

출력: `v = [['this', 'is', ['a', 'nice ', ['example']]]]`.