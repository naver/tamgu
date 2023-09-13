## 별칭

별칭은 함수와 유사하지만 몇 가지 특징적인 차이가 있습니다:

a) 매개변수는 어떤 타입도 지정하지 않습니다.
b) 별칭의 인수는 모두 수정할 수 있습니다.
c) 복잡한 구조 호출을 대체하는 데에 사용할 수 있습니다.
d) 간단한 함수로 사용할 수 있는 별칭은 `return`으로 값을 반환해야 합니다.

### 패턴

별칭 함수는 다음과 같이 구현됩니다:

```
alias name(a, b, ..., z) = code;
```

### 예제

```python
alias removing(a) = a.replace("-", "").replace(r"%p", " ").lower();
alias display(a, b) = {
    println(a, b);
}

a = removing(a);
display("Test", a);
```

### 복잡한 패턴

아래 예제는 프레임 내용에 접근하는 별칭 함수를 생성하는 방법을 보여줍니다:

```python
alias get(a, i) = a.persons.pnames[i];
```

이 별칭은 요소를 검색하거나 수정하는 데에 모두 사용될 수 있습니다. 요소를 수정해야 할 때는 다음과 같은 대괄호를 사용해야 합니다: `[*...]` 다른 인수를 제공하기 위해.

#### 예제

```python
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

frame Items {
    ListOfPerson persons;
    function string() {
        return persons;
    }
}

// ListOfPerson의 pnames에 접근하기 위한 별칭 정의
alias get(a, i) = a.persons.pnames[i];

// Items 타입의 요소 선언
Items itms;

// pnames의 1번째 위치에 있는 값을 접근
println(get(itms, 1));

// pnames의 1번째 위치에 있는 값을 수정
// [*itms, 1] 사용에 주목하세요
get[*itms, 1] = "B";
```