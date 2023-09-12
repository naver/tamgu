## Example

We display the nodes in an indented way using the `Display` function.

```javascript
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 5);
    if (i) // when i == 0, it is the root of our tree and we do not want to display its sisters
        Display(x.next(), i);
}
```

---

We prepare our constituent tree by creating different nodes.

```javascript
synode np1 = { "bar": 2 };
synode np2 = { "bar": 2 };
synode vp = { "bar": 2 };
synode s = { "bar": 3 };
synode v = { "word": "eats", "pers": 3, "pres": "+", "verb": "+" };
synode d1 = { "word": "the", "det": "+" };
synode n1 = { "word": "dog", "noun": "+" };
synode d2 = { "word": "a", "det": "+" };
synode n2 = { "word": "bone", "noun": "+" };

s.addchild(np1);
s.addchild(vp);
vp.addchild(v, np2);
np1.addchild(d1, n1);
```

