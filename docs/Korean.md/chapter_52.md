# 라이브러리 사운드: 탐구에서의 사운드 조작

탐구는 다양한 종류의 사운드 파일(WAV, MP3, FLAC, OGG 등)을 사용하기 편리하게 제공합니다. 파일을 로드하여 "sound" 타입을 사용하여 코드 어디에서든 쉽게 재생할 수 있습니다.

"sound" 라이브러리를 사용하기 전에 libao4, libsndfile-1 및 libmpg123 라이브러리가 올바르게 설치되어 있는지 확인하십시오.

## 라이브러리 호출

"sound" 라이브러리를 사용하려면 코드에서 간단히 `use("sound")`를 호출하면 됩니다.

## 메소드

다음과 같은 메소드를 사용할 수 있습니다.

1. `close()`: 사운드 채널을 닫습니다.
2. `decode(ivector soundbuffer)`: 사운드 파일을 디코딩하고 `soundbuffer`에 버퍼 단위로 내용을 반환합니다. 파일의 끝에 도달하면 false를 반환합니다.
3. `encode(ivector soundbuffer)`: `decode`에 의해 반환된 사운드 버퍼를 재생합니다.
4. `load(string pathname)`: 지정된 `pathname`의 사운드 파일을 로드합니다.
5. `open(map params)`: 현재 사운드 파일의 매개변수를 사용하여 사운드 채널을 엽니다(매개변수 참조).
6. `parameters()`: 현재 사운드 파일의 매개변수를 맵 형태로 반환합니다.
7. `parameters(map modifs)`: 현재 사운드 파일의 "rate" 및 "channels" 매개변수를 수정합니다.
8. `play()`: 사운드를 재생합니다.
9. `play(bool beg)`: `beg`가 true인 경우 사운드를 처음부터 재생합니다.
10. `play(ivector soundbuffer)`: 사운드 버퍼를 재생합니다(`encode` 참조).
11. `reset()`: 사운드 파일을 처음부터 재설정합니다.