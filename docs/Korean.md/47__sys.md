# Tamgu 시스템 기능

Tamgu는 디렉토리의 내용을 벡터로 읽거나 시스템 명령을 실행하는 등의 시스템 기능을 제공합니다. 다음 메서드에 접근하기 위해 `_sys` 변수를 사용해야 합니다:

## 메서드

1. `command(string s, string outputfile)`: 시스템 명령 `s`를 실행합니다. `outputfile`은 선택적으로 사용되며 명령의 출력(stdout)을 이 파일로 리다이렉트합니다. 만약 `outputfile`이 제공되면, `command`는 또한 이 파일의 내용을 문자열로 반환합니다.

2. `createdirectory(string path)`: 주어진 `path`에 디렉토리를 생성합니다. 디렉토리가 이미 존재하거나 생성할 수 없는 경우 false를 반환합니다.

3. `env(string var)`: 환경 변수 `var`의 값을 반환합니다.

4. `env(string var, string value)`: 환경 변수 `var`의 값을 설정합니다.

5. `listdirectory(string path)`: 디렉토리의 파일들을 벡터로 반환합니다.

6. `ls(string path)`: 디렉토리의 파일들을 벡터로 반환합니다.

7. `mkdir(string path)`: 주어진 `path`에 디렉토리를 생성합니다. 디렉토리가 이미 존재하거나 생성할 수 없는 경우 false를 반환합니다.

8. `fileinfo(string path)`: 주어진 파일에 대한 다음 정보를 가진 맵을 반환합니다:
   - `info["size"]`: 파일의 크기
   - `info["date"]`: 파일의 날짜
   - `info["change"]`: 최근 변경 날짜
   - `info["access"]`: 최근 접근 날짜
   - `info["directory"]`: 경로가 디렉토리인 경우 true
   - `info["pathname"]`: 실제 경로

9. `pipe(string cmd)`: `cmd` 명령을 실행하고 결과로 벡터를 반환합니다.

10. `realpath(string path)`: 상대 경로에 대한 실제 경로를 반환합니다.

11. `getchar()`: 마지막으로 입력된 문자를 반환합니다. `getchar`는 화면에 문자를 표시하지 않습니다.

12. `colors(int style, int code1, int code2, bool disp)`: 텍스트 색상을 설정하고 색상 문자열을 반환합니다.

13. `foregroundcolor(int color)`: 전경 텍스트 색상을 설정합니다.

14. `backgroundcolor(int color)`: 배경 텍스트 색상을 설정합니다.

15. `rgbforegroundcolor(int red, int green, int blue)`: RGB 전경 텍스트 색상을 설정합니다.

16. `rgbbackgroundcolor(int red, int green, int blue)`: RGB 배경 텍스트 색상을 설정합니다.

17. `scrollmargin(int top, int bottom)`: 스크롤 마진 영역을 정의합니다.

18. `deletechar(int nb)`: `nb`개의 문자를 삭제합니다.

19. `up(int nb)`: `nb`줄 위로 이동합니다.

20. `down(int nb)`: `nb`줄 아래로 이동합니다.

21. `right(int nb)`: `nb`개의 문자 오른쪽으로 이동합니다.

22. `left(int nb)`: `nb`개의 문자 왼쪽으로 이동합니다.

23. `next_line(int nb)`: `nb`번 아래로 다음 줄로 이동합니다.

24. `previous_line(int nb)`: `nb`번 위로 이전 줄로 이동합니다.

25. `column(int nb)`: `nb`번째 열로 이동합니다.

26. `row_column(int row, int column)`: 행/열로 이동합니다.

27. `home()`: 커서를 홈 위치로 이동합니다.

28. `cls()`: 화면을 지우고 홈 위치로 돌아갑니다.

29. `hor_vert(int hor, int vert)`: 가로/세로로 이동합니다.

30. `clearscreen(int nb)`: 화면을 지웁니다. `nb`는 부분 또는 전체 화면 지우기에 사용되는 0, 1, 2 또는 3일 수 있습니다.

31. `clear()`: 화면을 지웁니다.

32. `eraseline(int nb)`: `nb`줄을 지웁니다.

33. `scroll_up(int nb)`: `nb`개의 문자를 위로 스크롤합니다.

34. `scroll_down(int nb)`: `nb`개의 문자를 아래로 스크롤합니다.

35. `screensizes()`: 행과 열에 대한 화면 크기를 반환합니다.

36. `hasresized()`: 화면 크기가 변경되었으면 'true'를 반환합니다.

37. `cursor()`: 커서 위치를 반환합니다.

38. `inmouse()`: 마우스 추적을 활성화합니다.

39. `outmouse()`: 마우스 추적을 비활성화합니다.

40. `ismouseaction(string key)`: 마우스 동작인 경우 true를 반환합니다.

41. `mousexy(string key)`: 마우스 위치를 반환합니다.

