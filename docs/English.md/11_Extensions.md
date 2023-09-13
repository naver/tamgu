# Extensions

Extensions allow you to add new methods to certain types, such as map, vector, int, string, and others. The extension name must be one of the following valid types: string, automat on, date, time, file, integer, float, vector, list, map, set.

## Extension Definition

To define an extension, you create a frame where you declare the new methods that you want to add to the system.

If you need to refer to the current element, you can use a variable whose name is the type itself with "_" as a prefix. For example, if you are extending the type "vector", the variable would be "_vector".

It's important to note that if you add new methods to "map", all maps will inherit these new methods. The same applies to vectors.

### Example:

```python
// We extend map to return a value, which will be removed from the map.
extension map {
    // We add this new method, which will be available to all maps...
    function returnAndDelete(int key) {
        // We extract the value with our key
        string s = _map[key];
        // We remove it
        _map.pop(key);
        return s;
    }
}

map mx = {1: 2, 3: 4};
// returnAndDelete is now available to all types of map.
string s = mx.returnAndDelete(1);

imap imx = {1: 2, 3: 4};
int x = imx.returnAndDelete(1);
```

In the example above, we extend the "map" type to add a new method called "returnAndDelete". This method returns a value from the map and removes it. The method is then used on a map object called "mx" and an imap object called "imx".