## 절 (Clauses)

절은 다음과 같이 정의됩니다:

```
predicate(arg1,arg2,...,argn) :- pred(arg1,...),pred(arg,...), etc.;
```

사실은 지식 베이스입니다. 다음 구문을 사용하여 프로그램에서 선언할 수 있습니다:

```
predicate(val1,val2,...).
```

또는

```
predicate(val1,val2,...) :- true.
```

`loadfacts(pathname)` 함수는 파일에서 대량의 지식 베이스를 로드하는 데 사용될 수 있습니다. 이 함수는 컴파일 시간에 로드를 시작하도록 파일의 시작에 배치되어야 합니다.

### 분리

Tamgu는 절에서 `;` 연산자를 사용하여 분리를 허용합니다. 이 연산자는 술어 사이에 `,` 대신 사용할 수 있습니다.

예시:

```
predicate mere,pere;
mere("jeanne","marie").
mere("jeanne","rolande").
pere("bertrand","marie").
pere("bertrand","rolande").
predicate parent;
parent(?X,?Y) :- mere(?X,?Y);pere(?X,?Y).
parent._trace(true);
vector v = parent(?X,?Y);
println(v);
```

결과:

```
r:0=parent(?X,?Y) --> parent(?X6,?Y7)
e:0=parent(?X8,?Y9) --> mere(?X8,?Y9)
k:1=mere('jeanne','marie').
success:2=parent('jeanne','marie')
k:1=mere('jeanne','rolande').
success:2=parent('jeanne','rolande')
[parent('jeanne','marie'),parent('jeanne','rolande')]
```

### 컷, 실패, 중지

Tamgu는 실행 흐름을 제어하는 데 사용할 수 있는 컷 연산자인 `!`를 제공합니다. 또한 절의 실패를 강제하는 데 사용할 수 있는 `fail` 키워드도 제공합니다. `stop` 키워드는 전체 평가를 중지합니다.

### 함수

Tamgu는 Prolog 언어에서 제공하는 일부 일반 함수를 제공합니다. 다음과 같습니다:

- `asserta(pred(...))`: 지식 베이스의 시작 부분에 술어를 추가(삽입)합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.
- `assertz(pred(...))`: 지식 베이스의 끝 부분에 술어를 추가(삽입)합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.
- `retract(pred(...))`: 지식 베이스에서 술어를 제거합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.
- `retractall(pred)`: 지식 베이스에서 술어 `pred`의 모든 인스턴스를 제거합니다. 매개변수 없이 사용하면 전체 지식 베이스를 정리합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.
- `predicatedump(pred)` 또는 `findall(pred)`: 메모리에 저장된 모든 술어를 벡터로 반환합니다. 문자열로 술어 이름이 제공되면 지정된 이름을 가진 모든 술어를 덤프합니다.

예시:

```
predicate parent;
adding(?X,?Y) :- asserta(parent(?X,?Y)).
adding("Pierre","Roland");
println(predicatedump(parent));
```

### 범용 술어 이름

특정 술어 이름 없이 지식 베이스에서 사실을 질의할 때, 술어 이름 자리에 `_`를 사용할 수 있습니다.

예시:

```
father("george","sam").
father("george","andy").
mother("andy","mary").
mother("sam","christine").
parent(?A,?B) :- _(?A,?B).
```

또한 `_0`부터 `_9`까지의 특정 변수를 사용할 수 있습니다. 이 변수는 일치하는 술어 이름을 반환합니다.

예시:

```
parent(?A,?B,?P) :- _1(?A,?B), ?P is _1.
```

### 꼬리 재귀

꼬리 재귀는 재귀를 반복적인 프로세스로 변환하는 메커니즘입니다. 꼬리 재귀를 활성화하려면 규칙의 마지막 요소의 이름을 `#`로 수정하면 됩니다.

예시:

```
vector v = [1..10];
traverse([],0).
traverse([?X|?Y],?A) :-
    println(?X,?Y),
    traverse#(?Y,?A).
vector vv = traverse(v,?A);
println(vv);
```

### 콜백 함수

콜백 함수로 술어를 선언할 수 있습니다. 콜백 함수의 서명은 다음과 같습니다:

```
function OnSuccess(predicatevar p, string s) {
    println(s,p);
    return true;
}
```

콜백 함수를 술어와 연결하려면 다음 구문을 사용하세요:

```
string s = "Parent:";
predicate parent(s) with OnSuccess;
parent("John","Mary") :- true.
parent("John","Peter") :- true.
parent(?X,?Y);
```

콜백 함수는 `parent`에 대한 각 평가가 성공할 때마다 호출됩니다. 함수의 두 번째 인수는 선언에서 `parent`에 지정된 매개변수에 해당합니다. 함수가 true를 반환하면 추론 엔진은 다른 해결책을 시도하고, 그렇지 않으면 중지합니다.

결과:

```
Parent: parent('John','Mary')
Parent: parent('John','Peter')
```