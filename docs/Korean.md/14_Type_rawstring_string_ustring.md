# 탐구(Tamgu)에서의 문자열 조작

탐구는 문자열을 다루기 위해 `string`, `ustring`, `rawstring` 세 가지 타입을 제공합니다.

- `string` 타입은 어떤 종류의 문자열도 다루기 위해 사용됩니다. 문자열에서 부분 문자열, 문자 또는 패턴 인식을 적용하는 다양한 메서드를 제공합니다.
- `ustring` 타입은 매우 큰 문자열에 대한 빠른 액세스를 제공하기 위해 사용됩니다. 시스템은 전체 문자열에 대해 하나의 단일 인코딩을 가정하기 때문에 이를 기반으로 합니다. 이는 C++의 `wstring` 구현을 기반으로 합니다.
- `rawstring` 타입은 바이트 수준에서 문자열을 처리합니다. 문자열을 허용하지만 바이트 수준에서 처리합니다. 이는 특정 보호 기능이 없으며 스레드에서 자유롭게 액세스하고 수정할 수 있습니다. 그러나 백그라운드에서 스레드가 실행 중인 경우 `rawstring`을 크기 조정할 수 없습니다.

## 메서드

### 기본 변환

- `base(int b, bool toconvert)`: 기본 `b`에 해당하는 문자열 숫자 내용에 대한 숫자 값 반환합니다. `toconvert` 매개변수는 선택 사항입니다. `false`인 경우 변환할 숫자는 10진법이며 기본 `b`에 해당하는 숫자로 변환됩니다.
- `base(vector chrs)`: 주어진 기본에 대한 각 자릿수의 인코딩을 설정합니다. 기본 세트는 64개의 문자로 구성됩니다: 0-9, A-Z, a-z, #, @. 이 기본 문자 집합을 사용자 지정 문자 집합으로 바꿀 수 있습니다. 빈 벡터를 제공하면 시스템은 문자열의 기본 집합으로 재설정됩니다.

### 문자열 추출 및 조작

