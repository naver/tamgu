# Printj Functions

The `printj`, `printjln`, `printjerr`, and `printjlnerr` functions are variations of the previous version. The "j" in their names stands for "join". These functions are used to display container values that have been joined beforehand. They can accept two, three, or four arguments. 

The first parameter should be a container, and the second parameter should be a separator string. If the container is a map, a key separator can also be supplied as the third parameter. For vectors and strings, a fourth numerical parameter (or a third for vectors and strings) can be provided to add a carriage return every n values.

If only the container is supplied, the default separator is the carriage return.

Here are the different variations of the `printj` functions:

- `printj(string)` - The separator is a carriage return.
- `printj(string, "sep")` - The separator is specified as a string.
- `printj(string, "sep", counter)` - The separator and a counter are specified.
- `printj(vector)` - The separator is a carriage return.
- `printj(vector, "sep")` - The separator is specified as a string.
- `printj(vector, "sep", counter)` - The separator and a counter are specified.
- `printj(map)` - The separator is a carriage return, and the value separator is ":".
- `printj(map, "sep")` - The separator is specified as a string, and the value separator is ":".
- `printj(map, "sep", "value-sep")` - The separators for keys and values are specified as strings.
- `printj(map, "sep", "value-sep", counter)` - The separators for keys and values are specified as strings, and a counter is provided.

Example usage:

```python
ivector v=[1..10];
printj(v,"-");
# Result is: 1-2-3-4-5-6-7-8-9-10

map m={1:2,2:3,4:5,6:7};
printjln(m,",","-");
# Result is: 1-2,2-3,4-5,6-7
```

These functions provide a convenient way to display container values with custom separators.