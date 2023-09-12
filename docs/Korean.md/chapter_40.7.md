42.6 Taskell 표현식에서의 함수

Taskell 또는 Tamgu에서 Taskell 표현식 내에서 어떤 함수나 메소드든 호출할 수 있습니다. 예를 들어, 문자열에 간단한 "trim"을 구현해 봅시다:

```taskell
<trim1 (w) = x | let x = w.trim()> //가장 간단한 방법
<trim2 (w) = x | let x = <trim w>> //순수한 Taskell 호출
//Tamgu 함수를 정의합니다
function Trim(string c) {
    return c.trim();
}
<trim3 (w) = x | let x = Trim(w)> //외부 함수를 통해 호출
<trim4 (w) = x | let x = <Trim w>> //동일하지만 Taskell 스타일로 호출
```

주의할 점은 요소 유형과 일치한다면 Taskell 표현식 내에서 어떤 메소드나 함수든 호출할 수 있다는 것입니다.

```taskell
<adding (v) = <sum v>>;  
```

Tamgu 방식으로 함수를 호출하든 Taskell 표현식을 사용하든 실제로는 차이가 없습니다.

예시: 리스트 정렬

```taskell
<fastsort ([])= []>  //리스트가 비어있을 경우 빈 "리스트"를 반환합니다
<fastsort ([fv:v])= (mn &&& fv &&& mx) | //다른 하위 리스트들을 병합합니다...
    let mn = fastsort (<a | a < - v, a<=fv>), //fv(First Value)보다 작은 요소들을 포함하는 리스트에 대해 "sort"를 적용합니다
    let mx = fastsort (<a | a < - v, a >fv>)> //fv보다 큰 요소들을 포함하는 리스트에 대해 "sort"를 적용합니다
```

Haskell 함수

우리는 Haskell의 실제 함수 선언과는 달리, 인자들이 공백으로 구분되는 것에서 벗어났습니다. 이는 "(...)"가 Tamgu와 같은 다중 패러다임 언어에서 이러한 함수를 감지하기에 더 나은 시각적 단서를 제공하기 때문입니다.

일반적인 Haskell 함수는 다음과 같이 선언되어야 합니다: `adding x y = x + y`.

함수 처리

Taskell은 함수형 언어로, 함수를 입력으로 받을 수 있을 뿐만 아니라 함수를 결과로 반환할 수도 있습니다.

함수나 메소드를 인자로 받기

Taskell 함수는 다른 함수를 인자로 받을 수 있습니다. 이 경우, 타입은 "call"입니다. 이 구현에서는 실제 Taskell 구현에서 가능한 세부 설명은 허용하지 않습니다.

```taskell
<Invert :: call -> string -> string>
<Invert (f, x) = <f x>>
println(Invert(reverse, "test")); //tset를 반환합니다
```

예를 들어, 위의 함수는 호출을 인자로 받아 이 함수를 x에 적용합니다.

함수를 결과로 반환하기

Taskell 함수는 결과로 함수를 반환할 수도 있습니다.

```taskell
<fonction :: call>
<renvoie :: _ -> _ -> int>
```