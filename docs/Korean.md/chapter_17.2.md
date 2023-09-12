## 방법

1. `clear()`: 컨테이너를 비웁니다.
2. `convert()`: 문자열 벡터 `s`에서 숫자 값을 감지하여 실제 숫자로 변환합니다 (`svector` 및 `uvector`에만 해당). 벡터 객체를 반환합니다.
3. `editdistance(v)`: 벡터 `v`와의 편집 거리를 계산합니다.
4. `flatten()`: 벡터 구조를 평평하게 만듭니다.
5. `insert(i,v)`: `i` 위치에 `v`를 삽입합니다.
6. `join(string sep)`: 컨테이너에 대한 문자열 표현을 생성합니다.
7. `json()`: 컨테이너와 일치하는 JSON 호환 문자열을 반환합니다.
8. `last()`: 마지막 요소를 반환합니다.
9. `merge(v)`: 벡터 `v`를 현재 벡터와 병합합니다.
10. `pop(i)`: 벡터에서 요소를 삭제합니다.
11. `product()`: 요소들의 곱을 반환합니다.
12. `∏(v,i,j)`: `i`부터 `j`까지의 요소를 곱합니다 (선택적 매개변수).
13. `push(v)`: 값을 벡터에 추가합니다.
14. `read(string path)`: 파일을 벡터에 읽어옵니다 (`vector`, `svector`, `uvector`에만 해당).
15. `remove(e)`: 벡터에서 `e`를 제거합니다.
16. `reserve(int sz)`: 벡터에서 `sz` 개의 잠재적 요소 크기를 예약합니다.
17. `reverse()`: 벡터를 반전합니다.
18. `size()`: 요소의 수, 즉 컨테이너의 크기를 반환합니다.
19. `shape(d1,d2,d3)`: 일차원 벡터의 텐서의 차원을 결정합니다.
20. `shuffle()`: 벡터의 값을 섞습니다.
21. `sort(bool reverse | function | method)`: 벡터의 값을 정렬합니다.
22. `sortfloat(bool reverse)`: 벡터의 값을 부동 소수점으로 정렬합니다.
23. `sortint(bool reverse | function)`: 벡터의 값을 정수로 정렬합니다.
24. `sortstring(bool reverse)`: 벡터의 값을 문자열로 정렬합니다.
25. `sortustring(bool reverse | function)`: 벡터의 값을 유니코드 문자열로 정렬합니다.
26. `sum()`: 요소의 합을 반환합니다.
27. `∑(v,i,j)`: `i`부터 `j`까지의 요소를 합산합니다 (선택적 매개변수).
28. `unique()`: 중복 요소를 제거합니다.
29. `write(string file)`: 벡터 내용을 파일에 작성합니다.