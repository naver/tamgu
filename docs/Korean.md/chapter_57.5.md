## 옵션

옵션은 라이브러리로 전달되는 맵 형태로 제공됩니다. 이 옵션들은 라이브러리에서 기대하는 옵션들과 정확히 동일합니다.

```python
맵 옵션 = {
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

## Tamgu에서 word2vec 라이브러리 사용

Tamgu는 word2vec의 캡슐화 기능을 제공합니다. 자세한 정보는 [여기](https://code.google.com/p/word2vec/)를 참조하세요.

이 라이브러리를 사용하면 말뭉치에서 시스템을 학습시키고, 거리나 유추를 통해 결과를 사용할 수 있습니다.

라이브러리의 이름은 word2vec이며, 다음과 같이 사용합니다: `use("word2vec");`