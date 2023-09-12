# 범용 고유 식별자 (UUID)

이 함수는 UUID RFC 4122 버전 4 (완전히 무작위)에 따라 UUID를 생성합니다.

이 함수는 다음 형식의 16진수 값을 포함하는 문자열 값을 반환합니다:

`XXXXXXXX-4XXX-1XXX-XXXX-XXXX`

## 예제

```python
s = uuid()
# 80c67c4d-4c4d-14cd-e58d-eb3364cd
```

참고: 이 예제는 `uuid()` 함수가 Python 프로그램에서 호출되는 것으로 가정합니다.