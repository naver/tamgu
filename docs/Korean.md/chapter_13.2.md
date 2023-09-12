## 방법

`treg` 타입은 두 가지 메소드를 제공합니다:

1. `compile(string rgx)`: 문자열을 `treg` 객체로 컴파일합니다.
2. `match(string s)`: 문자열 `s`가 Tamgu 정규 표현식과 일치하는지 확인합니다.

예시:

```cpp
treg tt("%C+");
if (tt.match("ABCDE"))
    println("Yes");
if (tt == "aABCDE")
    println("Yes");
else
    println("No");
```

동일한 유형의 작업을 수행하기 위해 `r` 문자열을 사용할 수도 있습니다:

```cpp
if (r"%C+" == "ABCDE")
    println("Yes");
```

`r` 표현식은 큰따옴표(`""`)나 작은따옴표(`''`)로 작성할 수 있습니다.