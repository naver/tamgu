# Maybe/Nothing

Haskell의 Maybe 타입은 오류와 일관성을 처리하는 특정한 방법을 제공합니다. Maybe의 실제 정의는 우리의 구현보다 복잡하며, 이는 Taskell 형식 내에서 예외나 오류를 처리하는 방법입니다. 어떤 필드가 Maybe로 표시되면 시스템은 자동으로 Nothing을 반환합니다.

## 예제

```haskell
division :: Float -> Float -> Maybe Float
division x y = if y == 0 then Nothing else Just (x / y)

main :: IO ()
main = do
    putStrLn $ show (division 5 10)
    putStrLn $ show (division 5 0) -- 0으로 나누면 예외가 발생하므로 Nothing이 표시됩니다.
    if division 5 0 == Nothing then putStrLn "Yep" else putStrLn "Nope"
```

위의 예제에서는 두 개의 `Float` 인수를 받아 `Maybe Float`를 반환하는 `division` 함수를 정의합니다. 두 번째 인수가 0이면 Nothing을 반환하고, 그렇지 않으면 첫 번째 인수를 두 번째 인수로 나눈 결과를 Just로 반환합니다.

그런 다음 `putStrLn`을 사용하여 서로 다른 인수로 `division`을 호출한 결과를 표시합니다. 마지막으로, `division 5 0`의 결과가 Nothing인지 확인하기 위해 if 문을 사용하고, "Yep" 또는 "Nope"를 출력합니다.