42. `mousescrollup(string key)`: 위로 스크롤할 때 마우스 위치를 반환합니다.

43. `mousescrolldown(string key)`: 아래로 스크롤할 때 마우스 위치를 반환합니다.

44. `mousedown1(string key)`: 주 클릭할 때 마우스 위치를 반환합니다.

45. `mousedown2(string key)`: 보조 클릭할 때 마우스 위치를 반환합니다.

46. `mouseup(string key)`: 버튼을 놓을 때 마우스 위치를 반환합니다.

47. `mousetrack(string key)`: 마우스가 눌린 상태로 움직일 때 마우스 위치를 반환합니다.

48. `reset()`: 마우스 모드를 재설정하고 초기 termios 값을 되돌립니다. `getchar`가 사용된 경우 Unix 기기에서 사용합니다.

49. `isescapesequence(string key)`: `key`가 이스케이프 시퀀스인 경우 true를 반환합니다.

50. `showcursor(bool show)`: 커서를 표시/숨깁니다.

51. `resizecallback(function f)`: 터미널 창의 크기가 조정될 때 호출되는 콜백 함수를 설정합니다. `f`는 두 개의 매개변수를 가진 함수입니다. 이 매개변수는 새로운 행과 열 크기입니다.

## 예제

```javascript
// 주어진 디렉토리에서 변경 날짜가 주어진 날짜보다 최근인 경우 모든 파일을 다른 디렉토리로 복사하는 함수
function cp(string thepath, string topath) {
    // 소스 디렉토리의 내용을 읽습니다
    vector v = _sys.listdirectory(thepath);
    iterator it;
    string path;
    string cmd;
    map m;
    date t;
    // 오늘 날짜를 9시로 설정합니다
    t.setdate(t.year(), t.month(), t.day(), 9, 0, 0);
    it = v;
    for (it.begin(); it.nend(); it.next()) {
        path = thepath + '/' + it.value();
        // 파일이 올바른 유형인 경우
        if (".cxx" in path || ".h" in path || ".c" in path) {
            m = _sys.fileinfo(path);
            // 날짜가 현재 날짜보다 최근인 경우
            if (m["date"] > t) {
                // 복사합니다
                cmd = "copy " + path + ' ' + topath;
                println(cmd);
                // 명령을 실행합니다
                _sys.command(cmd);
            }
        }
    }
}

// 이 함수를 호출하여 디렉토리에서 다른 디렉토리로 복사합니다
cp('C:\src', 'W:\src');
```

## 변수

이 라이브러리에서는 몇 가지 변수도 제공됩니다:

- `_sys_keyright`: 오른쪽 화살표
- `_sys_keyleft`: 왼쪽 화살표
- `_sys_keydown`: 아래쪽 화살표
- `_sys_keyup`: 위쪽 화살표
- `_sys_keydel`: 삭제 키
- `_sys_keyhomekey`: 홈 키
- `_sys_keyendkey`: 엔드 키
- `_sys_keyc_up`: 컨트롤+위쪽 화살표
- `_sys_keyc_down`: 컨트롤+아래쪽 화살표
- `_sys_keyc_right`: 컨트롤+오른쪽 화살표
- `_sys_keyc_left`: 컨트롤+왼쪽 화살표
- `_sys_keybackspace`: 백스페이스
- `_sys_keyescape`: ESC 키

Windows용으로 다음 변수가 추가되었습니다:

- `_sys_keyc_homekey`: 컨트롤+홈 키
- `_sys_keyc_endkey`: 컨트롤+엔드 키

이 변수들은 `getchar`가 키가 눌릴 때 이러한 값을 반환하므로 이 변수들과 비교할 수 있습니다.

## 예제

```javascript
// 문자가 눌렸는지 확인합니다
string c = _sys.getchar();
println(c.bytes()); // 내용을 표시합니다

// 사용자가 위쪽 화살표를 누르면 모든 가능한 색상을 표시합니다
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

터미널 내에서 마우스를 추적할 수도 있습니다. 다음은 예제입니다:

```javascript
// 마우스 추적을 초기화합니다
_sys.inmouse();
string u;
u = _sys.getchar();
ivector iv;
while (u.ord() != 17) {
    if (_sys.ismouseaction(u)) {
        iv = _sys.mousedown1(u);
        if (iv != [])
            println("Mouse button 1 down at position:", iv);
        iv = _sys.mousexy(u);
        if (iv != [])
            println("Mouse is at position:", iv);
        iv = _sys.mousescrollup(u);
        if (iv != [])
            println("Mouse is scrolling up at position:", iv);
        iv = _sys.mousescrolldown(u);
        if (iv != [])
            println("Mouse is scrolling down at position:", iv);
    }
    u = _sys.getchar();
}
// 마우스 추적을 비활성화하고 터미널을 재설정합니다
_sys.reset();
```