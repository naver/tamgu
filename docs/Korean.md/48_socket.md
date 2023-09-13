# 소켓

`Socket` 클래스는 클라이언트와 서버 간의 HTML 소켓 상호작용을 처리합니다.

## 메소드

### 클라이언트 측

1. `close()`: 소켓을 닫습니다.
2. `close(clientid)`: 지정된 `clientid`와의 통신을 닫습니다.
3. `connect(string hostname, int port)`: 클라이언트를 특정 호스트의 특정 포트에 연결합니다.
4. `createServer(int port, int nbclients)`: 로컬 호스트에 특정 포트로 서버를 생성합니다.
5. `createServer(string hostname, int port, int nbclients)`: 특정 호스트와 특정 포트에서 서버를 생성합니다.
6. `get()`: 소켓으로부터 한 문자를 가져옵니다.
7. `get(int clientid)`: 지정된 `clientid`와의 소켓으로부터 한 문자를 가져옵니다.
8. `getFrame(string name)`: 지정된 `name`의 프레임 객체 원격 핸들을 반환합니다.
9. `getFunction(string name)`: 지정된 `name`의 함수 원격 핸들을 반환합니다.
10. `getHostname()`: 현재 호스트 이름을 반환합니다. 이 정보를 얻기 위해 소켓을 활성화할 필요는 없습니다.
11. `read()`: 소켓에서 Tamgu 객체를 읽습니다.
12. `read(int clientid)`: 지정된 `clientid`와의 소켓에서 Tamgu 객체를 읽습니다.
13. `receive(int nb)`: 소켓으로부터 `nb`개의 문자를 읽습니다.
14. `receive(int clientid, int nb)`: 지정된 `clientid`와의 소켓으로부터 `nb`개의 문자를 읽습니다.
15. `run(int client, string stopstring)`: 서버를 실행 모드로 설정합니다. 이제 서버는 원격 메소드 호출(RMI) 모드를 수락할 수 있습니다.
16. `send(int clientid, string s)`: 지정된 `clientid`와의 소켓에 간단한 문자열을 작성합니다.
17. `send(string s)`: 소켓에 간단한 문자열을 작성합니다.

### 서버 측

18. `setTimeout(int i)`: 소켓에서 읽기와 쓰기에 대한 시간 제한을 초 단위로 설정합니다. 읽기나 쓰기에서 블로킹을 피하기 위해 이 명령을 사용하세요. -1은 시간 제한을 취소합니다.
19. `wait()`: 서버는 클라이언트의 연결을 기다립니다. 클라이언트 식별자를 반환하며, 클라이언트와 통신하기 위해 사용됩니다.
20. `write(clientid, o1, o2...)`: 지정된 `clientid`와의 소켓에 Tamgu 객체를 작성합니다.
21. `write(o1, o2...)`: 소켓에 Tamgu 객체를 작성합니다.

## 예제: 서버 측

```python
// 서버 측
int clientid;
Socket s; // 소켓 생성
string name = s.getHostname();
println("로컬 서버:", name);
// 소켓 2020에 최대 5개의 연결을 가진 서버 생성
s.createServer(2020, 5);
// 클라이언트 연결을 기다림
while (true) {
  // 최대 5개의 연결 허용
  clientid = s.wait();
  // 클라이언트로부터 메시지 읽기 (더 많은 연결을 처리하기 위해 스레드에서 수행되어야 함)
  string message = s.read(clientid);
  message += "와 반환됨";
  // 클라이언트에게 메시지 작성
  s.write(clientid);
  // 연결 닫기
  s.close(clientid);
}
// 서버 종료
s.close();
```

## 예제: 클라이언트 측

```python
// 클라이언트 측
Socket s; // 소켓 생성
string name = s.getHostname();
println("로컬 서버:", name);
// 소켓 2020에 서버 연결
s.connect(name, 2020);
// 서버에 메시지 작성
string message = "안녕하세요";
s.write(message);
// 서버로부터 메시지 읽기
message = s.read();
println(message);
// 연결 닫기
s.close();
```