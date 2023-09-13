## 예시

`Display` 함수를 사용하여 들여쓰기된 방식으로 노드를 표시합니다.

```javascript
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 5);
    if (i) // i가 0일 때, 우리 트리의 루트이므로 형제들을 표시하지 않습니다.
        Display(x.next(), i);
}
```

---

다양한 노드를 생성하여 우리의 구성 트리를 준비합니다.

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