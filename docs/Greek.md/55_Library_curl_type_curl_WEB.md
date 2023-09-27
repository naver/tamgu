# Βιβλιοθήκη curl

Η βιβλιοθήκη `curl` χρησιμοποιείται για τη φόρτωση σελίδων HTML από το διαδίκτυο. Βασίζεται στη βιβλιοθήκη cURL (http://curl.haxx.se/) και προσφέρει μερικά βασικά εργαλεία για την χειρισμό των σελίδων HTML.

Το όνομα της βιβλιοθήκης είναι `tamgucurl`. Για να τη χρησιμοποιήσετε, εισάγετε την χρησιμοποιώντας την ακόλουθη εντολή: `use("tamgucurl")`.

## Μέθοδοι

1. `execute()`: Εκτελεί μια αίτηση curl. Οι επιλογές θα πρέπει να έχουν παρασχεθεί.
2. `execute(string filename)`: Εκτελεί μια αίτηση curl. Οι επιλογές θα πρέπει να έχουν παρασχεθεί. Όταν δίνεται ένα όνομα αρχείου, το αποτέλεσμα αποθηκεύεται σε ένα αρχείο.
3. `options(string option, string|int parameter)`: Παρέχει επιλογές στο curl πριν από την κλήση της execute ή της url. Δείτε παρακάτω για μια λίστα όλων των διαθέσιμων επιλογών.
4. `password(string user, string psswrd)`: Παρέχει έναν χρήστη και έναν κωδικό πρόσβασης σε έναν ιστότοπο.
5. `proxy(string proxy)`: Ορίζει μια σύνδεση με έναν διαμεσολαβητή (proxy).
6. `url(string uri)`: Φορτώνει μια διεύθυνση URL. Αυτή η εντολή εκτελεί μια επιλογή `s("CURLOPT_URL", uri)` πριν από την εκτέλεση της εντολής αυτής.
7. `url(string uri, string filename)`: Φορτώνει μια διεύθυνση URL και αποθηκεύει το αποτέλεσμα σε ένα αρχείο.

## Επιλογές

Παρακάτω παρατίθεται μια λίστα με όλες τις διαθέσιμες επιλογές για το curl:

CURLOPT_ACCEPTTIMEOUT_MS, CURLOPT_ACCEPT_ENCODING, CURLOPT_ADDRESS_SCOPE, CURLOPT_APPEND, CURLOPT_AUTOREFERER, CURLOPT_BUFFERSIZE, CURLOPT_CAINFO, CURLOPT_CAPATH, CURLOPT_CERTINFO, CURLOPT_CHUNK_BGN_FUNCTION, CURLOPT_CHUNK_DATA, CURLOPT_CHUNK_END_FUNCTION, CURLOPT_CLOSESOCKETDATA, CURLOPT_CLOSESOCKETFUNCTION, CURLOPT_CONNECTTIMEOUT, CURLOPT_CONNECTTIMEOUT_MS, CURLOPT_CONNECT_ONLY, CURLOPT_CONV_FROM_NETWORK_FUNCTION, CURLOPT_CONV_FROM_UTF8_FUNCTION, CURLOPT_CONV_TO_NETWORK_FUNCTION, CURLOPT_COOKIE, CURLOPT_COOKIEFILE, CURLOPT_COOKIEJAR, CURLOPT_COOKIELIST, CURLOPT_COOKIESESSION, CURLOPT_COPYPOSTFIELDS, CURLOPT_CRLF, CURLOPT_CRLFILE, CURLOPT_CUSTOMREQUEST, CURLOPT_DEBUGDATA, CURLOPT_DEBUGFUNCTION, CURLOPT_DIRLISTONLY, CURLOPT_DNS_CACHE_TIMEOUT, CURLOPT_DNS_SERVERS, CURLOPT_DNS_USE_GLOBAL_CACHE, CURLOPT_EGDSOCKET, CURLOPT_ERRORBUFFER, CURLOPT_FAILONERROR, CURLOPT_FILETIME, CURLOPT_FNMATCH_DATA, CURLOPT_FNMATCH_FUNCTION, CURLOPT_FOLLOWLOCATION, CURLOPT_FORBID_REUSE, CURLOPT_FRESH_CONNECT, CURLOPT_FTPPORT, CURLOPT_FTPSSLAUTH, CURLOPT_FTP_ACCOUNT, CURLOPT_FTP_ALTERNATIVE_TO_USER, CURLOPT_FTP_CREATE_MISSING_DIRS, CURLOPT_FTP_FILEMETHOD, CURLOPT_FTP_RESPONSE_TIMEOUT, CURLOPT_FTP_SKIP_PASV_IP, CURLOPT_FTP_SSL_CCC, CURLOPT_FTP_USE_EPRT, CURLOPT_FTP_USE_EPSV, CURLOPT_FTP_USE_PRET, CURLOPT_GSSAPI_DELEGATION, CURLOPT_HEADER, CURLOPT_HEADERDATA, CURLOPT_HEADERFUNCTION, CURLOPT_HTTP200ALIASES, CURLOPT_HTTPAUTH, CURLOPT_HTTPGET, CURLOPT_HTTPHEADER, CURLOPT_HTTPPOST, CURLOPT_HTTPPROXYTUNNEL, CURLOPT_HTTP_CONTENT_DECODING, CURLOPT_HTTP_TRANSFER_DECODING, CURLOPT_HTTP_VERSION, CURLOPT_IGNORE_CONTENT_LENGTH, CURLOPT_INFILESIZE, CURLOPT_INFILESIZE_LARGE, CURLOPT_INTERLEAVEDATA, CURLOPT_INTERLEAVEFUNCTION, CURLOPT_IOCTLDATA, CURLOPT_IOCTLFUNCTION, CURLOPT_IPRESOLVE, CURLOPT_ISSUERCERT, CURLOPT_KEYPASSWD, CURLOPT_KRBLEVEL, CURLOPT_LOCALPORT, CURLOPT_LOCALPORTRANGE, CURLOPT_LOW_SPEED_LIMIT, CURLOPT_LOW_SPEED_TIME, CURLOPT_MAIL_FROM, CURLOPT_MAIL_RCPT, CURLOPT_MAXCONNECTS, CURLOPT_MAXFILESIZE, CURLOPT_MAXFILESIZE_LARGE, CURLOPT_MAXREDIRS, CURLOPT_MAX_RECV_SPEED_LARGE, CURLOPT_MAX_SEND_SPEED_LARGE, CURLOPT_NETRC, CURLOPT_NETRC_FILE, CURLOPT_NEW_DIRECTORY_PERMS, CURLOPT_NEW_FILE_PERMS, CURLOPT_NOBODY, CURLOPT_NOPROGRESS, CURLOPT_NOPROXY, CURLOPT_NOSIGNAL, CURLOPT_OPENSOCKETDATA, CURLOPT_OPENSOCKETFUNCTION, CURLOPT_PASSWORD, CURLOPT_PORT, CURLOPT_POST, CURLOPT_POSTFIELDS, CURLOPT_POSTFIELDSIZE, CURLOPT_POSTFIELDSIZE_LARGE, CURLOPT_POSTQUOTE, CURLOPT_POSTREDIR, CURLOPT_PREQUOTE, CURLOPT_PRIVATE, CURLOPT_PROGRESSDATA, CURLOPT_PROGRESSFUNCTION, CURLOPT_PROTOCOLS, CURLOPT_PROXY, CURLOPT_PROXYAUTH, CURLOPT_PROXYPASSWORD, CURLOPT_PROXYPORT, CURLOPT_PROXYTYPE, CURLOPT_PROXYUSERNAME, CURLOPT_PROXYUSERPWD, CURLOPT_PROXY_TRANSFER_MODE, CURLOPT_PUT, CURLOPT_QUOTE, CURLOPT_RANDOM_FILE, CURLOPT_RANGE, CURLOPT_READDATA, CURLOPT_READFUNCTION, CURLOPT_REDIR_PROTOCOLS, CURLOPT_REFERER, CURLOPT_RESOLVE, CURLOPT_RESUME_FROM, CURLOPT_RESUME_FROM_LARGE, CURLOPT_RTSP_CLIENT_CSEQ, CURLOPT_RTSP_REQUEST, CURLOPT_RTSP_SERVER_CSEQ, CURLOPT_RTSP_SESSION_ID, CURLOPT_RTSP_STREAM_URI, CURLOPT_RTSP_TRANSPORT, CURLOPT_SEEKDATA, CURLOPT_SEEKFUNCTION, CURLOPT_SHARE, CURLOPT_SOCKOPTDATA, CURLOPT_SOCKOPTFUNCTION, CURLOPT_SOCKS5_GSSAPI_NEC, CURLOPT_SOCKS5_GSSAPI_SERVICE, CURLOPT_SSH_AUTH_TYPES, CURLOPT_SSH_HOST_PUBLIC_KEY_MD5, CURLOPT_SSH_KEYDATA, CURLOPT_SSH_KEYFUNCTION, CURLOPT_SSH_KNOWNHOSTS, CURLOPT_SSH_PRIVATE_KEYFILE, CURLOPT_SSH_PUBLIC_KEYFILE, CURLOPT_SSLCERT, CURLOPT_SSLCERTTYPE, CURLOPT_SSLENGINE, CURLOPT_SSLENGINE_DEFAULT, CURLOPT_SSLKEY, CURLOPT_SSLKEYTYPE, CURLOPT_SSLVERSION, CURLOPT_SSL_CIPHER_LIST, CURLOPT_SSL_CTX_DATA, CURLOPT_SSL_CTX_FUNCTION, CURLOPT_SSL_SESSIONID_CACHE, CURLOPT_SSL_VERIFYHOST, CURLOPT_SSL_VERIFYPEER, CURLOPT_STDERR, CURLOPT_TELNETOPTIONS, CURLOPT_TFTP_BLKSIZE, CURLOPT_TIMECONDITION, CURLOPT_TIMEOUT, CURLOPT_TIMEOUT_MS, CURLOPT_TIMEVALUE, CURLOPT_TLSAUTH_PASSWORD, CURLOPT_TLSAUTH_TYPE, CURLOPT_TLSAUTH_USERNAME, CURLOPT_TRANSFERTEXT, CURLOPT_TRANSFER_ENCODING, CURLOPT_UNRESTRICTED_AUTH, CURLOPT_UPLOAD, CURLOPT_URL, CURLOPT_USERAGENT, CURLOPT_USERNAME, CURLOPT_USERPWD, CURLOPT_USE_SSL, CURLOPT_VERBOSE, CURLOPT_WILDCARDMATCH, CURLOPT_WRITEDATA, CURLOPT_WRITEFUNCTION, CURLOPT_UNIX_SOCKET_PATH, CURLOPT_XFERINFODATA, CURLOPT_XFERINFOFUNCTION, CURLOPT_XOAUTH2_BEARER, CURLOPT_SSL_ENABLE_ALPN, CURLOPT_SSL_ENABLE_NPN, CURLOPT_SSL_FALSESTART, CURLOPT_SSL_OPTIONS, CURLOPT_SASL_IR, CURLOPT_SERVICE_NAME, CURLOPT_PROXYHEADER, CURLOPT_PATH_AS_IS, CURLOPT_PINNEDPUBLICKEY, CURLOPT_PIPEWAIT, CURLOPT_LOGIN_OPTIONS, CURLOPT_INTERFACE, CURLOPT_HEADEROPT, CURLOPT_DNS_INTERFACE, CURLOPT_DNS_LOCAL_IP4, CURLOPT_DNS_LOCAL_IP6, CURLOPT_EXPECT_100_TIMEOUT_MS, CURLOPT_MAIL_AUTH, CURLOPT_PROXY_SERVICE_NAME, CURLOPT_TCP_KEEPALIVE, CURLOPT_TCP_KEEPIDLE, CURLOPT_TCP_KEEPINTVL, CURLOPT_TCP_NODELAY, CURLOPT_SSL_VERIFYSTATUS

Για περισσότερες πληροφορίες σχετικά με αυτές τις επιλογές, επισκεφθείτε το: http://curl.haxx.se/

## Χειρισμός ιστοσελίδων

Υπάρχουν δύο διαφορετικοί τρόποι για να φορτώσετε μια σελίδα HTML: είτε μέσω μιας συνάρτησης κλήσης (callback) είτε με ένα όνομα αρχείου.

### Συνάρτηση κλήσης (Callback)

Η πρώτη δυνατότητα είναι να συσχετίσετε το αντικείμενο `url` σας με μια συνάρτηση κλήσης (callback), η οποία θα πρέπει να έχει την ακόλουθη υπογραφή:

```
function url_callback(string content, myobject o);
```

Η συνάρτηση θα συσχετιστεί με την ακόλουθη δήλωση:

```
url u(o) with url_callback.
```

Σε αυτή την περίπτωση, θα πρέπει να χρησιμοποιήσετε τη μέθοδο `url(string html)` για να φορτώσετε κάθε τμήμα κειμένου από την ιστοσελίδα σας. Για κάθε τμήμα, η συνάρτηση `url_callback` θα κληθεί με το περιεχόμενο του τμήματος ως τιμή.

Παράδειγμα:

```python
use("tamgucurl");

function fonc(string content, self o) {
    println(content);
}

curl c with fonc;

// Ορίστε έναν διαμεσολαβητή (proxy), που θα χρησιμοποιηθεί για τη φόρτωση των ιστοσελίδων σας
c.proxy("http://myproxy.mycompany:5050");

// Φορτώστε την ιστοσελίδα μας. Για κάθε τμήμα, θα κληθεί η συνάρτηση func...
c.url("http://www.liberation.fr/");
```

### Αρχείο

Η άλλη δυνατότητα είναι να παρέχετε στη μέθοδο `url` ένα όνομα αρχείου, το οποίο θα χρησιμοποιηθεί για να αποθηκευτεί το περιεχόμενο της ιστοσελίδας σας. Σε αυτή την περίπτωση, μην δηλώνετε τη μεταβλητή σας με μια συνάρτηση κλήσης.

Παράδειγμα:

```python
use("tamgucurl");

curl c;

// Ορίστε έναν διαμεσολαβητή (proxy), που θα χρησιμοποιηθεί για τη φόρτωση των ιστοσελίδων σας
c.proxy("http://myproxy.mycompany:5050");

// Φορτώστε την ιστοσελίδα μας και αποθηκεύστε το αποτέλεσμα σε ένα αρχείο
c.url("http://www.liberation.fr/", "c:\temp\myfile.html");
```

### Παράδειγμα

Αυτό το παράδειγμα δείχνει πώς να εκτελέσετε μια αναζήτηση σε έναν ιστότοπο (η διεύθυνση URL που παρέχεται εδώ δεν υπάρχει τη στιγμή που γράφτηκε αυτό το εγχειρίδιο):

```python
string mytxt;

function requester(string s, self e) {
    mytxt += s;
}

curl querying with requester;

// Ορίστε έναν διαμεσολαβητή
querying.proxy("my.proxy.com:8080");

// Ορίστε μερικές επιλογές, που είναι απαραίτητες για να προχωρήσετε με την εντολή μας
querying.options("CURLOPT_HEADER", 0);
querying.options("CURLOPT_VERBOSE", 0);
querying.options("CURLOPT_AUTOREFERER", 1);
querying.options("CURLOPT_FOLLOWLOCATION", 1);
querying.options("CURLOPT_COOKIEFILE", "");
querying.options("CURLOPT_COOKIEJAR", "");
querying.options("CURLOPT_USERAGENT", "Mozilla/4.0 (compatible;)");

function request(svector words) {
    // Κατασκευάστε την αναζήτησή μας
    string query = "http://my.any.search.engine.com/html/?q=";
    mytxt = "";
    string thequery = query + words.join("+");
    querying.url(thequery);
    println(mytxt);
}

request(["test", "word"]);
```