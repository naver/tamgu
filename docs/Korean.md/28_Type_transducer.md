# 타입 변환기

이 타입은 매우 간결하고 효율적인 방식으로 렉시콘을 저장하고 처리하는 데 중점을 둡니다. 다음과 같은 메서드를 노출합니다:

## 메서드

1. `add(container, bool norm, int encoding)`: 컨테이너(벡터 또는 맵)를 변환기 렉시콘으로 변환합니다. 컨테이너가 벡터인 경우 값의 개수는 짝수여야 합니다.
2. `build(string input, string output, bool norm, int encoding)`: 표면 형태가 첫 번째 줄에 나오고 렘마+특징이 다음 줄에 나오는 텍스트 파일에서 변환기 파일을 빌드합니다.
3. `compilergx(string rgx, svector features, string filename)`: 정규 표현식으로부터 변환기 파일을 빌드합니다. 파일 이름은 선택 사항이며, 결과 자동화기는 파일에 저장됩니다.
4. `load(string file)`: 변환기 파일을 로드합니다.
5. `lookdown(string lemFeat, byte lemma)`: 렘마와 특징과 일치하는 표면 형태를 검색합니다. 렘마는 선택 사항입니다. 1 또는 2인 경우 비교 대상 문자열은 렘마만으로 축소될 수 있습니다. 렘마가 2인 경우 특징도 반환됩니다. 참고: 렘마와 특징은 탭으로 구분되어야 합니다.
6. `lookup(string wrd, int threshold, int flags)`: 변환기와 잠재적인 작업 세트 및 임계값과 결합하여 단어를 조회합니다. 마지막 두 인수는 생략할 수 있습니다. 가능한 작업은 다음과 같습니다:
   - `a_first`: 자동화기는 첫 번째 문자에 작업을 적용할 수 있습니다.
   - `a_change`: 자동화기는 문자를 다른 문자로 변경할 수 있습니다.
   - `a_delete`: 자동화기는 문자를 삭제할 수 있습니다.
   - `a_insert`: 자동화기는 문자를 삽입할 수 있습니다.
   - `a_switch`: 자동화기는 두 문자를 교환할 수 있습니다.
   - `a_nocase`: 자동화기는 두 문자가 대소문자에 관계없이 일치하는지 확인할 수 있습니다.
   - `a_repetition`: 자동화기는 문자가 몇 번 반복되는지 허용할 수 있습니다.
   - `a_vowel`: 자동화기는 강조 기호가 제거된 모음을 함께 비교합니다. 예를 들어 "e"는 "é" 또는 "è"와 일치하지만 "a"와는 일치하지 않습니다.
   - `a_surface`: 자동화기는 표면 형태만 반환합니다.
7. `parse(string sentence, int option, int threshold, int flags)`: 변환기를 사용하여 단어의 시퀀스를 분석합니다. option은 다음 값을 가질 수 있습니다:
   - `0`: 초기 문자열에서 인식된 표면 형태만 반환합니다.
   - `1`: `0`과 동일하지만 오프셋도 포함됩니다.
   - `2`: 초기 입력에서 인식된 표면 형태와 렘마와 특징을 반환합니다.
   - `3`: `2`와 동일하지만 오프셋도 포함됩니다. 임계값과 플래그는 선택 사항이며, `lookup`과 동일한 규칙을 따릅니다.
8. `store(string output, bool norm, int encoding)`: 변환기를 파일에 저장합니다. 마지막 두 매개변수는 선택 사항입니다.

## 형식

`build` 또는 `add`를 통해 렉시콘으로 컴파일된 파일의 형식은 유사한 구조를 가집니다. 파일의 경우 첫 번째 줄은 표면 형태이고, 다음 줄은 탭으로 구분된 렘마와 일부 특징이어야 합니다.

예시:
```
classes
class +Plural+Noun
class
class +Singular+Noun
```

`build` 함수는 이러한 줄을 기반으로 해당 변환기를 포함하는 파일을 생성합니다. 두 번째 매개변수는 단어 또는 텍스트를 처리할 때 사용됩니다.

- 정규화는 대소문자를 구분하지 않고 단어를 일치시킬 수 있도록 합니다. 따라서 이 렉시콘은 "CLASS"를 단어로 인식합니다.
- 시스템은 UTF8 인코딩으로 단어를 인식하도록 구현되었습니다(실제로 변환기는 유니코드로 저장됩니다). 그러나 시스템에게 라틴 인코딩을 고려하는 방법을 알려줄 수 있습니다. 예를 들어, 이 경우에는 라틴 5로서의 5를 시스템에 제공할 수 있습니다. 기본값은 라틴 1입니다.

