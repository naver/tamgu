# Maybe/Nothing

The Maybe type in Haskell provides a specific way to handle errors and inconsistencies. The actual definition of Maybe is more complex than our implementation, which is a way to handle exceptions or errors within the Taskell formalism. When a field is denoted as Maybe, the system will automatically return Nothing.

## Example

```haskell
division :: Float -> Float -> Maybe Float
division x y = if y == 0 then Nothing else Just (x / y)

main :: IO ()
main = do
    putStrLn $ show (division 5 10)
    putStrLn $ show (division 5 0) -- Displays Nothing, as dividing by 0 raises an exception
    if division 5 0 == Nothing then putStrLn "Yep" else putStrLn "Nope"
```

In the example above, we define a function `division` that takes two `Float` arguments and returns a `Maybe Float`. If the second argument is 0, it returns Nothing. Otherwise, it returns Just the result of dividing the first argument by the second argument.

We then use `putStrLn` to display the results of calling `division` with different arguments. Finally, we use an if statement to check if the result of `division 5 0` is Nothing, and print "Yep" or "Nope" accordingly.