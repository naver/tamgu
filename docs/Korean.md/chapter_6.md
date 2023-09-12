# Tamgu에서의 Alias

Tamgu에서의 Alias는 함수와 유사하지만 몇 가지 특별한 차이점이 있습니다:

a) 매개변수는 타입을 지정하지 않고 설명됩니다.
b) Alias의 인수는 모두 수정될 수 있습니다.
c) Alias는 복잡한 구조 호출을 대체하는 데 사용할 수 있습니다.
d) Alias는 간단한 함수로 사용할 수 있습니다. 이 경우, `return` 키워드를 사용하여 값을 반환해야 합니다.

## 패턴

Alias 함수는 다음과 같은 방식으로 구현됩니다:

```
alias 이름(매개변수1, 매개변수2...매개변수n) = 코드;
```

예시:

```
alias removing(a) = a.replace("-", "").replace(r"%p", " ").lower();
alias display(a, b) = {
    println(a, b);
}

a = removing(a);
display("테스트", a);
```

## 복잡한 패턴

아래 예시는 프레임의 내용에 접근하는 Alias 함수를 생성하는 방법을 보여줍니다:

```
alias get(a, i) = a.persons.pnames[i].
```

이 Alias는 요소를 검색하거나 수정하는 데 모두 사용할 수 있습니다.

요소를 수정해야 할 때는 다음과 같은 대괄호를 사용해야 합니다: `[*...]` 다른 인수를 제공하기 위해.

예시:

```tamgu
// 두 개의 프레임 설명
frame ListOfPerson {
    svector pnames;
    function _initial() {
        pnames = ["a".."e"];
    }
    function string() {
        return pnames;
    }
}
```