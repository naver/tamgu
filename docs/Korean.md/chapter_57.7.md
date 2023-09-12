## 옵션

옵션은 라이브러리에 맞게 맵 형태로 제공되어야 합니다. 이 옵션들은 라이브러리가 예상하는 것과 정확히 일치해야 합니다.

```python
options = {
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
}
```