# 의존성과 Synode

의존성은 현대 자연 언어 처리에서 필수적인 언어학적 개체로 자리잡았습니다. Tamgu는 술어 엔진을 기반으로 한 이러한 의존성에 대한 특정 구현을 제공합니다. 이 구현의 목표는 의존성 파서(예: Stanford 파서)의 출력을 입력으로 사용하여 추가 분석을 수행하는 것입니다.

의존성은 구문 노드를 연결하는 술어로 평가됩니다. 우리 시스템은 synode라는 두 번째 유형을 제공하는데, 이는 구성 트리의 노드를 나타냅니다.

## Synode

Synode는 특징 집합(맵으로 구현)과 구성 트리에서의 위치로 정의된 구문 노드입니다. Synode에는 다음과 같은 메서드가 있습니다:

1. _initial(map m): 주어진 특징을 가진 구문 노드를 생성합니다.
2. addchild(synode): 자식 노드를 추가합니다.
3. addnext(synode): 다음 노드를 추가합니다.
4. addprevious(synode): 이전 노드를 추가합니다.
5. after(synode): 동일한 부모 아래에서 주어진 노드 이후에 있는 경우 true를 반환합니다.
6. attributes(): 특징 속성을 벡터로 반환합니다.
7. before(synode): 동일한 부모 아래에서 주어진 노드 이전에 있는 경우 true를 반환합니다.
8. child(): 첫 번째 자식 노드를 반환하거나 매개변수와 일치하는지 확인합니다.
9. children(): 주어진 노드의 자식 목록을 반환하거나 노드가 자식인지 확인합니다.
10. definitions(mapss): 모든 synode에 대한 유효한 특징 정의를 설정합니다.
11. last(): 마지막 자식 노드를 반환하거나 매개변수와 일치하는지 확인합니다.
12. nbchildren(): 직접 자식의 수를 반환합니다.
13. next(synode): 다음 노드를 반환하거나 매개변수와 일치하는지 확인합니다.
14. parent(): 부모 노드를 반환하거나 매개변수와 일치하는지 확인합니다.
15. previous(synode): 이전 노드를 반환하거나 매개변수와 일치하는지 확인합니다.
16. precede(synode): 노드가 (트리 어디에서든) 주어진 노드 앞에 있는 경우 true를 반환합니다.
17. sibling(synode): 노드가 형제 노드(또는 자손)인지 확인합니다.
18. sisters(synode): 자매 노드의 목록을 반환하거나 노드가 자매 노드인지 확인합니다.
19. succeed(synode): 노드가 (트리 어디에서든) 주어진 노드 뒤에 있는 경우 true를 반환합니다.
20. test(string attribute): 속성이 특징 구조에 속하는지 확인합니다.
21. values(): 특징 값들을 벡터로 반환합니다.

## 구성 트리 생성

구성 트리는 위에서 아래로 구축됩니다. `addchild` 함수를 사용할 때, 현재 노드 아래에 자식 노드를 추가하고, 이 함수에 대한 각 후속 호출은 이전 자식 노드 뒤에 새로운 자식을 추가합니다.

예시:

```cpp
// 우리는 NP 노드를 생성합니다
synode np({"pos": "np"});

// 그런 다음 세 개의 어휘 노드
synode det({"pos": "det", "surface": "the"});
synode adj({"pos": "adj", "surface": "big"});
synode noun({"pos": "noun", "surface": "dog"});

// np 아래에 하나씩 추가합니다
np.addchild(det);
np.addchild(adj);
np.addchild(noun);

// 들여쓰기된 방식으로 노드를 표시합니다
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 4);
    Display(x.next(), i);
}

Display(np, 0);
```

결과:
```
#0['pos': 'np']
    #0['pos': 'det', 'surface': 'the']
    #0['pos': 'adj', 'surface': 'big']
    #0['pos': 'noun', 'surface': 'dog']
```

synode가 의존성 변수가 아님을 나타내는 `#0`에 주목하세요.

## 의존성 유형

의존성은 두 개의 synode 사이의 관계입니다. 의존성은 직접 `dependency` 유형을 사용하여 생성할 수 있으며, 이후 `assertz`를 사용하여 지식 베이스에 저장하거나 의존성 규칙을 사용하여 생성할 수도 있습니다. 의존성은 이름, 특징 집합 및 인수 목록으로 구성됩니다.

