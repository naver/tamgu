# Operator in

This operator has a rich behavior, which is why we have a dedicated section to explain it. In the previous description, we already discussed some possible uses of this operator with files, vectors, maps, or strings. Now, let's see how it can also be used with frames.

## 1. "in" within a Frame

A frame can have an `in` function, which is used when the `in` operator is applied to a frame. If a frame object does not have an `in` function, then the operator always returns false.

### Example

```cpp
// This is an example of using the in operator with a map.
map dico;
vector lst;
dico = { 'a': 1, 'b': 6, 'c': 4, 'd': 6 };

// Boolean test, it returns true or false
if (6 in dico)
    print("As expected\n");

// The receiver is a list, so we return the list of indexes
lst = 6 in dico;
string s;
for (s in lst)
    print("LST: ", s, "\n");
```

Output:
```
As expected
LST: b
LST: d
```

As we can see in this example, the system returns information related to the type of the receiver.

### Example with a Frame

```cpp
frame testframe {
    int i;

    // The type of the parameter can be anything
    function in(int j) {
        if (i == j)
            return true;
        return false;
    }
}
```

Please note that the code has been formatted for better readability.