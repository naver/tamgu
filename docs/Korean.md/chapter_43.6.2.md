# NLP 예제

이 예제는 이전에 제공된 DCG 문법에서 생성된 절과 일치합니다.

## Predicates

```prolog
predicate sentence, noun_phrase, det, noun, verb_phrase, verb;
```

## Terms

```prolog
term P, SN, SV, det, nom, verbe;
```

## Sentence

```prolog
sentence. _trace(false);
sentence(?S1, ?S3, P(?A, ?B)) :- noun_phrase(?S1, ?S2, ?A), verb_phrase(?S2, ?S3, ?B).
```

## Noun Phrase

```prolog
noun_phrase(?S1, ?S3, SN(?A, ?B)) :- det(?S1, ?S2, ?A), noun(?S2, ?S3, ?B).
```

## Verb Phrase

```prolog
verb_phrase(?S1, ?S3, SV(?A, ?B)) :- verb(?S1, ?S2, ?A), noun_phrase(?S2, ?S3, ?B).
```

## Determiner

```prolog
det(["the" | ?X], ?X, det("the")) :- true.
det(["a" | ?X], ?X, det("a")) :- true.
```

## Noun

```prolog
noun(["cat" | ?X], ?X, nom("cat")) :- true.
noun(["dog" | ?X], ?X, nom("dog")) :- true.
noun(["bat" | ?X], ?X, nom("bat")) :- true.
```

## Verb

```prolog
verb(["eats" | ?X], ?X, verbe("eats")) :- true.
```

## 예제

```prolog
(dét(a), nom(cat)), SV(verbe(eats), SN(dét(the), nom(bat)))),
sentence(['a', 'cat', 'eats', 'the', 'bat'], [], P(a, SV(eats, SN(the, bat)))))
```

이것은 Prolog를 사용한 자연어 처리(NLP)의 예제입니다.