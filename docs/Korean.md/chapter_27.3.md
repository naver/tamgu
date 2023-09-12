## 콜백 함수

애너테이터 객체에 콜백 함수를 연결할 수 있습니다.

**참고:** 이 콜백 함수는 렉시콘을 제공한 경우에만 사용할 수 있습니다.

애너테이터 콜백 함수는 파싱이 수행된 후, 규칙이 텍스트에 적용되기 전에 호출됩니다.

이 콜백 함수는 다음과 같은 형식을 가지고 있습니다:

```javascript
// "lexicon label"과 관련된 단어들만
function catching(vector v, annotator a) {
    v = pos.tagger(v);
    return v;
}
```

모든 이러한 규칙들은 `catching`을 가진 애너테이터 `r`을 통해 액세스됩니다.

벡터 `v`는 벡터의 리스트로, 각 하위 벡터는 다음과 같은 구조를 가지고 있습니다:

`[word, lemma1, features1, lemma2, features2,.., leftOffset, rightOffset]`

"word"는 텍스트에서 추출된 토큰입니다. `lemma1`, `features1` 등은 이 토큰의 가능한 읽기입니다. `leftOffset`과 `rightOffset`은 해당 단어의 문자 위치입니다.

반환된 벡터는 동일한 구조를 가지고 있어야 합니다.

예시:

```javascript
['about', 'about', '+Adv+notly+ADV', 'about', '+Prep+PREP', '39', '44']
```

이 함수는 예를 들어 사전 분석에 태깅을 적용하는 데 사용될 수 있습니다.