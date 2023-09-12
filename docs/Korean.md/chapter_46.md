# 탐구에서의 소켓

`socket` 타입은 클라이언트와 서버 간의 HTML 소켓 상호작용을 처리합니다.

## 1. 메소드

### 클라이언트 측

1. `close()`: 소켓을 닫습니다.
2. `close(clientid)`: `clientid`와의 통신을 닫습니다.
3. `connect(string hostname, int port)`: 클라이언트를 특정 호스트와 포트에 연결합니다.
4. `createserver(int port, int nbclients)`: 지정된 포트에서 로컬 호스트에 서버를 생성합니다.
5. `createserver(string hostname, int port, int nbclients)`: 특정 호스트와 포트에서 서버를 생성합니다.
6. `get()`: 소켓에서 한 문자를 가져옵니다.
7. `get(int clientid)`: `clientid`와의 소켓에서 한 문자를 가져옵니다.
8. `getframe(string name)`: 이름이 `name`인 프레임 객체의 원격 핸들을 반환합니다.
9. `getfunction(string name)`: 이름이 `name`인 함수의 원격 핸들을 반환합니다.
10. `gethostname()`: 현재 호스트 이름을 반환합니다. 이 정보를 얻기 위해 소켓을 활성화할 필요는 없습니다.
11. `read()`: 소켓에서 Tamgu 객체를 읽습니다.
12. `read(clientid)`: `clientid`와의 소켓에서 Tamgu 객체를 읽습니다.
13. `receive(int nb)`: 소켓에서 `nb`개의 문자를 읽습니다.
14. `receive(int clientid, int nb)`: `clientid`와의 소켓에서 `nb`개의 문자를 읽습니다.
15. `run(int client, string stopstring)`: 서버를 실행 모드로 설정합니다. 이제 서버는 원격 메소드 호출(RMI) 모드를 수락할 수 있습니다.
16. `send(int clientid, string s)`: `clientid`와의 소켓에 간단한 문자열을 씁니다.
17. `send(string s)`: 소켓에 간단한 문자열을 씁니다.

### 서버 측

18. `settimeout(int i)`: 소켓에서의 쓰기와 읽기에 대한 타임아웃을 초 단위로 설정합니다. 읽기나 쓰기에서 블로킹을 피하기 위해 이 명령을 사용하세요. -1의 값은 타임아웃을 취소합니다.
19. `wait()`: 서버는 클라이언트의 연결을 기다립니다. 클라이언트와 통신하기 위해 클라이언트 식별자를 반환합니다.
20. `write(clientid, o1, o2...)`: `clientid`와의 소켓에 Tamgu 객체를 씁니다.
21. `write(o1, o2...)`: 소켓에 Tamgu 객체를 씁니다.

## 예제: 서버 측

```cpp
// 서버 측
int clientid;
socket s; // 소켓을 생성합니다.
string name = s.gethostname();
println("로컬 서버:", name);
// 최대 5개의 연결을 가진 소켓 2020에서 서버를 생성합니다...
s.createserver(2020, 5);
// 클라이언트 연결을 기다립니다.
while (true) {
    // 최대 5개의 연결을 수락할 수 있습니다...
    clientid = s.wait();
    // 클라이언트로부터 메시지를 읽습니다. 더 많은 연결을 처리하기 위해 스레드에서 수행되어야 합니다.
    string message = s.read(clientid);
    message += "와 반환";
    // 클라이언트에게 메시지를 씁니다.
    s.write(clientid);
    // 연결을 닫습니다.
    s.close(clientid);
}
// 서버를 종료합니다.
s.close();
```

## 예제: 클라이언트 측

```cpp
// 클라이언트 측
socket s; // 소켓을 생성합니다.
string name = s.gethostname();
println("로컬 서버:", name);
// 소켓 2020에서 서버를 생성합니다.
```