## Initialization

A vector can be initialized using square brackets "[]".

```cpp
vector<int> v = {1, 2, 3, 4, 5};
vector<string> vs = {"a", "b", "v"};
vector<int> vr = range(10, 20, 2); // vr is initialized with [10, 12, 14, 16, 18];
vs = range('a', 'z', 2); // vs is initialized with ['a', 'c', 'e', 'g', 'i', 'k', 'm', 'o', 'q', 's', 'u', 'w', 'y']
```

Note: The `range()` function is used to generate a sequence of numbers or characters within a given range.