메서드:
1. _initial([name, features, arg1, arg2, ...]): 이름(문자열), 특징 집합(mapss) 및 인수 목록으로 의존성을 생성합니다.
2. features(): 의존성 특징을 반환합니다.
3. name(): 의존성 이름을 반환합니다.
4. rule(): 이 의존성을 생성한 규칙 ID를 반환합니다.

예시:

```cpp
// 두 개의 어휘 노드를 생성합니다
synode det({"pos": "det", "surface": "the"});
synode noun({"pos": "noun", "surface": "dog"});

dependency d(["DET", {"direct": "+"}, det, noun]);

// 지식 베이스에 추가합니다
assertz(d);

println(d);
```

결과:
```
DET['direct': '+']({"pos": "det", "surface": "the"}, {"pos": "noun", "surface": "dog"})
```

## 의존성 규칙

의존성 규칙은 다음 패턴을 따릅니다:

```
If ([^|~] dep[features] (#x[features], #y) and/or dep(#w, #z)...)
    depres(#n, #nn), ..., depres(#n, #nn) / ~ / #x[..], #x[..].
```

이 규칙에서 `x`, `y`, `w`, `z`, `n`, `nn`은 synode 객체를 나타내는 정수입니다. 규칙은 지식 베이스의 의존성을 규칙에 저장된 의존성과 비교합니다. 일치하는 항목이 발견되면, 동일한 변수를 사용하여 새로운 의존성이 생성됩니다.

규칙은 함수 호출과 의존성과 함께 술어를 혼합할 수 있습니다. `^` 기호는 의존성이 수정될 것임을 나타냅니다. 규칙에서 한 번에 하나의 의존성만 수정할 수 있습니다. `~` 기호는 부정을 나타냅니다. 의존성 앞에 배치되면, 지식 베이스에 해당 의존성이 없어야 함을 의미합니다.

규칙의 출력을 `~`로 대체하면 규칙이 적용되지만 의존성은 생성되지 않습니다.

사실:
지식 베이스에 의존성을 추가하는 가장 간단한 방법은 사실로서 삽입하는 것입니다:

```
dep[features](#1, #2).
```

## 특징

의존성 규칙의 특징 구조는 다음 규칙을 따릅니다:
- 속성과 값 주위에 따옴표는 선택 사항입니다.
- `+` 기호는 하나의 값이 있는 속성의 기본값입니다.

연산자:
- `attribute`: 속성의 존재 여부를 확인합니다.
- `attribute: value`: 속성을 값과 비교합니다.
- `attribute: ~`: 속성은 어떤 값도 가져서는 안됩니다.
- `attribute ~: value`: 속성은 지정된 값이 아니어야 합니다.
- `attribute = value`: 속성에 지정된 값을 할당합니다.
- `attribute = ~`: 속성을 특징 집합에서 제거합니다.
- `attribute -: gram`: 속성을 TREG(Tamgu 정규 표현식)와 비교합니다.
- `attribute ~ -: gram`: 속성을 TREG와 비교하며, 실패해야 합니다.

Stop:
`stop` 술어는 문법의 평가를 중지하는 데 사용될 수 있습니다. 평가 엔진이 이 술어를 만나면, 나머지 규칙을 건너뛰고 문법의 평가를 중지합니다.

```cpp
If (stop) ~.
```

_dependencies():
이 메서드는 규칙을 지식 베이스에 적용하여 의존성 분석을 트리거합니다.

_setvalidfeatures(mapss features):
이 메서드는 synode와 의존성에 사용될 수 있는 유효한 특징에 대한 제약 조건을 설정합니다. 특징은 맵의 속성/값 쌍으로 정의됩니다. 단일 값으로 어떤 값을 가질 수 있는 속성(예: 단어의 lemma)은 빈 문자열이어야 합니다. 기본값은 `+`입니다.

예시:

```cpp
mapss feats = {'Obl': '+', 'lemma': '', 'c_person': '+', 'CR4': '+', 'Punct': '+', 'surface': ''};
_setvalidfeatures(feats);
```

## 예시

