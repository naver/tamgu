# Tamgu 정규 표현식

Tamgu는 `treg`라는 고유한 정규 표현식 형식을 제공합니다. `treg`는 타입이자 정규 표현식을 작성하는 특정한 방법입니다. 타입으로 사용될 때는 아래에 설명된 대로 문자열을 인수로 사용하며, 문자열로 직접 제공할 수도 있습니다. 이 경우 문자열은 `r"..."` 형식을 따릅니다. 여기서 'r'은 정규 표현식을 나타냅니다.

## 메서드

`treg` 타입은 두 가지 메서드를 노출합니다.

1. `compile(string rgx)`: 문자열을 `treg` 객체로 컴파일합니다.
2. `match(string s)`: 문자열 `s`가 Tamgu 정규 표현식과 일치하는지 확인합니다.

예제:

```cpp
treg tt("%C+");
if (tt.match("ABCDE"))
    println("Yes");
if (tt == "aABCDE")
    println("Yes");
else
    println("NO");
```

## r 표현식

`r` 문자열을 사용하여 동일한 유형의 작업을 수행할 수도 있습니다.

```cpp
if (r"%C+" == "ABCDE")
    println("Yes");
```

`r` 표현식은 `""` 또는 `''` (이중 인용부호 또는 단일 인용부호)로 작성할 수 있습니다.

## 언어 설명

Tamgu 정규 표현식은 단어의 설명에 일정한 자유를 부여하기 위해 메타 문자를 사용하는 문자열입니다. 이 메타 문자는 다음과 같습니다.

- `%d`: 어떤 숫자를 나타냅니다.
- `%x`: 16진수 숫자를 나타냅니다 (abcdef0123456789ABCDEF).
- `%p`: 다음 집합에 속하는 모든 구두점을 나타냅니다: `< > { } [ ] ) , ; : . & | ! / \ = ~ # @ ^ ? + - * $ % ' _ ¬ £ €` “
- `%c`: 소문자 알파벳을 나타냅니다.
- `%C`: 대문자 알파벳을 나타냅니다.
- `%a`: 모든 알파벳을 나타냅니다.
- `?`: 어떤 문자를 나타냅니다.
- `%?`: 문자 "?" 자체를 나타냅니다.
- `%%`: 문자 "%" 자체를 나타냅니다.
- `%s`: 비중첩 공백 문자를 포함한 모든 공백 문자를 나타냅니다.
- `%r`: 캐리지 리턴을 나타냅니다.
- `%n`: 비중첩 공백을 나타냅니다.
- `~`: 부정
- `\x`: 이스케이프 문자
- `\ddd`: 정확히 3자리 숫자로 된 문자 코드
- `\xFFFF`: 정확히 4자리 16진수로 된 문자 코드
- `{a-z}`: a부터 z까지 (포함)
- `[a-z]`: 문자열
- `^`: 표현식은 문자열의 시작부터 시작해야 합니다.
- `$`: 표현식은 문자열의 끝까지 일치해야 합니다.

예제:

- `dog%c`는 `dogs` 또는 `dogg`와 일치합니다.
- `m%d`는 `m0`, `m1`, ..., `m9`와 일치합니다.

### 연산자: `*`, `+`, `()`, `([])`

정규 표현식은 Kleene-스타 표기법을 사용하여 여러 번 발생하는 문자를 정의할 수 있습니다.

- `x*`: 문자는 0회 이상 반복될 수 있습니다.
- `x+`: 문자는 적어도 한 번은 있어야 합니다.
- `(x)`: 문자는 선택적입니다.
- `[xyz](+*)`: 문자열의 순서
- `{xyz}(+*)`: 문자들의 선택

여기서 `x`는 문자나 메타 문자가 될 수 있습니다. `*`와 `+`에는 한 가지 특별한 경우가 있습니다. 반복될 문자가 어떤 문자든 상관없는 경우 `%+` 또는 `%*`를 사용해야 합니다.

중요: 이 두 규칙은 `?*` 또는 `?+`와도 동일합니다.

예제:

