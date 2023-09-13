## Extensions in Tamgu

In Tamgu, it is possible to extend certain types, such as map, vector, int, string, and others, with new methods.

The concept of "extension" is similar to that of a frame, with the extension name being one of the following valid types: string, automat on, date, time, file, integer, float, vector, list, map, set.

### Extension Definition

To define an extension, you declare the new methods you want your system to use within a frame. If you need to refer to the current element, use a variable with the type itself as the name, prefixed with "_".

For example, if you are extending the "vector" type, the variable name would be "_vector".

It's important to note that if you add new methods to "map", all maps will inherit these new methods. The same applies to vectors.

#### Example

```tamgu
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
```

In the example above, we extend the "map" type with a new method called "returnAndDelete". This method extracts a value from the map using a given key and removes it from the map. The extracted value is then returned.

After defining the extension, we create a map called "mx" and use the newly added method "returnAndDelete" to extract and remove the value associated with the key 1. The extracted value is stored in the string variable "s".

We also create another map called "imx" which is of type "imap".