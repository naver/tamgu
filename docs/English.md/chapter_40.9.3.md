## Lambda Functions and List Operations

### ZipWith
The `zipWith` function applies a lambda function to corresponding elements from three lists. The lambda function takes three arguments `x`, `y`, and `z` and performs the operation `x*y+z`. The result is a list.

Example:
```
zipWith (\x y z -> x*y+z) [0..10] [0..10] [0..10]
Result: [0,2,6,12,20,30,42,56,72,90,110]
```

### Composing with takeWhile and Infinite Lists
The `takeWhile` function is used to take elements from a list until a certain condition is met. In this case, we are using it in combination with the `zipWith` function and infinite lists. The lambda function is the same as before. The result is a list.

Example:
```
takeWhile (<100) . zipWith (\x y z -> x*y+z) [0..] [0..] [0..]
Result: [0,2,6,12,20,30,42,56,72,90]
```

### Foldl and Foldr
The `foldl` and `foldr` functions apply a lambda function or operation to a list, using `first` as the initial value. The lambda function should have two arguments, and the difference between `foldl` and `foldr` is the direction of the fold. `foldl` starts from the beginning of the list and traverses it in a forward manner, while `foldr` starts from the end of the list and traverses it backward. The result of these functions is a single value.

If a lambda expression is used, the element from the list should be the first one for `foldr` and the second one for `foldl`. The other element is an accumulator, whose final value will be returned as the result of the fold expression.

Examples:
1. Summing elements from a list, starting with a value of 100:
```
foldl (+) 100 [1..10]
Result: 155 (100+1+2+3+...+10)
```
2. Accumulating values in a lambda expression with `foldl`:
```
foldl (\acc x -> acc+2*x) 10 [1..10]
Result: 120
```
Note that the element from the list, `x`, is the second element of the lambda expression, while the accumulator is the first one.

3. Accumulating values in a lambda expression with `foldr`:
```
foldr (\x acc -> acc+2*x) 10 [1..10]
Result: 120
```
Note that the element from the list, `x`, is the first element of the lambda expression.

### Foldl1 and Foldr1
The `foldl1` and `foldr1` functions are similar to `foldl` and `foldr`, but they take the first element of the list as the initial value.

Examples:
1. Summing elements from a list, starting with a value of 1:
```
foldl1 (+) [1..10]
Result: 55 (1+2+3+...+10)
```
2. Accumulating values in a lambda expression with `foldl1`:
```
foldl1 (\acc x -> acc+2*x) [1..10]
Result: 109
```
Note that the element from the list, `x`, is the second element of the lambda expression.

3. Accumulating values in a lambda expression with `foldr1`:
```
foldr1 (\x acc -> acc+2*x) [1..10]
Result: 110
```
Note that the element from the list, `x`, is the first element of the lambda expression.

### Scanl, Scanr, Scanl1, Scanr1
These functions are similar to the `fold` functions, but they store the intermediate results in a list.

Examples:
1. Summing elements from a list, starting with a value of 1:
```
scanl1 (+) [1..10]
Result: [1,3,6,10,15,21,28,36,45,55]
```
2. Accumulating values in a lambda expression with `scanl1`:
```
scanl1 (\acc x -> acc+2*x) [1..10]
Result: [1,5,11,19,29,41,55,71,89,109]
```
Note that the element from the list, `x`, is the second element of the lambda expression.

3. Accumulating values in a lambda expression with `scanr1`:
```
scanr1 (\x acc -> acc+2*x) [1..10]
Result: [100,98,94,88,80,70,58,44,28]
```
Note that the element from the list, `x`, is the first element of the lambda expression.