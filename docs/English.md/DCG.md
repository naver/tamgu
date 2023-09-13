## DCG

Tamgu also accepts DCG rules (Definite Clause Grammar) with a few modifications from the original definition. Firstly, Prolog variables should be denoted with `?V` as in the other rules. Secondly, atoms can only be declared as strings.

### Example

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

// We generate all possible interpretations...
vector vr = sentence(?Y, [], ?X);
println(vr);
```