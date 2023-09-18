# Bibliothèque curl

La bibliothèque `curl` est utilisée pour charger des pages HTML depuis internet. Elle est basée sur la bibliothèque cURL (http://curl.haxx.se/) et offre quelques outils de base pour manipuler des pages HTML.

Le nom de la bibliothèque est `tamgucurl`. Pour l'utiliser, importez-la avec la commande suivante : `use("tamgucurl")`.

## Méthodes

1. `execute()`: Exécute une requête curl. Les options doivent avoir été fournies.
2. `execute(string filename)`: Exécute une requête curl. Les options doivent avoir été fournies. Lorsqu'un nom de fichier est fourni, la sortie est stockée dans un fichier.
3. `options(string option, string|int parameter)`: Fournit des options à curl avant d'appeler execute ou url. Voir ci-dessous pour une liste de toutes les options disponibles.
4. `password(string user, string psswrd)`: Fournit un nom d'utilisateur et un mot de passe à un site.
5. `proxy(string proxy)`: Définit une connexion proxy.
6. `url(string uri)`: Charge une URL. Cette commande exécute l'option `s("CURLOPT_URL", uri)` avant d'exécuter la commande elle-même.
7. `url(string uri, string filename)`: Charge une URL et stocke le résultat dans un fichier.

## Options

Voici une liste de toutes les options disponibles pour curl :

CURLOPT_ACCEPTTIMEOUT_MS, CURLOPT_ACCEPT_ENCODING, CURLOPT_ADDRESS_SCOPE, CURLOPT_APPEND, CURLOPT_AUTOREFERER, CURLOPT_BUFFERSIZE, CURLOPT_CAINFO, CURLOPT_CAPATH, CURLOPT_CERTINFO, CURLOPT_CHUNK_BGN_FUNCTION, CURLOPT_CHUNK_DATA, CURLOPT_CHUNK_END_FUNCTION, CURLOPT_CLOSESOCKETDATA, CURLOPT_CLOSESOCKETFUNCTION, CURLOPT_CONNECTTIMEOUT, CURLOPT_CONNECTTIMEOUT_MS, CURLOPT_CONNECT_ONLY, CURLOPT_CONV_FROM_NETWORK_FUNCTION, CURLOPT_CONV_FROM_UTF8_FUNCTION, CURLOPT_CONV_TO_NETWORK_FUNCTION, CURLOPT_COOKIE, CURLOPT_COOKIEFILE, CURLOPT_COOKIEJAR, CURLOPT_COOKIELIST, CURLOPT_COOKIESESSION, CURLOPT_COPYPOSTFIELDS, CURLOPT_CRLF, CURLOPT_CRLFILE, CURLOPT_CUSTOMREQUEST, CURLOPT_DEBUGDATA, CURLOPT_DEBUGFUNCTION, CURLOPT_DIRLISTONLY, CURLOPT_DNS_CACHE_TIMEOUT, CURLOPT_DNS_SERVERS, CURLOPT_DNS_USE_GLOBAL_CACHE, CURLOPT_EGDSOCKET, CURLOPT_ERRORBUFFER, CURLOPT_FAILONERROR, CURLOPT_FILETIME, CURLOPT_FNMATCH_DATA, CURLOPT_FNMATCH_FUNCTION, CURLOPT_FOLLOWLOCATION, CURLOPT_FORBID_REUSE, CURLOPT_FRESH_CONNECT, CURLOPT_FTPPORT, CURLOPT_FTPSSLAUTH, CURLOPT_FTP_ACCOUNT, CURLOPT_FTP_ALTERNATIVE_TO_USER, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLOPT_FTP_FILEMETHOD, CURLOPT_FTP_RESPONSE_TIMEOUT, CURLOPT_FTP_SKIP_PASV_IP, CURLOPT_FTP_SSL_CCC, CURLOPT_FTP_USE_EPRT, CURLOPT_FTP_USE_EPSV, CURLOPT_FTP_USE_PRET, CURLOPT_GSSAPI_DELEGATION, CURLOPT_HEADER, CURLOPT_HEADERDATA, CURLOPT_HEADERFUNCTION, CURLOPT_HTTP200ALIASES, CURLOPT_HTTPAUTH, CURLOPT_HTTPGET, CURLOPT_HTTPHEADER, CURLOPT_HTTPPOST, CURLOPT_HTTPPROXYTUNNEL, CURLOPT_HTTP_CONTENT_DECODING, CURLOPT_HTTP_TRANSFER_DECODING, CURLOPT_HTTP_VERSION, CURLOPT_IGNORE_CONTENT_LENGTH, CURLOPT_INFILESIZE, CURLOPT_INFILESIZE_LARGE, CURLOPT_INTERLEAVEDATA, CURLOPT_INTERLEAVEFUNCTION, CURLOPT_IOCTLDATA, CURLOPT_IOCTLFUNCTION, CURLOPT_IPRESOLVE, CURLOPT_ISSUERCERT, CURLOPT_KEYPASSWD, CURLOPT_KRBLEVEL, CURLOPT_LOCALPORT, CURLOPT_LOCALPORTRANGE, CURLOPT_LOW_SPEED_LIMIT, CURLOPT_LOW_SPEED_TIME, CURLOPT_MAIL_FROM, CURLOPT_MAIL_RCPT, CURLOPT_MAXCONNECTS, CURLOPT_MAXFILESIZE, CURLOPT_MAXFILESIZE_LARGE, CURLOPT_MAXREDIRS, CURLOPT_MAX_RECV_SPEED_LARGE, CURLOPT_MAX_SEND_SPEED_LARGE, CURLOPT_NETRC, CURLOPT_NETRC_FILE, CURLOPT_NEW_DIRECTORY_PERMS, CURLOPT_NEW_FILE_PERMS, CURLOPT_NOBODY, CURLOPT_NOPROGRESS, CURLOPT_NOPROXY, CURLOPT_NOSIGNAL, CURLOPT_OPENSOCKETDATA, CURLOPT_OPENSOCKETFUNCTION, CURLOPT_PASSWORD, CURLOPT_PORT, CURLOPT_POST, CURLOPT_POSTFIELDS, CURLOPT_POSTFIELDSIZE, CURLOPT_POSTFIELDSIZE_LARGE, CURLOPT_POSTQUOTE, CURLOPT_POSTREDIR, CURLOPT_PREQUOTE, CURLOPT_PRIVATE, CURLOPT_PROGRESSDATA, CURLOPT_PROGRESSFUNCTION, CURLOPT_PROTOCOLS, CURLOPT_PROXY, CURLOPT_PROXYAUTH, CURLOPT_PROXYPASSWORD, CURLOPT_PROXYPORT, CURLOPT_PROXYTYPE, CURLOPT_PROXYUSERNAME, CURLOPT_PROXYUSERPWD, CURLOPT_PROXY_TRANSFER_MODE, CURLOPT_PUT, CURLOPT_QUOTE, CURLOPT_RANDOM_FILE, CURLOPT_RANGE, CURLOPT_READDATA, CURLOPT_READFUNCTION, CURLOPT_REDIR_PROTOCOLS, CURLOPT_REFERER, CURLOPT_RESOLVE, CURLOPT_RESUME_FROM, CURLOPT_RESUME_FROM_LARGE, CURLOPT_RTSP_CLIENT_CSEQ, CURLOPT_RTSP_REQUEST, CURLOPT_RTSP_SERVER_CSEQ, CURLOPT_RTSP_SESSION_ID, CURLOPT_RTSP_STREAM_URI, CURLOPT_RTSP_TRANSPORT, CURLOPT_SEEKDATA, CURLOPT_SEEKFUNCTION, CURLOPT_SHARE, CURLOPT_SOCKOPTDATA, CURLOPT_SOCKOPTFUNCTION, CURLOPT_SOCKS5_GSSAPI_NEC, CURLOPT_SOCKS5_GSSAPI_SERVICE, CURLOPT_SSH_AUTH_TYPES, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, CURLOPT_SSH_KEYDATA, CURLOPT_SSH_KEYFUNCTION, CURLOPT_SSH_KNOWNHOSTS, CURLOPT_SSH_PRIVATE_KEYFILE, CURLOPT_SSH_PUBLIC_KEYFILE, CURLOPT_SSLCERT, CURLOPT_SSLCERTTYPE, CURLOPT_SSLENGINE, CURLOPT_SSLENGINE_DEFAULT, CURLOPT_SSLKEY, CURLOPT_SSLKEYTYPE, CURLOPT_SSLVERSION, CURLOPT_SSL_CIPHER_LIST, CURLOPT_SSL_CTX_DATA, CURLOPT_SSL_CTX_FUNCTION, CURLOPT_SSL_SESSIONID_CACHE, CURLOPT_SSL_VERIFYHOST, CURLOPT_SSL_VERIFYPEER, CURLOPT_STDERR, CURLOPT_TELNETOPTIONS, CURLOPT_TFTP_BLKSIZE, CURLOPT_TIMECONDITION, CURLOPT_TIMEOUT, CURLOPT_TIMEOUT_MS, CURLOPT_TIMEVALUE, CURLOPT_TLSAUTH_PASSWORD, CURLOPT_TLSAUTH_TYPE, CURLOPT_TLSAUTH_USERNAME, CURLOPT_TRANSFERTEXT, CURLOPT_TRANSFER_ENCODING, CURLOPT_UNRESTRICTED_AUTH, CURLOPT_UPLOAD, CURLOPT_URL, CURLOPT_USERAGENT, CURLOPT_USERNAME, CURLOPT_USERPWD, CURLOPT_USE_SSL, CURLOPT_VERBOSE, CURLOPT_WILDCARDMATCH, CURLOPT_WRITEDATA, CURLOPT_WRITEFUNCTION, CURLOPT_UNIX_SOCKET_PATH, CURLOPT_XFERINFODATA, CURLOPT_XFERINFOFUNCTION, CURLOPT_XOAUTH2_BEARER, CURLOPT_SSL_ENABLE_ALPN, CURLOPT_SSL_ENABLE_NPN, CURLOPT_SSL_FALSESTART, CURLOPT_SSL_OPTIONS, CURLOPT_SASL_IR, CURLOPT_SERVICE_NAME, CURLOPT_PROXYHEADER, CURLOPT_PATH_AS_IS, CURLOPT_PINNEDPUBLICKEY, CURLOPT_PIPEWAIT, CURLOPT_LOGIN_OPTIONS, CURLOPT_INTERFACE, CURLOPT_HEADEROPT, CURLOPT_DNS_INTERFACE, CURLOPT_DNS_LOCAL_IP4, CURLOPT_DNS_LOCAL_IP6, CURLOPT_EXPECT_100_TIMEOUT_MS, CURLOPT_MAIL_AUTH, CURLOPT_PROXY_SERVICE_NAME, CURLOPT_TCP_KEEPALIVE, CURLOPT_TCP_KEEPIDLE, CURLOPT_TCP_KEEPINTVL, CURLOPT_TCP_NODELAY, CURLOPT_SSL_VERIFYSTATUS

Pour plus d'informations sur ces options, veuillez visiter : http://curl.haxx.se/

## Manipulation des pages Web

Il existe deux façons différentes de charger une page HTML : soit via une fonction de rappel, soit avec un nom de fichier.

### Fonction de rappel

La première possibilité est d'associer votre objet `url` à une fonction de rappel, dont la signature devrait être la suivante :

```
function url_callback(string content, myobject o);
```

La fonction sera associée à la déclaration suivante :

```
url u(o) with url_callback.
```

Dans ce cas, vous devez utiliser la méthode `url(string html)` afin de charger chaque bloc de texte depuis votre page web. Pour chaque bloc, votre fonction `url_callback` sera appelée avec le contenu du bloc en tant que valeur.

Exemple :

```python
use("tamgucurl");

function fonc(string content, self o) {
    println(content);
}

curl c with fonc;

// Définir un proxy, qui sera utilisé comme moyen de charger vos pages web
c.proxy("http://myproxy.mycompany:5050");

// Charger notre page web. Pour chaque bloc, fonc sera appelée...
c.url("http://www.liberation.fr/");
```

### Fichier

L'autre possibilité est de fournir à la méthode `url` un nom de fichier, qui sera utilisé pour stocker le contenu de votre page web. Dans ce cas, ne déclarez pas votre variable avec une fonction de rappel.

Exemple :

```python
use("tamgucurl");

curl c;

// Définir un proxy, qui sera utilisé comme moyen de charger vos pages web
c.proxy("http://myproxy.mycompany:5050");

// Charger notre page web et stocker le résultat dans un fichier
c.url("http://www.liberation.fr/", "c:\temp\myfile.html");
```

### Exemple

Cet exemple montre comment interroger un site de recherche (l'URL fournie ici n'existe pas au moment où ce manuel a été rédigé) :

```python
string mytxt;

function requester(string s, self e) {
    mytxt += s;
}

curl querying with requester;

// Définir un proxy
querying.proxy("my.proxy.com:8080");

// Définir quelques options, qui sont nécessaires pour procéder à notre commande
querying.options("CURLOPT_HEADER", 0);
querying.options("CURLOPT_VERBOSE", 0);
querying.options("CURLOPT_AUTOREFERER", 1);
querying.options("CURLOPT_FOLLOWLOCATION", 1);
querying.options("CURLOPT_COOKIEFILE", "");
querying.options("CURLOPT_COOKIEJAR", "");
querying.options("CURLOPT_USERAGENT", "Mozilla/4.0 (compatible;)");

function request(svector words) {
    // Construire notre requête
    string query = "http://my.any.search.engine.com/html/?q=";
    mytxt = "";
    string thequery = query + words.join("+");
    querying.url(thequery);
    println(mytxt);
}

request(["test", "word"]);
```