## 벡터

`add`에 벡터를 입력으로 사용하는 경우 구조가 약간 다릅니다. 벡터의 짝수 위치는 표면 형태이고, 홀수 위치는 다시 렘마와 특징으로 구분됩니다(탭으로 구분됨).

## 맵

맵의 경우 키는 표면 형태이고, 값은 렘마와 특징입니다. 맵은 실제로 모호한 단어를 저장하는 데 문제가 될 수 있습니다.

## 문자열 처리

변환기로 문자열을 처리하는 다양한 방법이 있습니다.

### `lookup`

`lookup`은 단어가 변환기에 속하는지 여부를 감지하고, 이 경우 그 렘마와 특징을 반환합니다. 변환기는 하나 이상의 솔루션을 반환할 수 있습니다. 결과 변수는 모든 가능한 솔루션을 검색하려면 벡터여야 합니다.

예시:
```cpp
t.lookup("class") 반환값: class +Singular+Noun
```

문자열 처리를 편집 거리 임계값과 작업으로 제한할 수 있습니다.

```cpp
t.lookup("cliss", 1, a_change) 반환값: class +Singular+Noun
```

### `lookdown`

`lookdown`은 렘마와 특징을 사용하여 단어의 올바른 표면 형태를 검색하는 데 사용됩니다.

예시:
```cpp
t.lookdown("class +Plural+Noun") 반환값: classes
```

### `parse`

`parse`는 문자열을 토큰으로 분할하고, 각 토큰에 대해 가능한 모든 렘마+특징을 벡터로 반환합니다.

예시:
```cpp
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
['glass', 'glass +18+23+18+23+Noun+Sg+NOUN', 'glass +18+23+18+23+Verb+Trans+Pres+Non3sg+VERB']
['of', 'of +24+26+24+26+Prep+PREP']
['milk', 'milk +27+31+27+31+Verb+Trans+Pres+Non3sg+VERB', 'milk +27+31+27+31+Noun+Sg+NOUN']
['.', '. +31+32+31+32+Punct+Sent+SENT']
```

참고: `parse`는 초기 문장에서 각 단어의 위치도 반환합니다.

## 정규 표현식

변환기가 처리하는 정규 표현식은 매우 제한적입니다:

1. `%c`: 문자를 정의합니다. `c`는 UTF8 문자입니다.
2. `$..`: 문자열을 정의합니다.
3. `u-u`: 두 유니코드 문자 사이의 간격을 정의합니다.
4. `[..]`: 문자 시퀀스를 정의합니다.
5. `{...}`: 문자열의 배타적인 선택을 정의합니다.
6. `.+`: 구조가 적어도 한 번 발생해야 합니다.
7. `(..)`: 선택적 구조를 정의합니다.
8. `!n`: 특징 구조를 특징 벡터의 번호와 함께 삽입합니다(n>=1).

예시:
```cpp
transducer t;
// 이 표현식은 로마 숫자를 인식합니다.
t.compilergx("{DMCLXVI}+!1", ["\t+Rom"]);
// 이 표현식은 소수점과 지수 표현을 포함한 모든 종류의 숫자를 인식합니다.
t.compilergx("({-+}){0-9}+!1(%.{0-9}+!2({eE}({-+}){0-9}+!3))", ["+Card", "+Dec", "+Exp+Dec"]);
// 서수를 인식하기 위해
t.compilergx("{[1st][2nd][3rd]}!1", ["+Ord"]);
t.compilergx("[3-9]([0-9]+)$th!1", ["+Ord"]);
// 그리스 문자로 이루어진 모든 문자열을 인식하려고 합니다.
t.compilergx("{α-ω0-9}+!1", ["+Greek"]);

int i;
string s;
for (i in <945,970,1>) s += i.chr();
println(t.lookup("MMMDDD")); // MMMDDD +Rom
println(t.lookup("1234")); // 1234 +Card
println(t.lookup("1.234")); // 1.234 +Dec
println(t.lookup("1.234e-8")); // 1.234e-8 +Exp+Dec
println(t.lookup("1st")); // 1st +Ord
println(t.lookup("2nd")); // 2nd +Ord
println(t.lookup("3rd")); // 3rd +Ord
println(t.lookup("4th")); // 4th +Ord
println(t.lookup(s)); // αβγδεζηθικλμνξοπρςστυφχψ +Greek
```