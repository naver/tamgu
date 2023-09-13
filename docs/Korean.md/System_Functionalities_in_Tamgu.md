# Tamgu에서의 시스템 기능

Tamgu는 디렉토리의 내용을 벡터로 읽거나 시스템 명령을 실행하는 등 다양한 시스템 기능을 제공합니다. `_sys`라는 변수를 노출시켜 다음과 같은 메서드에 접근해야 합니다:

## 메서드

1. `command(string s, string outputfile)`: 시스템 명령어 `s`를 실행합니다. `outputfile`은 선택적으로 사용되며 명령어 출력(stdout)을 리디렉션하기 위해 사용됩니다. 만약 `outputfile`이 제공되면, `command`는 또한 해당 파일의 내용을 문자열로 반환합니다.

2. `createdirectory(string path)`: 주어진 경로에 디렉토리를 생성합니다. 디렉토리가 이미 존재하거나 생성할 수 없는 경우 false를 반환합니다.

3. `env(string var)`: 환경 변수 `var`의 값을 반환합니다.

4. `env(string var, string value)`: 환경 변수 `var`의 값을 설정합니다.

5. `listdirectory(string path)`: 디렉토리의 파일들을 벡터로 반환합니다.

6. `ls(string path)`: 디렉토리의 파일들을 벡터로 반환합니다.

7. `mkdir(string path)`: 주어진 경로에 디렉토리를 생성합니다. 디렉토리가 이미 존재하거나 생성할 수 없는 경우 false를 반환합니다.

8. `fileinfo(string path)`: 주어진 파일에 대한 다음 정보를 담은 맵을 반환합니다:
   - `info["size"]`: 파일의 크기
   - `info["date"]`: 파일의 날짜
   - `info["change"]`: 최근 변경 날짜
   - `info["access"]`: 최근 접근 날짜
   - `info["directory"]`: 경로가 디렉토리인 경우 true
   - `info["pathname"]`: 실제 경로명

9. `pipe(string cmd)`: 명령어 `cmd`를 실행하고 그 결과로 출력된 벡터를 반환합니다.

10. `realpath(string path)`: 상대 경로에 대한 실제 경로를 반환합니다.

11. `getchar()`: 마지막으로 입력된 문자를 반환합니다. `getchar`는 화면에 문자를 표시하지 않습니다.

12. `colors(int style, int code1, int code2, bool disp)`: 텍스트 색상을 설정하고 색상 문자열을 반환합니다.

13. `foregroundcolor(int color)`: 전경 텍스트 색상을 설정합니다.

14. `backgroundcolor(int color)`: 배경 텍스트 색상을 설정합니다.

15. `rgbforegroundcolor(int red, int green, int blue)`: RGB 전경 텍스트 색상을 설정합니다.

## 변수

이 라이브러리에서는 몇 가지 변수도 노출되어 있습니다:
- `_sys_keyright`: 오른쪽 화살표
- `_sys_keyleft`: 왼쪽 화살표
- `_sys_keydown`: 아래쪽 화살표
- `_sys_keyup`: 위쪽 화살표
- `_sys_keydel`: Del 키
- `_sys_keyhomekey`: Home 키
- `_sys_keyendkey`: End 키
- `_sys_keyc_up`: Control + 위쪽 화살표
- `_sys_keyc_down`: Control + 아래쪽 화살표
- `_sys_keyc_right`: Control + 오른쪽 화살표
- `_sys_keyc_left`: Control + 왼쪽 화살표
- `_sys_keybackspace`: Backspace
- `_sys_keyescape`: Escape 키

Windows에서는 다음 변수가 추가되었습니다:
- `_sys_keyc_homekey`: Control + Home 키
- `_sys_keyc_endkey`: Control + End 키

이 변수들은 `getchar`와 비교할 수 있으며, 키가 눌렸을 때 이러한 값을 반환합니다.

## 예제

```cpp
// 문자가 입력되었는지 확인합니다.
string c = _sys.getchar();
println(c.bytes()); // 내용을 표시합니다.

// 사용자가 위쪽 화살표를 누른 경우, 모든 가능한 색상을 표시합니다.
if (c == _sys_keyup) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 99; j++) {
            for (int k = 0; k < 99; k++) {
                _sys.colors(i, j, k);
                println("test:", i, j, k);
            }
        }
    }
}
```

## 마우스 추적

터미널 내에서 마우스를 추적할 수도 있습니다. 다음은 예시입니다: