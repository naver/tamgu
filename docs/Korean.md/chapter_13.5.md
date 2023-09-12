## Tamgu에서의 Posix 정규 표현식

Tamgu에서는 Tamgu 정규 표현식과 마찬가지로 Posix 정규 표현식을 사용할 수 있습니다. 그러나 두 가지 작은 차이가 있습니다. 첫째, Posix 정규 표현식을 사용하려면 표현식 앞에 "r" 대신 "p"를 접두사로 붙여야 합니다. 둘째, Posix 정규 표현식과 관련된 객체 유형은 "preg"입니다.

이러한 정규 표현식은 Posix 정규 표현식 체계를 기반으로 합니다. 문자열과 파일에서 스캔 메소드를 제외한 모든 곳에서 treg 대신 사용할 수 있습니다.

### 예제

```cpp
string str = "this subject has a submarine as a subsequence";
svector vs = p'\\b(sub)([^ ]*)' in str;
println(vs); // ['subject','submarine','subsequence']

ivector iv = p'\\b(sub)([^ ]*)' in str;
println(iv); // [5,12,19,28,34,45]

string s = p'\\b(sub)([^ ]*)' in str;
println(s); // subject

s = str[p"\\b(sub)([^ ]*)"];
println("S=" + s); // S=subject

s = str[-p"\\b(sub)([^ ]*)"];
println("S=" + s); // S=subsequence
```