- `bytes()`: 문자열과 일치하는 바이트의 ivector를 반환합니다.
- `charposition(int pos)`: 바이트 위치를 문자 위치로 변환합니다 (특히 UTF8 문자열에서 유용).
- `deaccentuate()`: 강세 기호가 있는 문자에서 강세 기호를 제거합니다.
- `doublemetaphone()`: 문자열의 이중 메타폰 인코딩을 포함하는 svector를 반환합니다.
- `dos()`: 문자열을 DOS 인코딩으로 변환합니다.
- `dostoutf8()`: DOS 문자열을 UTF8 인코딩으로 변환합니다.
- `emoji()`: 이모지의 텍스트 설명(영어로)을 반환합니다.
- `evaluate()`: 문자열 내의 메타 문자를 평가하고 평가된 문자열을 반환합니다.
- `extract(int pos, string from, string up1, string up2...)`: 현재 문자열에서 시작 위치 `pos`에서 시작하여 `from`에서 `up1`, `up2` 등 다음 문자열 중 하나로 구성된 모든 부분 문자열을 포함하는 svector를 반환합니다.
- `fill(int nb, string char)`: `nb`개의 문자로 이루어진 문자열을 생성합니다.
- `find(string sub, int pos)`: 위치 `pos`에서 시작하는 부분 문자열 `sub`의 위치를 반환합니다.
- `format(p1, p2, p3)`: 각 `%x`가 인수 중 하나와 연결된 현재 문자열에서 새 문자열을 생성합니다. 여기서 `x`는 인수 목록에서 해당 매개변수의 위치입니다. `x`는 1부터 시작합니다.
- `geterr()`: 현재 오류 출력을 캐치합니다.
- `getstd()`: 현재 표준 출력을 캐치합니다.
- `html()`: 문자열을 HTML 호환 문자열 또는 문자열 벡터로 반환합니다.
- `insert(i, s)`: 문자열 `s`를 위치 `i`에 삽입합니다. `i`가 -1이면 입력 문자열의 각 문자 사이에 `s`를 삽입합니다.
- `isalpha()`: 문자열이 알파벳 문자로만 구성되어 있는지 테스트합니다.
- `isconsonant()`: 문자열이 자음으로만 구성되어 있는지 테스트합니다.
- `isdigit()`: 문자열이 숫자로만 구성되어 있는지 테스트합니다.
- `isemoji()`: 문자열이 이모지로만 구성되어 있는지 테스트합니다.
- `islower()`: 문자열이 소문자로만 구성되어 있는지 테스트합니다.
- `ispunctuation()`: 문자열이 구두점 기호로 구성되어 있는지 테스트합니다.
- `isupper()`: 문자열이 대문자로만 구성되어 있는지 테스트합니다.
- `isutf8()`: 문자열이 UTF8 문자를 포함하는지 테스트합니다.
- `isvowel()`: 문자열이 모음으로만 구성되어 있는지 테스트합니다.
- `last()`: 마지막 문자를 반환합니다.
- `latin()`: UTF8 문자열을 LATIN으로 변환합니다.
- `left(int nb)`: 문자열의 첫 `nb`개 문자를 반환합니다.
- `levenshtein(string s)`: Levenshtein 알고리즘에 따라 `s`와의 편집 거리를 반환합니다.
- `parenthetic()`: 괄호식을 벡터로 변환합니다.
- `parenthetic(string opening, string closing, bool comma, bool separator, bool keepwithdigit, svector rules)`: 괄호식을 사용하여 재귀적인 표현을 벡터로 변환합니다. 여는 문자열과 닫는 문자열을 구분자로 사용합니다.
- `lower()`: 문자열을 소문자로 변환합니다.
- `mid(int pos, int nb)`: 문자열의 위치 `pos`에서 시작하여 `nb`개의 문자를 반환합니다.
- `ngrams(int n)`: 순위 `n`의 모든 n-gram의 벡터를 반환합니다.
- `ord()`: 문자열 문자의 코드를 반환합니다.
- `parse()`: 문자열을 코드 조각으로 구문 분석하고 벡터로 평가 결과를 반환합니다.
- `pop()`: 마지막 문자를 제거합니다.
- `pop(i)`: 위치 `i`의 문자를 제거합니다.
- `read(string file)`: 파일을 문자열로 읽어옵니다.
- `removefirst(int nb)`: 문자열의 처음 `nb`개 문자를 제거합니다.
- `removelast(int nb)`: 문자열의 마지막 `nb`개 문자를 제거합니다.
- `replace(sub, str)`: `sub`와 일치하는 부분 문자열을 `str`로 대체합니다.
- `reverse()`: 문자열을 뒤집습니다.
- `rfind(string sub, int pos)`: 위치 `pos`에서 시작하여 뒤로 가면서 부분 문자열 `sub`의 위치를 반환합니다.
- `right(int nb)`: 문자열의 마지막 `nb`개 문자를 반환합니다.
- `scan(rgx, string sep, bool immediatematch, string remaining)`: Tamgu 정규 표현식 형식에 따라 `rgx`와 일치하는 모든 부분 문자열을 반환합니다.
- `size()`: 문자열의 길이를 반환합니다.
- `slice(int n)`: 문자열을 크기 `n`의 부분 문자열로 나눕니다.
- `split(string splitter)`: 문자열을 `splitter`를 따라 분할하고 결과를 svector에 저장합니다.
- `splite(string splitter)`: `split`과 같은 방식으로 문자열을 분할하지만 빈 문자열을 최종 결과에 유지합니다.
- `multisplit(string sp1, string sp2...)`: 여러 구분자 문자열을 따라 문자열을 분할합니다.
- `stokenize(map keeps)`: 문자열을 단어와 구두점으로 토큰화합니다.
- `tags(string o, string c, bool comma, bool separator, bool keepwithdigit, svector rules)`: 문자열을 괄호식으로 구문 분석하여 결과로서 여는 문자열과 닫는 문자열을 제공합니다.
- `tokenize(bool comma, bool separator, svector rules)`: 문자열을 단어와 구두점으로 토큰화합니다.
- `trim()`: 끝에 있는 문자를 제거합니다.
- `trimleft()`: 왼쪽에 있는 문자를 제거합니다.
- `trimright()`: 오른쪽에 있는 문자를 제거합니다.
- `upper()`: 문자열을 대문자로 변환합니다.
- `utf8()`: LATIN 문자열을 UTF8로 변환합니다.
- `utf8(int part)`: ISO 8859 일부로 인코딩된 Latin 문자열을 UTF8로 변환합니다.
- `write(string file)`: 문자열 내용을 파일에 작성합니다.

## 문자열 처리

탐구는 한국어 문자열과 라틴 테이블에 대한 특정 메서드도 제공합니다. 이러한 메서드는 `string` 및 `ustring` 타입과 함께 사용됩니다.

### 한국어 특정 메서드

- `ishangul()`: 한글 문자인 경우 `true`를 반환합니다.
- `isjamo()`: 한글 자모인 경우 `true`를 반환합니다.
- `jamo(bool combine)`: `combine`이 `false`이거나 생략된 경우 한글 자모를 주요 자음과 모음 구성 요소로 분리합니다. `combine`이 `true`인 경우 내용을 자모로 결합합니다.
- `normalizehangul()`: 다른 UTF8 인코딩의 한글 문자를 표준화합니다.
- `romanization()`: 한글 문자의 로마자 표기법입니다.

### 라틴 테이블

