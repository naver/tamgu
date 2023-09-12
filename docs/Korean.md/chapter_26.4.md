## 문자열 처리

트랜스듀서를 사용하여 문자열을 처리하는 다양한 방법이 있습니다.

### Lookup

`lookup` 함수는 단어가 트랜스듀서에 속하는지 여부를 확인하는 데 사용됩니다. 이 경우, 단어의 원형과 기능을 반환합니다. 트랜스듀서는 여러 가지 해결책을 반환할 수 있습니다. 모든 가능한 해결책을 검색하려면 수신 변수를 벡터로 설정해야 합니다.

예시:
```python
t.lookup("class")의 결과: class +Singular+Noun
```

또한 편집 거리 임계값과 작업으로 문자열의 처리를 제한할 수도 있습니다.

예시:
```python
t.lookup("cl iss", 1, a_change)의 결과: class +Singular+Noun
```

### Lookdown

`lookdown` 함수는 원형과 기능을 사용하여 단어의 올바른 표면 형태를 검색하는 데 사용됩니다.

예시:
```python
t.lookdown("class +Plural+Noun")의 결과: classes
```

### Parse

`parse` 함수는 문자열을 토큰으로 분리하고, 각 토큰에 대해 가능한 모든 원형+기능을 벡터로 반환합니다.

예시:
```python
transducer t(_current + "english.tra");
string sentence = "The lady drinks a glass of milk.";
vector v = t.parse(sentence);
printjln(v);
```

출력:
```
['The', 'The +0+3+0+3+Prop+WordParticle+Sg+NOUN', 'the +0+3+0+3+Det+Def+SP+DET']
['lady', 'lady +4+8+4+8+Noun+Sg+NOUN']
['drinks', 'drink +9+15+9+15+Verb+Trans+Pres+3sg+VERB', 'drink +9+15+9+15+Noun+Pl+NOUN']
['a', 'a +16+17+16+17+Det+Indef+Sg+DET']
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Ve rb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

참고: `parse` 함수는 초기 문장에서 각 단어의 위치도 반환합니다.