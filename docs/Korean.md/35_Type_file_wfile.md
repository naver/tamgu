# 파일 관리

`file` 타입은 입력 및 출력 파일을 관리하는 데 사용됩니다. `wfile` 타입은 특히 UTF16 (UCS-2) 파일을 처리하는 데 사용됩니다.

## 메소드

1. `eof()`: 파일의 끝에 도달하면 true를 반환합니다.
2. `file f(string filename, string mode_read)`: 지정된 `mode_read`에 따라 파일을 엽니다. 파일이 읽기 모드인 경우, `mode_read`는 선택 사항입니다. `mode_read`에 가능한 값은 다음과 같습니다:
   - "a": 추가
   - "r": 읽기
   - "w": 쓰기
   - "w+": 추가
3. `find(string s, bool case_insensitive)`: 파일에서 문자열 `s`가 발견된 모든 위치를 반환합니다.
4. `get()`: 파일에서 한 문자를 읽습니다.
5. `get_signature()`: 파일에 시그니처가 포함되어 있는지 여부를 반환합니다.
6. `open_append(string filename)`: 추가 모드로 파일을 엽니다.
7. `open_read(string filename)`: 읽기 모드로 파일을 엽니다.
8. `open_write(string filename)`: 쓰기 모드로 파일을 엽니다.
9. `read()`: 파일 전체를 변수에 읽어옵니다. 변수의 타입은 다음 중 하나일 수 있습니다:
   - string: 문서 전체가 하나의 문자열로 저장됩니다.
   - svector: 문서가 개행 문자를 기준으로 문자열로 분리되어 컨테이너에 저장됩니다.
   - bvector: 문서가 바이트 단위로 컨테이너에 저장됩니다.
   - ivector: 문서가 바이트 단위로 컨테이너에 저장됩니다.
10. `read(int nb)`: `read()`와 유사하지만, 파일에서 "nb"개의 문자 또는 바이트만 추출합니다.
11. `readln()`: 파일에서 한 줄을 읽습니다.
12. `seek(int p)`: 파일 커서를 위치 `p`로 이동시킵니다.
13. `set_signature(bool s)`: UTF8 또는 UTF16 시그니처를 설정합니다.
14. `tell()`: 파일 커서의 위치를 반환합니다.
15. `unget()`: 스트림에 한 문자를 반환합니다.
16. `unget(nb)`: 스트림에 `nb`개의 문자를 반환합니다.
17. `scan(string grammar, string separator)`: Tangu 정규식을 사용하여 파일을 읽고, 부분 문자열의 벡터를 반환합니다. 구분자는 필드 간 구분을 위한 문자입니다 (기본값은 공백).
18. `write(string s1, string s2, ...)`: 문자열 `s`를 파일에 씁니다.
19. `writelen(string s1, string s2, ...)`: 문자열을 파일에 씁니다. 각 문자열은 공백으로 구분되고, 줄 끝에 개행 문자가 추가됩니다.
20. `writebin(int s1, int s2, ...)`: 바이트를 파일에 씁니다. 값이 컨테이너인 경우, 해당 컨테이너에서 바이트 목록을 씁니다.

## 시그니처

UTF-8 및 UTF-16 파일은 시작 부분에 시그니처를 가질 수 있습니다. UTF-8 파일은 시그니처를 위해 세 개의 옥텟을 사용하고, UTF-16 파일은 두 개의 옥텟을 사용합니다.

- `file` 타입을 사용할 때, 시그니처는 미리 설정되어야 읽을 수 있습니다. 이 타입은 UTF-8 또는 이진 파일을 읽는 데에만 사용할 수 있습니다.
- `wfile`의 경우, 파일의 시작 부분에서 시그니처를 발견하면 자동으로 설정됩니다. 이 타입은 UTF-16 (UCS-2) 파일을 읽는 데에만 사용할 수 있습니다.

## 연산자

- `x in file`: `x`가 문자열인 경우, 파일에서 한 줄을 받습니다. `x`가 벡터인 경우, 해당 줄을 벡터의 맨 위에 추가합니다. `x`가 정수 또는 부동 소수점인 경우, 스트림에서 한 문자만 검색합니다.

## 예제

```cpp
file f;
f.open_read(path);
string s;
svector words;
string w;

for (s in f) { // in 연산자 사용
    s = s.trim();
    words = s.split(" ");
    for (w in words)
        print("word:", w, endl);
}

f.close();
```

## 표준 입력: stdin

Tamgu는 표준 입력을 처리하기 위해 `stdin` 변수를 제공합니다. 이 변수는 예를 들어 파이프된 파일에서 오는 데이터를 처리하는 데 유용합니다.

### 예제

```cpp
string s;
int i = 1;

for (s in stdin) {
    println(i, s);
    i++;
}
```

이러한 줄들을 작은 파일에 저장하고, 예를 들어 `stdin.Tamgu`라고 한다면 파이프된 문자열의 내용은 각 줄마다 특정 번호와 함께 표시됩니다:

```bash
echo "The lady is happy" | Tamgu stdin.Tamgu
```

### 예제 (scan)

텍스트에는 다음과 같은 줄들이 포함되어 있습니다:
```
456 -0x1.16bca4f9165dep -3 0x1.0d0e44bfeec9p -3
```

```cpp
file f(_current + "tst.txt");
// 복잡한 16진수 문자열을 읽기 위한 매크로 정의
grammar_macros("X", "({%+ -})0x%x+(%.%x+)(p({%+ -})%d+)");
// 첫 두 필드를 읽습니다. 공백은 기본 구분자입니다.
uvector v = f.scan("%d+ %X ");
println(v); // ['456', ' -0x1.16bca4f9165dep -3']
// 다음 필드를 읽습니다.
v = f.scan("%X ");
println(v); // ['0x1.0d0e44bfeec9p -3']
f.close();
```