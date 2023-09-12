# Library curl: type curl (WEB) in Tamgu

The `curl` type is used to load HTML pages from the internet. It is based on the cURL library (http://curl.haxx.se/) and offers some basic tools to handle HTML pages.

The name of the library is `tamgucurl`: `use("tamgucurl");`

## 55.1 Methods
1. `execute()`: to execute a curl query. Options should have been provided.
2. `execute(string filename)`: to execute a curl query. Options should have been provided. When a filename is supplied, the output is stored in a file.
3. `options(string option, string|int parameter)`: to supply options to curl before calling `execute` or `url`. See below for a list of all available options.
4. `password(string user, string psswrd)`: to provide a site with a user and a password.
5. `proxy(string proxy)`: to set a proxy connection.
6. `url(string uri)`: to load a URL. This command executes an option `s("CURLOPT_URL", uri)` before executing the command itself.
7. `url(string uri, string filename)`: to load a URL and store the result in a file.

## 55.2 Options
`CURLOPT_ACCEPTTIMEOUT_MS`, `CURLOPT_ACCEPT_ENCODING`, `CURLOPT_ADDRESS_SCOPE`, `CURLOPT_APPEND`, `CURLOPT_AUTOREFERER`, `CURLOPT_BUFFERSIZE`, `CURLOPT_CAINFO`, `CURLOPT_CAPATH`, `CURLOPT_CERTINFO`, `CURLOPT_CHUNK_BGN_FUNCTION`, `CURLOPT_CHUNK_DATA`, `CURLOPT_CHUNK_END_FUNCTION`, `CURLOPT_CLOSESOCKETDATA`, `CURLOPT_CLOSESOCKETFUNCTION`, `CURLOPT_CONNECTTIMEOUT`, `CURLOPT_CONNECTTIMEOUT_MS`, `CURLOPT_CONNECT_ONLY`, `CURLOPT_CONV_FROM_NETWORK_FUNCTION`, `CURLOPT_TLSAUTH_PASSWORD`, `CURLOPT_TLSAUTH_TYPE`, `CURLOPT_TLSAUTH_USERNAME`, `CURLOPT_TRANSFERTEXT`, `CURLOPT_TRANSFER_ENCODING`, `CURLOPT_UNRESTRICTED_AUTH`, `CURLOPT_UPLOAD`, `CURLOPT_URL`, `CURLOPT_USERAGENT`, `CURLOPT_USERNAME`, `CURLOPT_USERPWD`, `CURLOPT_USE_SSL`, `CURLOPT_VERBOSE`, `CURLOPT_WILDCARDMATCH`, `CURLOPT_WRITEDATA`, `CURLOPT_WRITEFUNCTION`, `CURLOPT_UNIX_SOCKET_PATH`, `CURLOPT_XFERINFODATA`, `CURLOPT_XFERINFOFUNCTION`, `CURLOPT_XOAUTH2_BEARER`, `CURLOPT_SSL_ENABLE_ALPN`, `CURLOPT_SSL_ENABLE_NPN`, `CURLOPT_SSL_FALSESTART`, `CURLOPT_SSL_OPTIONS`, `CURLOPT_SASL_IR`, `CURLOPT_SERVICE_NAME`, `CURLOPT_PROXYHEADER`, `CURLOPT_PATH_AS_IS`, `CURLOPT_PINNEDPUBLICKEY`, `CURLOPT_PIPEWAIT`, `CURLOPT_LOGIN_OPTIONS`, `CURLOPT_INTERFACE`, `CURLOPT_HEADEROPT`, `CURLOPT_DNS_INTERFACE`, `CURLOPT_DNS_LOCAL_IP4`, `CURLOPT_DNS_LOCAL_IP6`, `CURLOPT_EXPECT_100_TIMEOUT_MS`, `CURLOPT_MAIL_AUTH`, `CURLOPT_PROXY_SERVICE_NAME`, `CURLOPT_TCP_KEEPALIVE`, `CURLOPT_TCP_KEEPIDLE`, `CURLOPT_TCP_KEEPINTVL`, `CURLOPT_TCP_NODELAY`, `CURLOPT_SSL_VERIFYSTATUS`

Please visit http://curl.haxx.se/ to see the documentation about these options.

## 55.3 Handling Web pages
There are two different ways to load an HTML page: either through a callback function or with a filename.

### Callback
The first possibility is to associate your `url` object with a callback function, whose signature should be the following:

```javascript
function url_callback(string content, myobject o);
```

The function will be associated with the following declaration:

```javascript
url u(o) with url_callback.
```

In that case, you should use `url(string html)` as a method in order to have each block of texts loaded from your web page. For each block, your `url_callback` will be called with the block content as the value.

Example:

```javascript
use("tamgucurl");

function fonc(string content, self o) {
    println(content);
}

curl c with fonc;

// Set a proxy, which will be used as a way to load your web pages through
c.proxy("http://myproxy.mycompany:5050");

// Load our web page. For each block, `func` will be called...
c.url("http://www.liberation.fr/");
```

### File
The other possibility is to provide the `url` method with a filename, which will be used to store the content of your web page. In that case, do not declare your variable with a callback function.

Example:

```javascript
use("tamgucurl");

curl c;

// Set a proxy, which will be used as a way to load your web pages through
c.proxy("http://myproxy.mycompany:5050");

// Load our web page and store it in a file
c.url("http://www.liberation.fr/", "c:\\temp\\myfile.html");
```

Example: