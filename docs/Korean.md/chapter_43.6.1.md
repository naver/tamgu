## 예시

### 하노이 타워

다음 프로그램은 하노이 타워 문제를 해결합니다.

```prolog
% 우리는 우리의 술어를 선언합니다.
move.

% "?로 시작하는 변수 이름에 주목하세요."
move(1, ?X, ?Y, _) :- 
    println('Move the top disk from', ?X, 'to', ?Y).

move(?N, ?X, ?Y, ?Z) :- 
    ?N > 1, 
    ?M is ?N - 1, 
    move(?M, ?X, ?Z, ?Y), 
    move(1, ?X, ?Y, _), 
    move(?M, ?Z, ?Y, ?X).

% 결과는 res에 할당됩니다.
res :- move(3, "left", "right", "centre").
println(res).
```

이 예제를 실행하면 다음과 같은 결과를 얻을 수 있습니다:

```
Move the top disk from left to right
Move the top disk from left to centre
Move the top disk from right to centre
Move the top disk from left to right
Move the top disk from centre to left
Move the top disk from centre to right
Move the top disk from left to right
move(3, 'left', 'right', 'centre')
```

### 조상

이 프로그램을 사용하여 다른 사람들의 부모 관계에서 공통된 여성 조상을 찾을 수 있습니다.

```prolog
% 우리는 모든 술어를 선언합니다.
k:2 = parent('george', 'andy') --> ancestor('andy', ?Z19), female(?Z19)
r:3 = ancestor(?X, ?X) --> ancestor('andy', ?Z19), female(?Z19)
```