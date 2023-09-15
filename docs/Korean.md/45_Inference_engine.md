# 추론 엔진

탐구는 일반적인 탐구 명령어와 자유롭게 혼합될 수 있는 추론 엔진을 내장하고 있습니다. 이 추론 엔진은 Prolog와 매우 유사하지만 몇 가지 특징이 있습니다:

a) 탐구는 일반 함수와 구분하기 위해 사전에 선언할 필요가 없는 술어를 구분할 수 있습니다. 그러나 코드에서 나중에 구현될 술어를 사용해야하는 경우 미리 선언해야합니다.

b) 추론 변수를 선언할 필요가 없습니다. 그러나 그들의 이름은 전통적인 Prolog 이름과 매우 다릅니다: "?"로 시작해야합니다.

c) 각 추론 절은 "."으로 끝나야하며 ";"로 끝나지 않습니다.

d) 용어는 미리 선언 될 수 있습니다 (용어 변수로). 그러나 선언하지 않으려면 추론 변수와 마찬가지로 그 이름 앞에 "?"를 붙여야합니다.

e) 술어에 확률을 부여 할 수 있으며, 이는 가장 높은 확률을 가진 경로를 처음으로 선택하는 데 사용됩니다.

**참고**: Prolog 언어에 대한 적절한 설명은 관련 문서를 참조하십시오.

## 유형

탐구는 추론 객체에 대해 세 가지 특정 유형을 노출시킵니다:

### 술어

이 유형은 추론 절에서 사용될 술어를 선언하는 데 사용됩니다. 이 유형은 다음과 같은 메서드를 노출합니다:

1. `name()`: 술어 레이블을 반환합니다.
2. `size()`: 인수의 수를 반환합니다.
3. `_trace(bool)`: 호출 술어 인 경우이 술어에 대한 추적을 활성화 또는 비활성화합니다.

### 용어

이 유형은 추론 절에서 사용될 용어를 선언하는 데 사용됩니다 (아래의 NLP 예제 참조).

### 기타 추론 유형: 목록 및 연관 맵

탐구는 또한 Prolog와 유사한 전통적인 목록을 제공합니다. 이 목록은 목록 분해를 처리하기 위해 "|" 연산자와 함께 사용할 수 있습니다 (아래의 NLP 예제에서이 연산자의 데모를 참조).

**예제:**

```tamgu
술어 alist;
// 이 절에서 C는 목록의 나머지입니다...
alist([?A,?B|?C],[?A,?B],?C) :- true.
v=alist([1,2,3,4,5], ?X,?Y);
println(v); // [alist([1,2,3,4,5],[1,2],[3,4,5])]
```

탐구는 또한 연관 맵을 제공합니다. 이는 탐구 맵으로 구현되지만 인수 순서가 중요합니다.

**예제:**

```tamgu
술어 assign,avalue;
avalue(1,1) :- true.
avalue(10,2) :- true.
avalue(100,3) :- true.
avalue("fin",4) :- true.
assign({?X:?Y,?Z:?V}) :- avalue(?X,1), avalue(?Y,2), avalue(?Z,3), avalue(?V,4).
vector v=assign(?X);
println(v); // [assign({'100':'fin','1':10})]
```

이 예제에서 키와 값 모두 추론 변수에 따라 다를 수 있습니다. 그러나 연관 키:값이 선언된 순서가 중요합니다. 따라서 {?X:?Y,?Z:?V}는 {?Z:?V,?X:?Y}와 다릅니다.

### 술어변수

이 유형은 술어를 처리하여 이름과 값을 탐색하는 데 사용됩니다. 술어변수는 위치에 따라 인수 목록을 벡터로 액세스 할 수있는 함수로 볼 수 있습니다. 이 유형은 다음과 같은 메서드를 노출합니다:

1. `map()`: 술어를 맵으로 반환합니다: [name:name,'0':arg0,'1':arg1,...]
2. `name()`: 술어 이름을 반환합니다.
3. `query(predicate|name,v1,v2,v3)`: 동적으로 술어를 작성하고 평가합니다.
4. `remove()`: 술어를 메모리에서 제거합니다.
5. `remove(db)`: 술어를 데이터베이스 db에서 제거합니다.
6. `size()`: 인수의 수를 반환합니다.
7. `store()`: 술어를 메모리에 저장합니다.
8. `store(db)`: 술어 값을 데이터베이스 db에 저장합니다.
9. `vector()`: 술어를 벡터로 반환합니다: [name,arg0,arg1,...]

맵과 벡터 모두에 존재하는 "술어"라는 메서드는 벡터 또는 맵의 내용을 vector() 및 map()의 술어 출력과 유사한 형식으로 변환합니다.

**예제:**

```tamgu
vector v=['female','mary'];
술어변수 fem;
fem=v.predicate(); // 벡터를 술어로 변환
fem.store(); // 팩트 베이스에 저장
v=fem.query(female,?X); // 동적으로 술어 쿼리 작성
v=fem.query(female,'mary'); // 문자열로 술어 쿼리 작성
```

## 절

절은 다음과 같이 정의됩니다:

```tamgu
술어(arg1,arg2,...,argn) :- pred(arg...),pred(arg,...), etc. ;
```

지식베이스의 사실입니다. 프로그램에서 다음 명령으로 사실을 선언 할 수 있습니다:

```tamgu
술어(val1,val2,...).
```

또는

```tamgu
술어(val1,val2,...) :- true.
```

실제로 위 문법과 동일합니다.

**loadfacts(pathname)**: 대량의 지식베이스 로드

매우 큰 사실만 포함하는 지식베이스가 저장된 파일이 있는 경우 로드를 가속화하기 위해 `loadfacts`를 사용하는 것이 더 효율적입니다. 이 명령은 파일 경로명과 함께 사용하여 컴파일 시 로드를 시작합니다.

**택일**

탐구는 또한 연결 연산자 ";"을 사용하여 절에서 택일을 허용합니다.

**예제:**

```tamgu
술어 mother,father;
mother("jeanne","marie").
mother("jeanne","rolande").
father("bertrand","marie").
father("bertrand","rolande").
술어 parent;
// 그런 다음 택일로 규칙을 선언합니다...
parent(?X,?Y) :- mother(?X,?Y);father(?X,?Y).
parent._trace(true);
vector v=parent(?X,?Y);
println(v);
```

**결과:**

```
r:0=parent(?X,?Y) --> parent(?X6,?Y7)
e:0=parent(?X8,?Y9) --> mother(?X8,?Y9)
k:1=mother('jeanne','marie').
success:2=parent('jeanne','marie')
k:1=mother('jeanne','rolande').
success:2=parent('jeanne','rolande')
[parent('jeanne','marie'),parent('jeanne','rolande')]
```

**CUT, FAIL 및 STOP**

탐구는 또한 전통적인 "!"로 표시되는 컷을 제공합니다. 또한 절의 실패를 강제하기 위해 사용할 수있는 키워드 인 fail을 제공합니다. Stop은 전체 평가를 중지합니다.

**함수**

탐구는 Prolog 언어의 몇 가지 일반 함수도 제공합니다. 예를 들어:

**함수 asserta(pred(...))**

이 함수는 지식베이스의 시작 부분에 술어를 삽입합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.

**assertz(pred(...))**

이 함수는 지식베이스의 끝에 술어를 삽입합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.

**retract(pred(...))**

이 함수는 지식베이스에서 술어를 제거합니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.

**retractall(pred)**

이 함수는 지식베이스에서 술어 "pred"의 모든 인스턴스를 제거합니다. retractall은 매개 변수없이 사용되는 경우 전체 지식베이스를 지웁니다. 이 함수는 절 선언 내에서만 사용할 수 있습니다.

