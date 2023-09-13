35. 탐구(Tamgu)에서 "file"과 "wfile" 타입

이 타입은 입력과 출력 파일을 관리하기 위해 사용됩니다. "wfile" 타입은 특히 UTF16(UCS-2) 파일을 처리하는 데 사용됩니다.

35.1 메소드

1. eof(): 파일의 끝에 도달하면 true를 반환합니다.
2. file f(string filename, string mode_read): 지정된 모드에 따라 파일을 엽니다. 파일이 읽기 모드인 경우, "mode_read" 매개변수는 선택적입니다. "mode_read"에 가능한 값은 다음과 같습니다:
   a. "a": 추가
   b. "r": 읽기
   c. "w": 쓰기
   d. "w+": 추가
3. find(string s, bool case_insensitive): 파일에서 문자열 "s"가 발견된 모든 위치를 반환합니다.
4. get(): 파일에서 한 문자를 읽습니다.
5. getsignature(): 파일에 서명이 있는지 여부를 반환합니다.
6. openappend(string filename): 추가 모드로 파일을 엽니다.
7. openread(string filename): 읽기 모드로 파일을 엽니다.
8. openwrite(string filename): 쓰기 모드로 파일을 엽니다.
9. read(): 파일 전체를 변수에 읽어들입니다. 변수는 다음과 같을 수 있습니다:
   a. string: 문서 전체가 하나의 문자열에 저장됩니다.
   b. svector: 문서가 개행 문자를 기준으로 문자열로 분할되어 컨테이너에 저장됩니다.
   c. bvector: 문서가 한 바이트씩 컨테이너에 저장됩니다.
   d. ivector: 문서가 한 바이트씩 컨테이너에 저장됩니다.

연산자
x in file: x가 문자열인 경우, 파일에서 한 줄을 받아옵니다. x가 벡터인 경우, 줄을 벡터의 맨 위에 추가합니다. x가 정수나 실수인 경우, 스트림에서 한 문자만 가져옵니다.

예제
file f;
f.openread(path);
string s;
svector words;
string w;
for (s in f) { // "in" 연산자 사용
    s = s.trim();
    words = s.split(" ");
    for (w in words)
        print("word:", w, endl);
}
f.close();

35.2 표준 입력: stdin
탐구는 표준 입력을 처리하기 위해 "stdin" 변수를 제공합니다. 이 변수는 예를 들어 파이프로 연결된 파일에서 데이터를 처리하는 데 유용합니다.

예제
string s;
int i = 1;
for (s in stdin) {
    println(i, s);
    i++;
}

만약 이러한 줄들을 "stdin.tamgu"라는 작은 파일에 저장한다면, 파이프로 연결된 문자열의 내용은 각 줄에 대해 특정 번호와 함께 표시됩니다:
echo "The lady is happy" | Tamgu stdin.tamgu.