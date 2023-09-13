# Taskell: 로컬 변수 선언

Taskell에서는 식 내에서 로컬 변수를 선언하는 다양한 방법이 있습니다.

## `let` 연산자 사용

`let` 연산자는 변수나 벡터 패턴을 식과 연결하는 데 사용됩니다. 다음은 몇 가지 예시입니다:

- `<a | let a=10>`
- `<a+b+c | let [a,b,c] = [1,2,3]>`
- `<a+b | let a=10, let b=20>`

반환 값이 `<x | ...>`로 선언된 경우, 여러 `let` 식은 `,`로 구분할 수 있습니다.

## 타입 사용

위의 예시에서 `let` 선언은 식의 내용에 따라 동적으로 타입을 지정할 수 있습니다. 그러나 필요한 경우 특정 타입을 사용할 수도 있습니다. 예를 들어: `<a+b | int a=10, int b=20>`

## 평가

식에서 반복이 선언된 경우, `let`은 각 반복에서 재평가됩니다. 예를 들어: `<a | x <- [1..10], let a=x*2>`

## `in` 식 사용

`in` 식을 통해 값을 반환할 수도 있습니다. 이 경우, 다른 선언들은 하나의 공통된 `let`을 공유합니다. 이 경우에는 특정 타입을 사용할 수 없습니다. 예를 들어: `<let a=10, b=20 in a+b>`

## 정적 변수

`let` 변수가 반복과 함께 선언된 경우, 이 변수는 각 반복에서 다시 초기화됩니다. 이 경우 `static` 키워드를 사용하여 이 변수를 한 번만 초기화할 수 있습니다. 다음은 예시입니다:

```taskell
// 문자열을 출력하고 x와 연결합니다
function display(self u, int x) {
    u = u + x;
    println(u);
    return true;
}

// 여기서 tst는 정적입니다
<statique() = display(tst, i) | i <- [1..3], static string tst="val">

// 여기서 tst는 각 반복에서 다시 초기화됩니다
<dynamique() = display(tst, i) | i <- [1..3], string tst="val">

statique();
println("------");
dynamique();
```

출력:
```
val1
val12
val123
------
val1
val2
val3
```

## `where` 연산자 사용

`where` 연산자는 전역 변수를 선언하는 데 사용됩니다. Taskell 식의 끝에 배치됩니다. 이는 다른 분석 이전에 항상 한 번만 평가됩니다. 다음은 예시입니다: `<a | let a=w+10, where w=20>`

`where` 내에는 `;` 또는 `,`로 구분된 여러 선언이 있을 수 있습니다. 예를 들어: `<a | let a=w1*w2, where w1=20, w2=30>`

## 참고

`where` 내에서 함수를 선언할 수도 있으며, 이는 해당 Taskell 식에 로컬 함수로서 사용됩니다. 다음은 예시입니다:

```taskell
<description(l) = ("Liste=" + <what l>) |
    where
        <what([]): "empty">;
        <what([a]): "one">;
        <what(xs): "large">>
```

- `description([])`는 `empty`를 반환합니다.
- `description([1])`는 `one`을 반환합니다.
- `description([1,2,3])`는 `large`를 반환합니다.

## 가드

Taskell 언어는 스위치/케이스와 유사한 메커니즘인 `guard`를 제공합니다. 가드는 각 테스트와 연관된 동작의 연속입니다. 각 테스트는 `|`로 시작합니다. 기본 값은 `otherwise` 키워드로 시작합니다. 다음은 예시입니다:

```taskell
<imb(bmi) = | bmi <= 10 = "small" | bmi <= 20 = "medium" | otherwise = "large">
```

`imb(12)`는 "medium"을 반환합니다.

## Tamgu 코드 삽입: `{...}`

Taskell 식 중간에 일반적인 Tamgu 코드를 삽입할 수도 있습니다. 이 경우, 이러한 명령문을 `{...}` 사이에 선언하면 됩니다. 예를 들어: `<x | x <- [1..10], {println(x);}>`. 이 경우, 값 도메인에서 반복하는 동안 각 `x`의 값이 표시됩니다.