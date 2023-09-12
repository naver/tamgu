# 라이브러리 curl: 탐구에서 curl (WEB) 타입 사용하기

`curl` 타입은 인터넷에서 HTML 페이지를 로드하는 데 사용됩니다. 이는 cURL 라이브러리(http://curl.haxx.se/)를 기반으로 하며 HTML 페이지를 처리하는 몇 가지 기본 도구를 제공합니다.

라이브러리의 이름은 `tamgucurl` 입니다: `use("tamgucurl");`

## 55.1 메소드
1. `execute()`: curl 쿼리를 실행합니다. 옵션은 제공되어야 합니다.
2. `execute(string filename)`: curl 쿼리를 실행합니다. 옵션은 제공되어야 합니다. 파일 이름이 제공되면 결과가 파일에 저장됩니다.
3. `options(string option, string|int parameter)`: `execute` 또는 `url`을 호출하기 전에 curl에 옵션을 제공합니다. 사용 가능한 모든 옵션 목록은 아래에서 확인하세요.
4. `password(string user, string psswrd)`: 사용자 이름과 비밀번호를 제공하여 사이트에 인증합니다.
5. `proxy(string proxy)`: 프록시 연결을 설정합니다.
6. `url(string uri)`: URL을 로드합니다. 이 명령은 명령 자체를 실행하기 전에 옵션 `s("CURLOPT_URL", uri)`를 실행합니다.
7. `url(string uri, string filename)`: URL을 로드하고 결과를 파일에 저장합니다.

## 55.2 옵션
`CURLOPT_ACCEPTTIMEOUT_MS`, `CURLOPT_ACCEPT_ENCODING`, `CURLOPT_ADDRESS_SCOPE`, `CURLOPT_APPEND`, `CURLOPT_AUTOREFERER`, `CURLOPT_BUFFERSIZE`, `CURLOPT_CAINFO`, `CURLOPT_CAPATH`, `CURLOPT_CERTINFO`, `CURLOPT_CHUNK_BGN_FUNCTION`, `CURLOPT_CHUNK_DATA`, `CURLOPT_CHUNK_END_FUNCTION`, `CURLOPT_CLOSESOCKETDATA`, `CURLOPT_CLOSESOCKETFUNCTION`, `CURLOPT_CONNECTTIMEOUT`, `CURLOPT_CONNECTTIMEOUT_MS`, `CURLOPT_CONNECT_ONLY`, `CURLOPT_CONV_FROM_NETWORK_FUNCTION`, `CURLOPT_TLSAUTH_PASSWORD`, `CURLOPT_TLSAUTH_TYPE`, `CURLOPT_TLSAUTH_USERNAME`, `CURLOPT_TRANSFERTEXT`, `CURLOPT_TRANSFER_ENCODING`, `CURLOPT_UNRESTRICTED_AUTH`, `CURLOPT_UPLOAD`, `CURLOPT_URL`, `CURLOPT_USERAGENT`, `CURLOPT_USERNAME`, `CURLOPT_USERPWD`, `CURLOPT_USE_SSL`, `CURLOPT_VERBOSE`, `CURLOPT_WILDCARDMATCH`, `CURLOPT_WRITEDATA`, `CURLOPT_WRITEFUNCTION`, `CURLOPT_UNIX_SOCKET_PATH`, `CURLOPT_XFERINFODATA`, `CURLOPT_XFERINFOFUNCTION`, `CURLOPT_XOAUTH2_BEARER`, `CURLOPT_SSL_ENABLE_ALPN`, `CURLOPT_SSL_ENABLE_NPN`, `CURLOPT_SSL_FALSESTART`, `CURLOPT_SSL_OPTIONS`, `CURLOPT_SASL_IR`, `CURLOPT_SERVICE_NAME`, `CURLOPT_PROXYHEADER`, `CURLOPT_PATH_AS_IS`, `CURLOPT_PINNEDPUBLICKEY`, `CURLOPT_PIPEWAIT`, `CURLOPT_LOGIN_OPTIONS`, `CURLOPT_INTERFACE`, `CURLOPT_HEADEROPT`, `CURLOPT_DNS_INTERFACE`, `CURLOPT_DNS_LOCAL_IP4`, `CURLOPT_DNS_LOCAL_IP6`, `CURLOPT_EXPECT_100_TIMEOUT_MS`, `CURLOPT_MAIL_AUTH`, `CURLOPT_PROXY_SERVICE_NAME`, `CURLOPT_TCP_KEEPALIVE`, `CURLOPT_TCP_KEEPIDLE`, `CURLOPT_TCP_KEEPINTVL`, `CURLOPT_TCP_NODELAY`, `CURLOPT_SSL_VERIFYSTATUS`

이 옵션에 대한 자세한 내용은 http://curl.haxx.se/ 에서 문서를 확인하세요.

## 55.3 웹 페이지 처리
HTML 페이지를 로드하는 두 가지 다른 방법이 있습니다: 콜백 함수 또는 파일을 사용하는 방법입니다.

### 콜백 함수
첫 번째 방법은 `url` 객체를 콜백 함수와 연결하는 것입니다. 콜백 함수의 시그니처는 다음과 같아야 합니다:

```javascript
function url_callback(string content, myobject o);
```

함수는 다음과 같이 선언됩니다:

```javascript
url u(o) with url_callback.
```

이 경우, 각 텍스트 블록이 웹 페이지에서 로드될 때마다 `url_callback` 함수가 해당 블록의 내용으로 호출됩니다.

예제:

```javascript
use("tamgucurl");

function fonc(string content, self o) {
    println(content);
}

curl c with fonc;

// 웹 페이지를 로드하기 위한 방법으로 사용될 프록시 설정
c.proxy("http://myproxy.mycompany:5050");

// 웹 페이지 로드. 각 블록에 대해 `func`가 호출됩니다...
c.url("http://www.liberation.fr/");
```

### 파일
다른 방법은 `url` 메소드에 파일 이름을 제공하여 웹 페이지의 내용을 저장하는 것입니다. 이 경우, 콜백 함수가 있는 변수를 선언하지 마세요.

예제:

```javascript
use("tamgucurl");

curl c;

// 웹 페이지를 로드하기 위한 방법으로 사용될 프록시 설정
c.proxy("http://myproxy.mycompany:5050");

// 웹 페이지를 로드하고 파일에 저장
c.url("http://www.liberation.fr/", "c:\\temp\\myfile.html");
```

예제: