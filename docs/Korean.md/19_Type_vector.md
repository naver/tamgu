# 타입 벡터

`타입 벡터(Type vector)`는 어떤 타입의 객체든 저장하는 데 사용됩니다. 다음과 같은 메서드를 제공합니다:

## 메서드

1. `clear()`: 컨테이너를 비웁니다.
2. `convert()`: 문자열 벡터에서 숫자 값을 감지하여 실제 숫자로 변환합니다. 벡터 객체를 반환합니다.
3. `editdistance(v)`: 벡터 'v'와의 편집 거리를 계산합니다.
4. `flatten()`: 벡터 구조를 평탄화합니다.
5. `insert(i,v)`: 'v'를 'i' 위치에 삽입합니다.
6. `join(string sep)`: 컨테이너에 대한 문자열 표현을 생성합니다.
7. `json()`: 컨테이너와 일치하는 JSON 호환 문자열을 반환합니다.
8. `last()`: 마지막 요소를 반환합니다.
9. `merge(v)`: 'v'를 벡터에 병합합니다.
10. `pop(i)`: 벡터에서 요소를 삭제합니다.
11. `product()`: 요소들의 곱을 반환합니다.
12. `∏(v,i,j)`: 'i'에서 'j'까지의 요소를 곱합니다 (선택 사항).
13. `push(v)`: 값을 벡터에 추가합니다.
14. `read(string path)`: 파일을 벡터에 읽어옵니다.
15. `remove(e)`: 벡터에서 'e'를 제거합니다.
16. `reserve(int sz)`: 벡터에 'sz'만큼의 잠재적 요소 크기를 예약합니다.
17. `reverse()`: 벡터를 뒤집습니다.
18. `size()`: 컨테이너의 요소 수를 반환합니다.
19. `shape(d1,d2,d3)`: 일차원 벡터에서 텐서의 차원을 반환합니다.
20. `shuffle()`: 벡터의 값을 섞습니다.
21. `sort(bool reverse | function | method)`: 벡터의 값을 정렬합니다.
22. `sortfloat(bool reverse)`: 벡터의 값을 부동소수점으로 정렬합니다.
23. `sortint(bool reverse | function)`: 벡터의 값을 정수로 정렬합니다.
24. `sortstring(bool reverse)`: 벡터의 값을 문자열로 정렬합니다.
25. `sortustring(bool reverse | function)`: 벡터의 값을 부호 없는 문자열로 정렬합니다.
26. `sum()`: 요소들의 합을 반환합니다.
27. `∑(v,i,j)`: 'i'에서 'j'까지의 요소를 합산합니다 (선택 사항).
28. `unique()`: 중복 요소를 제거합니다.
29. `write(string file)`: 벡터 내용을 파일에 작성합니다.

## 초기화

벡터는 `[]` 사이에 구조를 사용하여 초기화할 수 있습니다.

예시:
```cpp
vector v = [1,2,3,4,5];
vector vs = ["a","b","v"];
vector vr = range(10,20,2); // vr은 [10,12,14,16,18]로 초기화됩니다.
vs = range('a','z',2); // vs는 ['a','c','e','g','i','k','m','o','q','s','u','w','y']로 초기화됩니다.
```

## 수학 함수

또한 벡터의 내용에 수학 함수를 적용할 수도 있습니다. 수치형 타입(int, float, long)에서 이러한 함수의 목록을 확인하십시오.

예시:
```cpp
fvector fv = [0,0.1..1];
// fv는: [0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1]
fv.cos()는: [1,0.995004,0.980067,0.955336,0.921061,0.877583,0.825336,0.764842,0.696707,0.62161,0.540302]입니다.
```

## 연산자

- `x in vect`: 수신 변수에 따라 true 또는 인덱스 목록을 반환합니다. 벡터에 문자열이 포함되어 있는 경우, 값이 테스트하는 값과 동일한 경우에만 true 또는 해당 인덱스를 반환합니다. 이 연산은 로컬 문자열 내에서 수행되지 않습니다.
- `for (s in vect) { ... }`: 모든 값을 반복합니다. 각 반복에서 's'에는 'vect'에서의 값이 포함됩니다.
- `+`, `*`, `-`, `/` 등: 벡터의 각 요소에 값을 더하거나 곱하거나 빼거나 나눕니다. 또는 벡터의 각 요소를 다른 벡터에 추가합니다.
- `&`, `|`: 두 벡터의 교집합 또는 합집합을 반환합니다.
- `&&&`: 벡터를 값과 병합합니다.
- `|||`: 컨테이너를 다른 컨테이너나 문자열과 결합합니다.
- `v = [1,2,3]; vv = [4,5,6]; println(v ||| vv); //[[1,4],[2,5],[3,6]]`
- `::`: 값을 벡터에 삽입합니다.
- `10::[1,2,3]` → `[10,1,2,3]`
- `[1,2,3]::10` → `[1,2,3,10]`

## 벡터에서 변수 추출하기

Tamgu는 `[a1,..,an|tail]` 형식의 벡터 패턴을 사용하여 벡터에서 읽는 방법을 제공합니다. 여기서 `a1,..,an`과 `tail`은 변수 또는 값입니다. `tail`은 각 변수가 할당된 후 남은 벡터입니다. 이러한 벡터 패턴은 두 가지 방법으로 사용할 수 있습니다:

- 할당에서:
  - `[a,b|v] = [1,2,3,4,5]`, 그러면 `a=1`, `b=2`, `v=[3,4,5]`가 됩니다.
- `for..in` 루프에서:
  - `for ([a,b|v] in [[1,2,3,4],[3,4,5]]) { ... }`
  - 첫 번째 반복에서 `a=1`, `b=2`, `v=[3,4]`가 됩니다.
  - 두 번째 반복에서 `a=3`, `b=4`, `v=[5]`가 됩니다.

## 예시

```cpp
vector vect;
vect = [1,2,3,4,5];
print(vect[0]); // 출력: 1
print(vect[0:3]); // 출력: [1,2,3]
vect.push(6);
print(vect); // 출력: [1,2,3,4,5,6]
vect.pop(1);
print(vect); // 출력: [1,3,4,5,6]
vect = vect.reverse();
print(vect); // 출력: [6,5,4,3,1]
vect.pop();
print(vect); // 출력: [6,5,4,3]
vect += 10;
print(vect); // 출력: [16,15,14,13]
```

예시 (정수를 정렬하는 경우):

```cpp
// 이 함수는 true 또는 false만 반환해야 합니다
// 매개변수의 타입에 따라 동작이 결정되며, 여기서는 각 요소가 문자열이거나 문자열로 변환된다고 가정합니다.
function compare(int i, int j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// 결과는: [5,10,20]입니다.
```

예시 (문자열로 간주되는 정수를 정렬하는 경우):

```cpp
function compare(string i, string j) {
    if (i < j)
        return true;
    return false;
}

vector myvect = [10,5,20];
myvect.sort(compare);
// 결과는: [10,20,5]입니다.
```

예시 (크기 메서드로 문자열을 정렬하는 경우):

```cpp
vector v = ["100","1","22"];
v.sort(size);
// 결과는: ['1','22','100']입니다.
```