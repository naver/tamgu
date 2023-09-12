# 자료 구조

키워드 'data'는 새로운 데이터 타입을 정의하는 데 사용됩니다. Tamgu에서의 자료 구조는 프레임 위에 매핑됩니다.

- `Eq`: 이 자료 구조는 객체를 비교하는 코드를 삽입합니다. 객체의 필드가 동일한 경우 true를 반환하고, 그렇지 않으면 false를 반환하는 동작을 정의합니다.
- `Ord`: 이 자료 구조는 `<`, `>`, `<=`, `>=`와 같은 연산자를 사용하여 객체를 비교하는 코드를 삽입합니다. `Eq`와 유사합니다.

예시:

더 정교한 방식으로 이러한 설명을 확장할 수 있습니다. 예를 들어, 좌표를 `Point` 자료 구조로 대체하기로 결정할 수 있습니다.

먼저, `Point` 자료 구조를 선언합니다:
```haskell
data Point = Point Float Float deriving (Show)
```

그런 다음 좌표를 대체하는 데 사용됩니다:
```haskell
data Shape = Circle Point Float | Rect Point Point deriving (Show)
```

`Point`를 `b` 오프셋으로 수정합니다:
```haskell
dep :: Point -> Float -> Point
dep (Point x y) b = Point (x + b) (y + b)
```

이동 함수는 `Shape` 객체를 수정합니다. `Shape` 객체와 `Float`를 입력으로 받아 `Shape` 객체를 반환합니다. 마지막 표현식 주위의 `<..>` 대신 `$` 연산자를 사용하는 것에 주목하세요:
```haskell
mouve :: Shape -> Float -> Shape
mouve (Circle p z) b = Circle (dep p b) z
mouve (Rect p1 p2) b = Rect (dep p1 b) $ dep p2 b
```

동등한 표현입니다:
```haskell
mouve (Rect p1 p2) b = Rect (dep p1 b) (dep p2 b)
```

`let be = mouve (Circle (Point 17 18) 30) 40`라고 가정해 봅시다:
```haskell
let be = mouve (Circle (Point 17 18) 30) 40
println be -- 결과: Circle (Point 57 58) 30
```

`let br = mouve (Rect (Point 17 18) (Point 30 40)) 40`라고 가정해 봅시다:
```haskell
let br = mouve (Rect (Point 17 18) (Point 30 40)) 40
println br -- 결과: Rect (Point 57 58) (Point 70 80)
```

## 필드 이름이 있는 자료 구조 (Monades)

중괄호 (`{}`) 사이에 필드 이름을 설명하여 필드 이름이 있는 자료 구조를 선언할 수 있습니다 (위에서 언급한 대로 프레임 위에 매핑됩니다).

각 선언은 필드 이름 다음에 해당하는 타입으로 구성됩니다:
```haskell
data Person = Person { name :: String, lastname :: String, age :: Int }
```

시스템은 각 필드를 자동으로 프레임 변수로 구현합니다. 또한 프레임에 있는 필드 수만큼의 함수 (`_name()`, `_lastname()`, `_age()`)를 생성합니다. 함수 이름은 필드 이름에 "_"를 접두어로 붙인 것입니다.

이전 단락에서 언급한 구조와 동일한 구조로 새로운 자료 구조를 생성할 수도 있고, 각 필드를 개별적으로 인스턴스화할 수도 있습니다:
```haskell
let nr = Person "Pierre" "Jean" 46 -- 위와 동일
let nr = Person { lastname = "Jean", name = "Dupont", age = 40 } -- 필드를 통해
```

위의 예시에서 볼 수 있듯이, 필드를 통해 구조를 초기화할 때는 순서가 더 이상 중요하지 않습니다.

## 함수로 자료 구조를 확장하기

자료 구조를 함수로 확장할 수도 있습니다. 예를 들어, 위의 자료 구조가 필드를 반환하도록 하려면 다음과 같이 할 수 있습니다:
```haskell
data Person = Name() = name
```

`Person` 프레임은 이제 `Name` 함수를 노출시켜 필드 `name`을 반환합니다. 이제 `nr.Name()`을 호출할 수 있습니다.

## 자료 구조는 프레임입니다

사실, 자료 구조는 프레임 위에 매핑된 맵입니다. 예를 들어:
```haskell
data Shape = Circle Float Float Float | Rectangle Float Float Float Float deriving (Show)
```

실제로는 다음과 같이 매핑됩니다:
```haskell
frame Shape {
    frame Circle {
        Float d1_Circle;
        Float d2_Circle;
        function _name() {
            return (name);
        }
        function _lastname() {
            return (name);
        }
        function _age() {
            return (name);
        }
    }
}
```

각 필드에 대한 함수가 생성되어 해당 내용에 액세스할 수 있도록 합니다. 또한 인수가 없는 `_initial()` 함수가 존재하는 것에 주목하세요. 이 함수는 필드 이름에 값을 할당하는 데 사용됩니다.