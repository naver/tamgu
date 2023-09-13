## Operator "in" in Tamgu

The "in" operator in Tamgu has a rich set of behaviors, which is why we have a dedicated section to explain its usage. In the previous description, we have already discussed some possible ways to use this operator with files, vectors, maps, or strings. Now, we will explore how it can also be used with frames.

### 40.1 "in" within a Frame

A frame can have an "in" function, which will be used when the "in" operator is applied to a frame. If the "in" function is not defined for a frame object, then the operator will always return false.

#### Example

Here is an example of using the "in" operator with a map:

```tamgu
map dico;
vector lst;

dico = { 'a': 1, 'b': 6, 'c': 4, 'd': 6 };

// Boolean test, it returns true or false
if (6 in dico)
    print("As expected");

// The receiver is a list, so we return the list of indexes
lst = 6 in dico;
string s;
for (s in lst)
    print("LST:", s);
```

Output:

```
As expected
LST: b
LST: d
```

In this example, we first test if the value 6 exists in the map "dico" using the "in" operator. If it does, we print "As expected". Then, we assign the result of the "in" operator to the vector "lst", which contains the indexes of the occurrences of the value 6 in the map. Finally, we iterate over the elements of "lst" and print them.