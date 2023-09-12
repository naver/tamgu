## 애니메이션 하노이 타워

아래 코드는 디스크가 한 열에서 다른 열로 이동하는 애니메이션을 표시합니다. 그래픽과 술어를 모두 병합합니다.

```prolog
% 우리는 술어를 선언합니다.
predicate move.

% 초기 설정... 모든 디스크는 왼쪽 열에 있습니다.
map columns = { 'left': [70, 50, 30], 'centre': [], 'right': [] }.

% 디스크를 그 위치와 열에 따라 그립니다.
function disk(window w, int x, int y, int sz, int position) {
    int start = x + 100 - sz;
    int level = y - 50 * position;
    w.rectanglefill(start, level, sz * 2 + 20, 30, FL_BLUE);
}
```

## 분석

분석:

```prolog
[sentence(['the', 'dog', 'eats', 'a', 'bat'], [], P(SN(dét(the), nom(dog)), SV(verbe(eats), SN(dét(a), nom(bat)))))]
```