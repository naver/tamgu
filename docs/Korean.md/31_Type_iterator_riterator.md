# Iterator와 Riterator

이 반복자들은 문자열, 벡터, 맵, 규칙과 같은 여러 유형의 객체를 반복하는 데 사용됩니다.

Riterator는 컬렉션의 끝부터 반복을 허용하는 역순 반복자입니다.

## 메소드

1. `begin()`: 반복자를 컬렉션의 시작으로 초기화합니다.
2. `end()`: 컬렉션의 끝에 도달했을 때 true를 반환합니다.
3. `key()`: 현재 요소의 키를 반환합니다.
4. `next()`: 컬렉션의 다음 요소로 이동합니다.
5. `value()`: 현재 요소의 값을 반환합니다.

## 초기화

반복자는 간단한 할당으로 초기화할 수 있습니다.

## 예제

```cpp
vector<int> v = {1, 2, 3, 4, 5};
iterator it = v;

for (it.begin(); it.end(); it.next()) {
    print(it.value(), ",");
}
```

## 출력

```
1, 2, 3, 4, 5,
```