```cpp
// 들여쓰기된 방식으로 노드를 표시합니다
function Display(synode x, int i) {
    if (x == null)
        return;
    string sp;
    sp.fill(i, " ");
    println(sp, x);
    Display(x.child(), i + 5);
    if (i) // i가 0인 경우, 트리의 루트이므로 자매 노드를 표시하지 않습니다
        Display(x.next(), i);
}

// 구성 트리를 준비합니다
synode np1 = {"bar": 2};
synode np2({"bar": 2});
synode vp({"bar": 2});
synode s({"bar": 3});
synode v({"word": "eats", "pers": 3, "pres": "+", "verb": "+"});
synode d1({"word": "the", "det": "+"});
synode n1({"word": "dog", "noun": "+"});
synode d2({"word": "a", "det": "+"});
synode n2({"word": "bone", "noun": "+"});

s.addchild(np1);
s.addchild(vp);
vp.addchild(v, np2);
np1.addchild(d1, n1);
np2.addchild(d2, n2);

// synode가 맵처럼 사용될 수 있도록 기존 특징을 추가하거나 수정할 수 있습니다
vp["pos"] = "verb";
np1["pos"] = "noun";
np2["pos"] = "noun";

// 초기 의존성
subj(v, n1).
obj(v, n2).
det(n1, d1).
det(n2, d2);

// 다른 방법으로도 생성할 수 있습니다
dependency dpe(["mydep", {}, n1, n2]);

// 그러나 지식 베이스에 직접 추가해야 합니다
assertz(dpe);

// 이 함수는 아래의 규칙에서 호출됩니다. #x는 synode로 변환됩니다.
// 함수는 규칙이 실패하지 않도록 true를 반환해야 합니다.
function DTree(synode n) {
    Display(n, 0);
    println("---------------------------");
    return true;
}

// 노드를 반전시키는 간단한 규칙
If (subj(#1, #2)) inverted(#2, #1).

// 노드에 제약 조건을 사용하는 규칙
If (subj(#1[pres, pers: 3], #2) and obj(#1, #3)) arguments(#2, #1, #3).

// 의존성에 특징을 추가합니다
If (^subj(#1, #2)) subj[direct=+](#1, #2).

// 두 인수를 가진 모든 의존성을 탐색하기 위해 _를 사용하며, 두 노드가 다른지에 대한 제약 조건을 가집니다
If (_(#1, #2) && obj(#1, #3) && #2 != #3) link(#2, #3).

// 의존성 변수 _1과 _2를 사용하여 동일한 인수 사이에 의존성을 생성하지 않도록 합니다
If (_1(#1, #2) && obj_2(#1, #3) && _1 != _2) other(#2, #3).

// 의존성 규칙에서 노드 #1의 부모에 접근하기 위해 p_parent 술어를 사용합니다
// 그런 다음 DTree를 호출하여 표시합니다... DTree는 true를 반환해야 합니다. 그렇지 않으면 규칙이 실패합니다
// #3은 함수가 호출될 때 자동으로 synode 객체로 변환됩니다
If (det(#1, #2) && p_parent(#1, #3) && DTree(#3)) ~.

// TREG를 제약 조건으로 사용하는 규칙
If (obj(#1[word -: "e%a+"], #2)) Verb(#1).

// 의존성 파서를 실행합니다
_dependencies();

// 지식 베이스의 모든 의존성을 수집합니다
vector res = predicatedump();

Display(s, 0);
println("---------------------------");
printjln(res);
```

결과:
```
s
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
---------------------------
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
#0['bar': '3']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'the', 'det': '+']
    #0['word': 'dog', 'noun': '+', 'subject': '+']
#0['bar': '2', 'pos': 'verb']
    #0['word': 'eats', 'pers': '3', 'pres': '+', 'verb': '+']
#0['bar': '2', 'pos': 'noun']
    #0['word': 'a', 'det': '+']
    #0['object': '+', 'word': 'bone', 'noun': '+']
---------------------------
other({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
subj['direct': '+']({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"word": "dog", "noun": "+", "subject": "+"})
inverted({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
obj({"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
det({"word": "dog", "noun": "+", "subject": "+"}, {"word": "the", "det": "+"})
det({"object": "+", "word": "bone", "noun": "+"}, {"word": "a", "det": "+"})
arguments({"word": "dog", "noun": "+", "subject": "+"}, {"word": "eats", "pers": "3", "pres": "+", "verb": "+"}, {"object": "+", "word": "bone", "noun": "+"})
link({"word": "dog", "noun": "+", "subject": "+"}, {"object": "+", "word": "bone", "noun": "+"})
Verb({"word": "eats", "pers": "3", "pres": "+", "verb": "+"})
```