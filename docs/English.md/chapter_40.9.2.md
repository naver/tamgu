## Functional Programming in Haskell

This repository contains examples of functional programming in Haskell. 

### Filtering a List

To filter a list based on a condition, we can use the `filter` function. For example, to filter a list of numbers from 0 to 1000 and keep only the numbers with exactly one factor, we can use the following expression:

```haskell
filter (\x -> length (factors x) == 1) [0..1000]
```

### Using a Comparison Function

We can also define a function to perform the comparison. For instance, to check if a number is odd, we can define the following function:

```haskell
odd :: Int -> Bool
odd x = x `mod` 2 /= 0
```

Then, we can use this function with the `filter` function to filter a list. For example, to filter the numbers from 1 to 10 and keep only the odd numbers, we can use the following expression:

```haskell
filter odd [1..10]
```

### Composing Expressions with `map`

We can compose expressions using the `map` function. For example, to filter the numbers from 1 to 10, multiply each number by 3, and keep only the odd numbers, we can use the following expression:

```haskell
filter odd (map (*3) [1..10])
```

### `all`, `any`, `and`, `or`, and `xor` Functions

The `all` function returns `True` if a condition is verified for each element in a list. For example, to check if all the numbers in the list [1, 3, 5] are odd, we can use the following expression:

```haskell
all odd [1, 3, 5]
```

The `any` function returns `True` if a condition is verified for at least one element in a list. For example, to check if any number in the list [2, 3, 6] is odd, we can use the following expression:

```haskell
any odd [2, 3, 6]
```

The `and` function returns `True` if each element in a list is `True`. For example, to check if all the elements in the list [1, 1, 1] are `True`, we can use the following expression:

```haskell
and [1, 1, 1]
```

The `or` function returns `True` if at least one element in a list is `True`. For example, to check if any element in the list [1, 0, 0] is `True`, we can use the following expression:

```haskell
or [1, 0, 0]
```

The `xor` function returns `True` if at least one element in a list is `True`, but not all elements are `True`. For example, to check if any element in the list [1, 0, 0] is `True`, we can use the following expression:

```haskell
xor [1, 0, 0]
```

### `takeWhile` Function

The `takeWhile` function applies a condition to each element in a list. It stops the iteration on the list when the condition is not met. For example, to iterate on an infinite list [1, 11, 21, 31, ...] and stop when the value is above 100, we can use the following expression:

```haskell
takeWhile (<100) [1, 11..]
```

### Combining `takeWhile`, `map`, and `filter`

We can combine the `takeWhile`, `map`, and `filter` functions to perform more complex operations. For example, to extract all the squares below 500 that are odd, we can use the following expression:

```haskell
filter odd (takeWhile (<500) (map (^2) [1..]))
```

### `dropWhile` Function

The `dropWhile` function drops all the elements from a list until an element does not match the condition. It then keeps the rest of the list. For example, to drop all the characters from the string "12345ABCD123" until a non-digit character is encountered, we can use the following expression:

```haskell
dropWhile isDigit "12345ABCD123"
```

### Combining Lists with `zip` and `zipWith`

The `zip` function combines different lists together. Each element from the input lists is stored into a list. For example, to combine three lists [0..2], [0..2], and [0..2], we can use the following expression:

```haskell
zip [0..2] [0..2] [0..2]
```

The `zipWith` function combines different lists together using a given function. The function should have as many arguments as the number of lists in the expression. For example, to combine three lists [0..10], [0..10], and [0..10] using the addition function, we can use the following expression:

```haskell
zipWith (+) [0..10] [0..10] [0..10]
```