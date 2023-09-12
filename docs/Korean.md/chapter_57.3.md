## 옵션

옵션은 라이브러리에게 제공되는 맵으로 제공됩니다. 이러한 옵션은 라이브러리에서 기대하는 것과 정확히 동일합니다.

```python
map options = {
    "train": "input.txt",
    "output": "output.txt",
    "cbow": 1,
    "size": 200,
    "window": 5,
    "negative": 25,
    "hs": 0,
    "sample": 1e-4,
    "threads": 20,
    "binary": 1,
    "iter": 15
};
```

## 라이브러리 word2vec: Tamgu에서 word2vec 타입

Tamgu는 word2vec의 캡슐화를 제공합니다. 자세한 정보는 [여기](https://code.google.com/p/word2vec/)를 참조하십시오.

이 라이브러리를 사용하면 말뭉치에서 시스템을 훈련시키고 거리 또는 유추를 통해 결과를 사용할 수 있습니다.

라이브러리를 사용하려면 단순히 `use("word2vec")`을 호출하십시오.