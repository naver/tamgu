## 유형

Tamgu는 추론 객체에 대해 세 가지 특정 유형을 노출시킵니다:

### Predicate
이 유형은 추론 절에서 사용될 예측기를 선언하는 데 사용됩니다. 이 유형은 다음과 같은 메서드를 노출시킵니다:
1. `name()`: 예측기 레이블을 반환합니다.
2. `size()`: 인수의 수를 반환합니다.
3. `_trace(bool)`: 호출하는 예측기일 때 이 예측기의 추적을 활성화하거나 비활성화합니다.

### Term
이 유형은 추론 절에서 사용될 용어를 선언하는 데 사용됩니다 (아래의 NLP 예제 참조).

### 다른 추론 유형: 리스트와 연관 맵
Tamgu는 Prolog 스타일의 전통적인 리스트도 제공하며, "|" 연산자를 사용하여 목록 분해를 처리할 수 있습니다 (아래의 NLP 예제에서 이 연산자의 사용법을 확인하세요).

**예시:**
```prolog
predicate alist;
//이 절에서 C는 목록의 나머지입니다...
alist([?A,?B|?C],[?A,?B],?C) :- true.
v = alist([1,2,3,4,5], ?X,?Y);
println(v); → [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

Tamgu는 또한 인수 순서가 중요한 Tamgu 맵으로 구현된 연관 맵을 제공합니다.

**예시:**
```prolog
predicate assign, avalue;
avalue(1,1) :- true.
avalue(10,2) :- true.
avalue(100,3) :- true.
avalue("fin",4) :- true.
```

**예시:**
```prolog
vector v = ['female', 'mary'];
predicatevar fem;
fem = v.predicate(); //벡터를 예측기로 변환합니다.
fem.store(); //팩트 베이스에 저장합니다.
v = fem.query(female, ?X); //예측기 쿼리를 동적으로 구축합니다.
v = fem.query(female, 'mary'); //문자열로 예측기 쿼리를 구축합니다.
```