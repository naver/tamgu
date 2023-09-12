## DCG

탐구는 몇 가지 수정사항을 통해 DCG 규칙 (Definite Clause Grammar)도 허용합니다. 첫째로, Prolog 변수는 다른 규칙과 마찬가지로 `?V`로 표시해야 합니다. 둘째로, 원자는 문자열로만 선언될 수 있습니다.

### 예제

```
predicate sentence, noun_phrase, verb_phrase;
term s, np, vp, d, n, v;

sentence(s(?NP, ?VP)) --> noun_phrase(?NP), verb_phrase(?VP).
noun_phrase(np(?D, ?N)) --> det(?D), noun(?N).
verb_phrase(vp(?V, ?NP)) --> verb(?V), noun_phrase(?NP).
det(d("the")) --> ["the", ?X], {?X is "big"}.
det(d("a")) --> ["a"].
noun(n("bat")) --> ["bat"].
noun(n("cat")) --> ["cat"].
verb(v("eats")) --> ["eats"].

// 모든 가능한 해석을 생성합니다...
vector vr = sentence(?Y, [], ?X);
println(vr);
```