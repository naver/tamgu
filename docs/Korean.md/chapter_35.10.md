## ioredirect과 iorestate

이 두 가지 함수는 stderr 또는 stdout의 출력을 캡처하여 파일에 저장하는 데 사용됩니다.

### ioredirect

```cpp
int ioredirect(string filename, bool err);
```

이 함수는 `filename`으로 stderr(만약 `err`이 true인 경우) 또는 stdout(만약 `err`이 false인 경우)를 리디렉션합니다. 출력을 원래대로 돌리기 위해 사용될 ID를 반환합니다.

### iorestate

```cpp
void iorestate(int id, bool err);
```

이 함수는 출력을 원래대로 돌립니다. 첫 번째 매개변수는 `ioredirect`에서 반환된 ID입니다. 파일은 그 후에 닫힙니다.

### 예시

```cpp
int id = ioredirect("C:\\Tamgu\\test\\test.txt", true);
printlnerr("This string is now stored in file: test.txt");
iorestate(id, true); // 원래대로 돌아감
```

위의 예시에서 `printlnerr`의 출력은 "test.txt" 파일로 리디렉션되고, `iorestate`를 사용하여 원래대로 복원됩니다.