**함수: predicatedump(pred) 또는 findall(pred)**

이 함수는 매개 변수없이 사용되는 경우 메모리에 저장된 모든 술어를 벡터로 반환합니다. 문자열로 된 술어 이름을 제공하면 지정된 이름을 가진 모든 술어를 벡터로 덤프합니다.

**예제:**

```tamgu
// 술어를 사용하려면 "parent"를 선언해야합니다.
술어 parent;
adding(?X,?Y) :- asserta(parent(?X,?Y)).
adding("Pierre","Roland");
println(predicatedump(parent));
```

**범용 술어 이름**

지식베이스에서 사실을 찾을 때 특정 술어 이름없이 쿼리 할 수 있습니다. 이 경우 술어 이름 대신 "_"을 사용할 수 있습니다.

**예제:**

```tamgu
// 지식베이스
father("george","sam").
father("george","andy").
mother("andy","mary").
mother("sam","christine").
// 규칙, 위의 모든 사실과 일치합니다.
parent(?A,?B) :- _(?A,?B).
```

또한 특정 변수를 사용할 수 있습니다: _0.._9는 일치한 술어 이름을 반환 할 수 있습니다.

**예제:**

```tamgu
// 규칙, 위의 모든 사실과 일치합니다.
parent(?A,?B,?P) :- _1(?A,?B), ?P is _1.
```

이 코드는 다음과 같이 반환됩니다:

```
parent("andy","mary","mother")
parent("sam","christine","mother")
parent("george","sam","father")
parent("george","andy","father")
```

**꼬리 재귀**

꼬리 재귀는 재귀를 반복 프로세스로 변환하는 매커니즘입니다. 꼬리 재귀 메커니즘을 활성화하려면 규칙의 마지막 요소의 이름을 "#"로 수정해야합니다. 꼬리 재귀는이 마지막 요소가 현재 규칙과 일치하는 경우에만 활성화됩니다.

꼬리 재귀는 벡터의 순회를 반복 프로세스로 변환합니다. 그러나 프로세스의 끝에서 벡터의 초기 값은 손실됩니다.

**예제:**

```tamgu
vector v = [1..10];
// 재귀의 끝
traverse([],0).
// 마지막 요소는 꼬리 재귀를 위해 표시됩니다.
traverse([?X|?Y],?A) :- println(?X,?Y), traverse#(?Y,?A).
vector vv = traverse(v,?A);
println(vv);
```

**결과:**

```
1 [2,3,4,5,6,7,8,9,10]
2 [3,4,5,6,7,8,9,10]
3 [4,5,6,7,8,9,10]
4 [5,6,7,8,9,10]
5 [6,7,8,9,10]
6 [7,8,9,10]
7 [8,9,10]
8 [9,10]
9 [10]
10 [] // 첫 번째 인수는 이제 []
[traverse([],0)]
```

**콜백 함수**

술어는 콜백 함수로 선언 될 수 있으며, 그 서명은 다음과 같습니다:

```tamgu
function OnSuccess(predicatevar p, string s) {
    println(s,p);
    return true;
}
```

함수의 두 번째 인수는 부모에서 제공된 매개 변수에 해당합니다. 함수가 true를 반환하면 추론 엔진은 다른 솔루션을 시도하고, 그렇지 않으면 중지합니다.

**결과:**

위의 예제를 실행하면 다음과 같은 결과를 얻습니다.

```
Parent: parent('John','Mary')
Parent: parent('John','Peter')
```

**DCG (Definite Clause Grammar)**

탐구는 일부 수정을 통해 DCG 규칙 (Definite Clause Grammar)을 수용합니다. 첫째, Prolog 변수는 다른 규칙과 마찬가지로 ?V로 표시되어야합니다. 세 번째, 원자는 문자열로만 선언 할 수 있습니다.

**예제:**