1. `a*ed`는 `aed`, `aaed`, `aaaed` 등과 일치합니다. (`a`는 0회 이상 발생할 수 있음)
2. `a%*ed`는 `aed`, `aued`, `auaed`, `aubased` 등과 일치합니다. (`a`와 `ed` 사이에는 어떤 문자든 발생할 수 있음)
3. `a%d*`는 `a`, `a1`, `a23`, `a45`, `a765735` 등과 일치합니다.
4. `a{%d %p}`는 `a1`, `a/`, `a` 등과 일치합니다.
5. `a{bef}`는 `ab`, `ae`, 또는 `af`와 일치합니다.
6. `a{%dbef}`는 `a1`, `ab`, `ae`, `af`, `a0`, `a9` 등과 일치합니다.
7. `a{be}+`는 `ab`, `ae`, `abb`, `abe`, `abbbe`, `aeeeb` 등과 일치합니다.
8. `a[be]+`는 `abe`, `abebe` 등과 일치합니다.

## 매크로: `grammar_macros(key, pattern)`

일부 표현식은 작성하기에 약간 복잡할 수 있습니다. Tamgu는 이러한 경우를 위해 `grammar_macros` 절차를 제공합니다. 이 절차는 표현식에서 사용할 수 있는 새로운 메타 문자를 생성합니다. 첫 번째 인수는 인덱스로 사용될 문자이고, 두 번째 인수는 이 키에 연결될 정규 표현식 패턴입니다.

이 함수는 문법 정규 표현식에 대한 모든 호출에 대한 패턴을 제공합니다.

예제:

```cpp
grammar_macros("X", "({%+ -})0x%x+(.%x+)(p({%+ -})%d+)");
string s = "ceci est: 0x1.0d0e44bfeec9p -3 dans la chaine.";
// 매크로 사용
string res = s.scan("%X");
println(res); // 0x1.0d0e44bfeec9p -3
```

중요: `grammar_macros`는 시스템 함수이므로 코드를 구문 분석하는 동안 실행됩니다. 컴파일이 완료된 후에는 코드의 나머지 부분과 함께 실행되지 않습니다.

## 문자열에서 `treg` 사용

문자열에서 `treg`를 직접 사용하여 내용을 추출하거나 수정하는 것도 가능합니다.

```cpp
string uu = "That was: 0x1.0d0e44bfeec9p -3, 0x3.456aebp -1 in here.";
print("Tst:<", uu[-r"%d":" "], ">\n"); // Tst:<1>
println("Tst2:", uu["x":r"%d %c"]); // Tst2: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

`r` 표현식을 변수 `treg`로 대체하는 것도 가능합니다.

```cpp
treg subt(r"%d %c");
println("Tst3:", uu["x":subt]); // Tst3: x1.0d0e44bfeec9p -3, 0x3.456aebp -1 i
```

## Taskell 사용

`treg` 표현식을 Taskell 함수와 함께 사용할 수도 있습니다.

```cpp
<mytreg(r"%C+") = "uppercase">
<mytreg([r"%d+":v]) = "ok">
<mytreg(v) = "missed">
println(mytreg("ABC")); // uppercase
println(mytreg(["3a4", 1, 4, 5])); // ok
```

## Prolog 사용

Prolog에서도 사용할 수 있습니다.

```cpp
tst(r"%d+"). // 여기에 `treg` 표현식이 있습니다...
tst("machin").
truc("Ok", ?P) :- tst(?P).
vector vpred;
vpred = truc(?P, "machin");
println(vpred); // [truc("Ok",machin)]
vpred = truc(?P, "12");
println(vpred); // [truc("Ok",12)]
vpred = truc(?P, "nope");
println(vpred); // []
```

## Posix 정규 표현식: `preg`

Tamgu에서는 posix 정규 표현식도 Tamgu 정규 표현식과 동일한 방식으로 사용할 수 있습니다. 그러나 두 가지 차이점이 있습니다. 먼저, 이러한 표현식을 사용하려면 표현식을 `r` 대신 `p`로 접두사로 붙여야 합니다. 두 번째로, 연관된 객체 유형은 `preg`입니다.

이 정규 표현식은 posix 정규 표현식 체계를 기반으로 합니다. `scan` 메서드를 제외한 모든 곳에서 `treg` 대신 사용할 수 있습니다.

예제:

```cpp
string str = "this subject has a submarine as a subsequence";
svector vs = p'\b(sub)([^ ]*)' in str;
println(vs); // ['subject', 'submarine', 'subsequence']
ivector iv = p'\b(sub)([^ ]*)' in str;
println(iv); // [5, 12, 19, 28, 34, 45]
string s = p'\b(sub)([^ ]*)' in str;
println(s); // subject
s = str[p"\b(sub)([^ ]*)"];
println("S=" + s); // S=subject
s = str[-p"\b(sub)([^ ]*)"];
println("S=" + s); // S=subsequence
```