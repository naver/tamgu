# 라이브러리 사운드

Tamgu의 라이브러리 사운드는 WAV, MP3, FLAC, OGG 등과 같은 다양한 유형의 사운드 파일을 재생하는 방법을 제공합니다. "사운드" 타입을 사용하여 파일을 로드하고 코드 어디에서든 재생할 수 있습니다.

**참고:** Tamgu는 디코딩과 재생을 위해 다음 라이브러리를 사용합니다: libao4, libsndfile-1, libmpg123.

이 라이브러리를 사용하려면 `use("sound")`를 호출하면 됩니다.

## 메소드

API는 다음 메소드를 제공합니다:

1. `close()`: 사운드 채널을 닫습니다.
2. `decode(ivector soundbuffer)`: 사운드 파일을 디코딩하고 `soundbuffer`로 버퍼별로 내용을 반환합니다. 파일의 끝에 도달하면 false를 반환합니다.
3. `encode(ivector soundbuffer)`: `decode`에 의해 반환된 사운드 버퍼를 재생합니다.
4. `load(string pathname)`: 지정된 `pathname`에서 사운드를 로드합니다.
5. `open(map params)`: 현재 사운드 파일의 매개변수로 사운드 채널을 엽니다 (매개변수 참조).
6. `parameters()`: 현재 사운드 파일의 매개변수를 맵 형태로 반환합니다.
7. `parameters(map modifs)`: "rate"와 "channels"만 수정할 수 있습니다.
8. `play()`: 사운드를 재생합니다.
9. `play(bool beg)`: 사운드를 처음부터 재생합니다.
10. `play(ivector soundbuffer)`: 사운드 버퍼를 재생합니다 (`encode` 참조).
11. `reset()`: 사운드 파일을 처음으로 재설정합니다.
12. `stop()`: 사운드를 정지합니다. 사운드 파일을 스레드로 재생해야 이 명령을 사용할 수 있습니다.

## 예제

```cpp
use("sound");
sound s;
s.load("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
s.play();
```

다음 선언으로도 사운드를 로드할 수 있습니다:

```cpp
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");
```

## 디코딩 예제

```cpp
use("sound");

// 사운드 파일 열기
sound s("C:\TAMGU\TAMGU7\sound\Kalimba.mp3");

// 두 번째 사운드 채널 열기
sound c;

// 사운드 매개변수 가져오기
map params = s.parameters();

// 사운드 매개변수를 사용하여 채널 열기
c.open(params);

// 사운드 파일에서 디코딩하고
// 각 새로운 버퍼마다 사운드 재생
// "encode" 대신에 "play"를 사용할 수도 있지만
// 조금 더 느립니다
ivector snd;
while (s.decode(snd))
    c.encode(snd);

// 채널 닫기
c.close();
```