```tamgu
술어 sentence,noun_phrase,verb_phrase;
term s,np,vp,d,n,v;
sentence(s(np(d("the"),n("cat")),vp(v("eats"),np(d("a"),n("bat"))))) --> [].
```

**평가 시작**

평가는 함수와 정확히 동일한 방식으로 시작됩니다. 필요한 만큼 많은 추론 변수를 제공 할 수 있지만 한 번에 하나의 술어 만 시작할 수 있으므로 표현식을 여러 술어로 구성하려면 먼저 절로 선언해야합니다.

**중요**: 수신 변수가 벡터 인 경우 모든 가능한 분석이 제공됩니다. 평가 트리가 완전히 탐색됩니다. 수신 변수가 다른 경우 솔루션이 찾아지면 평가가 중지됩니다.

**메서드 매핑을 술어로**

대부분의 객체 메서드는 간단한 방식으로 술어로 매핑됩니다. 예를 들어, 문자열이 trim 메서드를 내보내면 "p_trim"이라는 두 가지 변수로 생성됩니다. 각 메서드는 이와 같은 술어에 매핑됩니다. 각 메서드에는 "p_" 접두사가 추가되어 이 메서드를 술어로 변환합니다. 이 술어의 첫 번째 인수는 메서드의 헤드 개체이고, 마지막 매개 변수는 해당 개체에이 메서드를 적용한 결과입니다. 따라서 s가 문자열 인 경우 s.trim()은 p_trim(s,?X)가되며, 여기서 ?X는 s에 대한 trim의 결과입니다. ?X가 통일되면 술어는 ?X가 s.trim()과 동일한지 확인합니다.

**예제:**

```tamgu
compute(?X,?Y) :- p_log(?X,?Y).
between(?X,?B,?E), succ(?X,?Y).
```

**탐구 변수와 관련된 일반적인 실수**

문자열 s, 정수 s 또는 기타 유형의 변수와 같은 일반적인 변수를 사용하는 경우 이러한 변수가 비교 값을 사용하는 술어에서 사용된다는 것을 기억해야합니다. 예는 약간 무엇을 의미하는지 명확히해 줄 수 있습니다.

**예제 1**