탐구는 이러한 인코딩 이름과 상수를 사용하여 이러한 인코딩에 액세스할 수 있습니다. 인코딩 이름은 다음과 같습니다:

- `e_latin_we`: 서유럽어
- `e_latin_ce`: 중앙 유럽어
- `e_latin_se`: 남유럽어
- `e_latin_ne`: 북유럽어
- `e_cyrillic`: 키릴 문자
- `e_arabic`: 아랍어
- `e_greek`: 그리스어
- `e_hebrew`: 히브리어
- `e_turkish`: 터키어
- `e_nordic`: 북유럽어
- `e_thai`: 태국어
- `e_baltic`: 바르트 해안
- `e_celtic`: 켈트어
- `e_latin_ffe`: 확장 (프랑스어, 핀란드어, 에스토니아어)
- `e_latin_see`: 동남 유럽어
- `e_windows`: Windows 인코딩

## 연산자

탐구는 문자열 조작에 대해 여러 연산자를 지원합니다:

- `sub in s`: `sub`가 `s`의 부분 문자열인지 테스트합니다.
- `for (c in s) {…}`: `s`의 모든 문자에 대해 반복합니다.
- `+`: 두 개의 문자열을 연결합니다.
- `"…"`: 메타 문자인 `\n`, `\t`, `\r`, `\"`와 같이 해석되는 문자열을 정의합니다.
- `'…'`: 메타 문자가 해석되지 않는 문자열을 정의합니다.

## 인덱스

탐구는 인덱스를 사용하여 문자열을 색인화하고 수정할 수 있습니다:

- `str[i]`: 문자열의 `i`번째 문자를 반환합니다.
- `str[i:j]`: `i`와 `j` 사이의 부분 문자열을 반환합니다.
- `str[s..]`: 문자열 `s`에서 시작하는 부분 문자열을 반환합니다.
- `str[-s..]`: 문자열 끝에서부터 문자열 `s`에서 시작하는 부분 문자열을 반환합니다.

인덱스가 범위를 벗어나면 `erroronkey` 플래그가 `false`로 설정되지 않는 한 예외가 발생합니다.

## 예제

다음은 탐구에서 문자열 조작의 몇 가지 예입니다:

```tamgu
string s;
string x;
vector v;

// 기본적인 문자열 조작
s = "12345678a";
x = s[0];  // 값=1
x = s[2:3];   // 값=3
x = s[2:-2];  // 값=34567
x = s[3:];  // 값=45678a
x = s[:"56"];  // 값=123456
x = s["2":"a"];  // 값=2345678a
s[2] = "ef";  // 값=빈 문자열

// 마지막 3개 문자
x = s.right(3);  // 값=78a

// 공백을 기준으로 분할
s = "a b c";
v = s.split(" ");  // v=["a", "b", "c"]

// 정규식, x는 문자열이며 정규식의 첫 번째 일치 항목을 찾습니다
x = s.scan("%d%d%c");  // 값=78a

// 패턴을 사용하여 문자열을 분할합니다
s = "12a23s45e";
v = s.scan("%d%d%c");  // 값=['12a', '23s', '45e']

x = s.replace(r"%d%ds", "X"); // 값=12aX45e

// Tamgu 정규식과 마찬가지로 %x 변수를 사용할 수도 있습니다
x = s.replace(r"%d%1s", "%1"); // 값=12a2345e

// 정규식: 모든 플랫폼에서 사용할 수 없습니다
preg rgx(p"\w+day");
string str = "Yooo Wesdenesday Saturday";
vector vrgx = rgx in str; // ['Wesdenesday', 'Saturday']
string s = rgx in str; // Wesdenesday
int i = rgx in str; // 위치는 5

// (…)를 사용하여 특정 토큰을 분리하여 벡터에 저장합니다
rgx = p"(\d{1,3}):( \d{1,3}):( \d{1,3}):( \d{1,3})";
str = '1:22:33:444';
vrgx = str.split(rgx);  // [1, 22, 33, 444], rgx는 분할 표현식입니다

str = '1:22:33:4444';
vrgx = str.split(rgx);  // [] (4444는 4자리 숫자입니다)

str = "A_bcde";
// 전체 일치가 필요합니다
if (p'[a-zA-Z]_.+' == str)
    println("Yooo"); // Yooo

// 이것은 다음과 동일합니다:
rgx = p'[a-zA-Z]_.+';
if (rgx.match(str))
    println("Yooo bis");

str = "ab(Tamgu 12,Tamgu 14,Tamgu 15,Tamgu 16)";
v = str.extract(0, "Tamgu ", ",", ")");
// 결과: ['12', '14', '15', '16']

string frm = "this %1 is a %2 of %1 with %3";
str = frm.format("tst", 12, 14);
println(str); // 결과: this tst is a 12 of tst with 14
```