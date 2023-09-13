## 문자열 조작 예제

다음은 문자열 조작의 몇 가지 예제입니다:

```cpp
string s;
string x;
vector<string> v;

// 기본적인 문자열 조작
s = "12345678a";
x = s[0];            // 값 = '1'
x = s.substr(2, 2);  // 값 = '34'
x = s.substr(2);     // 값 = '345678a'
x = s.substr(0, 5);  // 값 = '12345'
x = s.substr(2, s.find('a') - 2);  // 값 = '34567'
s[2] = "ef";         // 값 = '12ef5678a'

// 마지막 3개의 문자
x = s.substr(s.length() - 3);  // 값 = '78a'

// 공백을 기준으로 나누기
s = "a b c";
v = split(s, ' ');  // v = ['a', 'b', 'c']

// 정규 표현식을 사용하여 첫 번째 일치 항목 찾기
x = regex_find(s, "\\d\\d\\w");  // 값 = '78a'

// 패턴을 사용하여 문자열 나누기
s = "12a23s45e";
v = regex_split(s, "\\d\\d\\w");  // 값 = ['12a', '23s', '45e']

// 정규 표현식을 사용하여 대체하기
x = regex_replace(s, "\\d\\d\\w", "X");  // 값 = '12aX45e'
x = regex_replace(s, "\\d\\d\\w", "$1");  // 값 = '12a2345e'

// 정규 표현식: 모든 플랫폼에서 사용할 수 없음
preg rgx("\\w+day");
string str = "Yooo Wesdenesday Saturday";
vector<string> vrgx = rgx.find_all(str);  // ['Wesdenesday', 'Saturday']
```

제공된 코드는 C++로 작성되었습니다. 예제에서 사용된 일부 함수는 모든 플랫폼에서 사용할 수 없을 수 있습니다.