```tamgu
string s="test";
string sx="other";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

실행:

```
r:0=comp(s,3) --> comp(other,?X172) --> Fail
```

이 절은 s와 sx가 다른 값을 가지기 때문에 실패했습니다.

**예제 2**

```tamgu
string s="test"; // 이제 같은 값이 있습니다.
string sx="test";
predicate comp;
comp._trace(true);
comp(s,3) :- println(s).
comp(sx,?X);
```

실행:

```
r:0=comp(s,3) --> comp(test,?X173)
e:0=comp(test,3) --> println(s)test
success:1=comp('test',3)
```

디자인하는 동안 외부 변수를 비교 소스로 사용하고 인스턴스화로 사용하지 않도록 주의하십시오. 술어에 문자열 값을 전달하려면 해당 문자열의 위치 표시자가 술어 변수 여야합니다.

**예제 3**

```tamgu
string sx="test";
predicate comp;
comp._trace(true);
comp(?s,3) :- println(?s).
comp(sx,?X);
```

실행:

```
r:0=comp(?s,3) --> comp(test,?X176)
e:0=comp('test',3) --> println(?s177:test)test
success:1=comp('test',3)
```

**예제**

**하노이 타워**

다음 프로그램은 하노이 타워 문제를 해결합니다.

```tamgu
술어 move;
move(1,?X,?Y,_) :- println('Move the top disk from ',?X,' to ',?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
술어변수 res;
res=move(3,"left","right","centre");
println(res);
```

**조상**

이 프로그램을 사용하여 다른 사람의 부모 관계에서 공통 여성 조상을 찾을 수 있습니다.

```tamgu
술어 ancestor,parent,male,female,test;
ancestor(?X,?X) :- true.
ancestor(?X,?Z) :- parent(?X,?Y),ancestor(?Y,?Z).
parent("george","sam") :- true.
parent("george","andy") :- true.
parent("andy","mary") :- true.
male("george") :- true.
male("sam") :- true.
male("andy") :- true.
female("mary") :- true.
test(?X,?Q) :- ancestor(?X,?Q), female(?Q).
test._trace(true);
vector v=test("george",?Z);
println(v);
```

**NLP 예제**

이 예제는 이전에 주어진 DCG 문법에서 생성 된 절을 나타냅니다.

```tamgu
술어 sentence,noun_phrase,det,noun,verb_phrase,verb;
term P,SN,SV,dét,nom,verbe;
sentence(?S1,?S3,P(?A,?B)) :- noun_phrase(?S1,?S2,?A), verb_phrase(?S2,?S3,?B).
noun_phrase(?S1,?S3,SN(?A,?B)) :- det(?S1,?S2,?A), noun(?S2,?S3,?B).
verb_phrase(?S1,?S3,SV(?A,?B)) :- verb(?S1,?S2,?A), noun_phrase(?S2,?S3,?B).
det(["the"|?X],?X,dét("the")) :- true.
det(["a"|?X],?X,dét("a")) :- true.
noun(["cat"|?X],?X,nom("cat")) :- true.
noun(["dog"|?X],?X,nom("dog")) :- true.
verb(["eats"|?X],?X,verbe("eats")) :- true.
vector v;
v=sentence(?X,[],?A);
println("All the sentences that can be generated:",v);
// 문장을 분석합니다.
v=sentence(["the","dog","eats","a","bat"],[],?A);
println("The analysis:",v);
```

**애니메이션 하노이 타워**

아래 코드는 디스크가 한 열에서 다른 열로 이동하는 애니메이션을 표시합니다. 그래픽과 술어를 모두 병합합니다.

```tamgu
술어 move;
map columns={'left':[70,50,30],'centre':[],'right':[]};
function disk(window w,int x,int y,int sz,int position) {
    int start=x+100-sz;
    int level=y-50*position;
    w.rectanglefill(start,level,sz*2+20,30,FL_BLUE);
}
function displaying(window w,self o) {
    w.drawcolor(FL_BLACK);
    w.font(FL_HELVETICA,40);
    w.drawtext("Left",180,200);
    w.drawtext("Centre",460,200);
    w.drawtext("Right",760,200);
    w.rectanglefill(200,300,20,460,FL_BLACK);
    w.rectanglefill(100,740,220,20,FL_BLACK);
    w.rectanglefill(500,300,20,460,FL_BLACK);
    w.rectanglefill(400,740,220,20,FL_BLACK);
    w.rectanglefill(800,300,20,460,FL_BLACK);
    w.rectanglefill(700,740,220,20,FL_BLACK);
    vector left=columns['left'];
    vector centre=columns['centre'];
    vector right=columns['right'];
    int i;
    for (i=0;i<left;i++)
        disk(w,100,740,left[i],i+1);
    for (i=0;i<centre;i++)
        disk(w,400,740,centre[i],i+1);
    for (i=0;i<right;i++)
        disk(w,700,740,right[i],i+1);
}
window w with displaying;
function moving(string x,string y) {
    columns[y].push(columns[x][-1]);
    columns[x].pop();
    w.redraw();
    pause(0.5);
    return true;
}
move(1,?X,?Y,_) :- moving(?X,?Y).
move(?N,?X,?Y,?Z) :- ?N>1, ?M is ?N-1, move(?M,?X,?Z,?Y), move(1,?X,?Y,_), move(?M,?Z,?Y,?X).
thread hanoi() {
    move(3,"left","right","centre");
}
function launch(button b,self o) {
    hanoi();
}
button b with launch;
w.begin(50,50,1000,800,"HANOI");
b.create(20,20,60,30,FL_Regular,FL_NORMAL_BUTTON,"Launch");
w.end();
w.run();
```