/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgucurl.cxx
 Date       : 2017/09/01
 Purpose    : curl library encapsulation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#if (_MSC_VER >= 1900)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

#include "tamgu.h"
#include "tamguversion.h"
#include "tamgucurl.h"
#include "instructions.h"

static map<string, CURLoption> curloptions;

static void Init()
{
	static bool init = false;
	if (!init)
	{
		init = true;
		curloptions["CURLOPT_ACCEPTTIMEOUT_MS"] = CURLOPT_ACCEPTTIMEOUT_MS;
		curloptions["CURLOPT_ACCEPT_ENCODING"] = CURLOPT_ACCEPT_ENCODING;
		curloptions["CURLOPT_ADDRESS_SCOPE"] = CURLOPT_ADDRESS_SCOPE;
		curloptions["CURLOPT_APPEND"] = CURLOPT_APPEND;
		curloptions["CURLOPT_AUTOREFERER"] = CURLOPT_AUTOREFERER;
		curloptions["CURLOPT_BUFFERSIZE"] = CURLOPT_BUFFERSIZE;
		curloptions["CURLOPT_CAINFO"] = CURLOPT_CAINFO;
		curloptions["CURLOPT_CAPATH"] = CURLOPT_CAPATH;
		curloptions["CURLOPT_CERTINFO"] = CURLOPT_CERTINFO;
		curloptions["CURLOPT_CHUNK_BGN_FUNCTION"] = CURLOPT_CHUNK_BGN_FUNCTION;
		curloptions["CURLOPT_CHUNK_DATA"] = CURLOPT_CHUNK_DATA;
		curloptions["CURLOPT_CHUNK_END_FUNCTION"] = CURLOPT_CHUNK_END_FUNCTION;
		curloptions["CURLOPT_CLOSESOCKETDATA"] = CURLOPT_CLOSESOCKETDATA;
		curloptions["CURLOPT_CLOSESOCKETFUNCTION"] = CURLOPT_CLOSESOCKETFUNCTION;
		curloptions["CURLOPT_CONNECTTIMEOUT"] = CURLOPT_CONNECTTIMEOUT;
		curloptions["CURLOPT_CONNECTTIMEOUT_MS"] = CURLOPT_CONNECTTIMEOUT_MS;
		curloptions["CURLOPT_CONNECT_ONLY"] = CURLOPT_CONNECT_ONLY;
		curloptions["CURLOPT_CONV_FROM_NETWORK_FUNCTION"] = CURLOPT_CONV_FROM_NETWORK_FUNCTION;
		curloptions["CURLOPT_CONV_FROM_UTF8_FUNCTION"] = CURLOPT_CONV_FROM_UTF8_FUNCTION;
		curloptions["CURLOPT_CONV_TO_NETWORK_FUNCTION"] = CURLOPT_CONV_TO_NETWORK_FUNCTION;
		curloptions["CURLOPT_COOKIE"] = CURLOPT_COOKIE;
		curloptions["CURLOPT_COOKIEFILE"] = CURLOPT_COOKIEFILE;
		curloptions["CURLOPT_COOKIEJAR"] = CURLOPT_COOKIEJAR;
		curloptions["CURLOPT_COOKIELIST"] = CURLOPT_COOKIELIST;
		curloptions["CURLOPT_COOKIESESSION"] = CURLOPT_COOKIESESSION;
		curloptions["CURLOPT_COPYPOSTFIELDS"] = CURLOPT_COPYPOSTFIELDS;
		curloptions["CURLOPT_CRLF"] = CURLOPT_CRLF;
		curloptions["CURLOPT_CRLFILE"] = CURLOPT_CRLFILE;
		curloptions["CURLOPT_CUSTOMREQUEST"] = CURLOPT_CUSTOMREQUEST;
		curloptions["CURLOPT_DEBUGDATA"] = CURLOPT_DEBUGDATA;
		curloptions["CURLOPT_DEBUGFUNCTION"] = CURLOPT_DEBUGFUNCTION;
		curloptions["CURLOPT_DIRLISTONLY"] = CURLOPT_DIRLISTONLY;
		curloptions["CURLOPT_DNS_CACHE_TIMEOUT"] = CURLOPT_DNS_CACHE_TIMEOUT;
		curloptions["CURLOPT_DNS_SERVERS"] = CURLOPT_DNS_SERVERS;
		curloptions["CURLOPT_DNS_USE_GLOBAL_CACHE"] = CURLOPT_DNS_USE_GLOBAL_CACHE;
		curloptions["CURLOPT_EGDSOCKET"] = CURLOPT_EGDSOCKET;
		curloptions["CURLOPT_ERRORBUFFER"] = CURLOPT_ERRORBUFFER;
		curloptions["CURLOPT_FAILONERROR"] = CURLOPT_FAILONERROR;
		curloptions["CURLOPT_FILETIME"] = CURLOPT_FILETIME;
		curloptions["CURLOPT_FNMATCH_DATA"] = CURLOPT_FNMATCH_DATA;
		curloptions["CURLOPT_FNMATCH_FUNCTION"] = CURLOPT_FNMATCH_FUNCTION;
		curloptions["CURLOPT_FOLLOWLOCATION"] = CURLOPT_FOLLOWLOCATION;
		curloptions["CURLOPT_FORBID_REUSE"] = CURLOPT_FORBID_REUSE;
		curloptions["CURLOPT_FRESH_CONNECT"] = CURLOPT_FRESH_CONNECT;
		curloptions["CURLOPT_FTPPORT"] = CURLOPT_FTPPORT;
		curloptions["CURLOPT_FTPSSLAUTH"] = CURLOPT_FTPSSLAUTH;
		curloptions["CURLOPT_FTP_ACCOUNT"] = CURLOPT_FTP_ACCOUNT;
		curloptions["CURLOPT_FTP_ALTERNATIVE_TO_USER"] = CURLOPT_FTP_ALTERNATIVE_TO_USER;
		curloptions["CURLOPT_FTP_CREATE_MISSING_DIRS"] = CURLOPT_FTP_CREATE_MISSING_DIRS;
		curloptions["CURLOPT_FTP_FILEMETHOD"] = CURLOPT_FTP_FILEMETHOD;
		curloptions["CURLOPT_FTP_RESPONSE_TIMEOUT"] = CURLOPT_FTP_RESPONSE_TIMEOUT;
		curloptions["CURLOPT_FTP_SKIP_PASV_IP"] = CURLOPT_FTP_SKIP_PASV_IP;
		curloptions["CURLOPT_FTP_SSL_CCC"] = CURLOPT_FTP_SSL_CCC;
		curloptions["CURLOPT_FTP_USE_EPRT"] = CURLOPT_FTP_USE_EPRT;
		curloptions["CURLOPT_FTP_USE_EPSV"] = CURLOPT_FTP_USE_EPSV;
		curloptions["CURLOPT_FTP_USE_PRET"] = CURLOPT_FTP_USE_PRET;
		curloptions["CURLOPT_GSSAPI_DELEGATION"] = CURLOPT_GSSAPI_DELEGATION;
		curloptions["CURLOPT_HEADER"] = CURLOPT_HEADER;
		curloptions["CURLOPT_HEADERDATA"] = CURLOPT_HEADERDATA;
		curloptions["CURLOPT_HEADERFUNCTION"] = CURLOPT_HEADERFUNCTION;
		curloptions["CURLOPT_HTTP200ALIASES"] = CURLOPT_HTTP200ALIASES;
		curloptions["CURLOPT_HTTPAUTH"] = CURLOPT_HTTPAUTH;
		curloptions["CURLOPT_HTTPGET"] = CURLOPT_HTTPGET;
		curloptions["CURLOPT_HTTPHEADER"] = CURLOPT_HTTPHEADER;
		curloptions["CURLOPT_HTTPPOST"] = CURLOPT_HTTPPOST;
		curloptions["CURLOPT_HTTPPROXYTUNNEL"] = CURLOPT_HTTPPROXYTUNNEL;
		curloptions["CURLOPT_HTTP_CONTENT_DECODING"] = CURLOPT_HTTP_CONTENT_DECODING;
		curloptions["CURLOPT_HTTP_TRANSFER_DECODING"] = CURLOPT_HTTP_TRANSFER_DECODING;
		curloptions["CURLOPT_HTTP_VERSION"] = CURLOPT_HTTP_VERSION;
		curloptions["CURLOPT_IGNORE_CONTENT_LENGTH"] = CURLOPT_IGNORE_CONTENT_LENGTH;
		curloptions["CURLOPT_INFILESIZE"] = CURLOPT_INFILESIZE;
		curloptions["CURLOPT_INFILESIZE_LARGE"] = CURLOPT_INFILESIZE_LARGE;
		curloptions["CURLOPT_INTERLEAVEDATA"] = CURLOPT_INTERLEAVEDATA;
		curloptions["CURLOPT_INTERLEAVEFUNCTION"] = CURLOPT_INTERLEAVEFUNCTION;
		curloptions["CURLOPT_IOCTLDATA"] = CURLOPT_IOCTLDATA;
		curloptions["CURLOPT_IOCTLFUNCTION"] = CURLOPT_IOCTLFUNCTION;
		curloptions["CURLOPT_IPRESOLVE"] = CURLOPT_IPRESOLVE;
		curloptions["CURLOPT_ISSUERCERT"] = CURLOPT_ISSUERCERT;
		curloptions["CURLOPT_KEYPASSWD"] = CURLOPT_KEYPASSWD;
		curloptions["CURLOPT_KRBLEVEL"] = CURLOPT_KRBLEVEL;
		curloptions["CURLOPT_LOCALPORT"] = CURLOPT_LOCALPORT;
		curloptions["CURLOPT_LOCALPORTRANGE"] = CURLOPT_LOCALPORTRANGE;
		curloptions["CURLOPT_LOW_SPEED_LIMIT"] = CURLOPT_LOW_SPEED_LIMIT;
		curloptions["CURLOPT_LOW_SPEED_TIME"] = CURLOPT_LOW_SPEED_TIME;
		curloptions["CURLOPT_MAIL_FROM"] = CURLOPT_MAIL_FROM;
		curloptions["CURLOPT_MAIL_RCPT"] = CURLOPT_MAIL_RCPT;
		curloptions["CURLOPT_MAXCONNECTS"] = CURLOPT_MAXCONNECTS;
		curloptions["CURLOPT_MAXFILESIZE"] = CURLOPT_MAXFILESIZE;
		curloptions["CURLOPT_MAXFILESIZE_LARGE"] = CURLOPT_MAXFILESIZE_LARGE;
		curloptions["CURLOPT_MAXREDIRS"] = CURLOPT_MAXREDIRS;
		curloptions["CURLOPT_MAX_RECV_SPEED_LARGE"] = CURLOPT_MAX_RECV_SPEED_LARGE;
		curloptions["CURLOPT_MAX_SEND_SPEED_LARGE"] = CURLOPT_MAX_SEND_SPEED_LARGE;
		curloptions["CURLOPT_NETRC"] = CURLOPT_NETRC;
		curloptions["CURLOPT_NETRC_FILE"] = CURLOPT_NETRC_FILE;
		curloptions["CURLOPT_NEW_DIRECTORY_PERMS"] = CURLOPT_NEW_DIRECTORY_PERMS;
		curloptions["CURLOPT_NEW_FILE_PERMS"] = CURLOPT_NEW_FILE_PERMS;
		curloptions["CURLOPT_NOBODY"] = CURLOPT_NOBODY;
		curloptions["CURLOPT_NOPROGRESS"] = CURLOPT_NOPROGRESS;
		curloptions["CURLOPT_NOPROXY"] = CURLOPT_NOPROXY;
		curloptions["CURLOPT_NOSIGNAL"] = CURLOPT_NOSIGNAL;
		curloptions["CURLOPT_OPENSOCKETDATA"] = CURLOPT_OPENSOCKETDATA;
		curloptions["CURLOPT_OPENSOCKETFUNCTION"] = CURLOPT_OPENSOCKETFUNCTION;
		curloptions["CURLOPT_PASSWORD"] = CURLOPT_PASSWORD;
		curloptions["CURLOPT_PORT"] = CURLOPT_PORT;
		curloptions["CURLOPT_POST"] = CURLOPT_POST;
		curloptions["CURLOPT_POSTFIELDS"] = CURLOPT_POSTFIELDS;
		curloptions["CURLOPT_POSTFIELDSIZE"] = CURLOPT_POSTFIELDSIZE;
		curloptions["CURLOPT_POSTFIELDSIZE_LARGE"] = CURLOPT_POSTFIELDSIZE_LARGE;
		curloptions["CURLOPT_POSTQUOTE"] = CURLOPT_POSTQUOTE;
		curloptions["CURLOPT_POSTREDIR"] = CURLOPT_POSTREDIR;
		curloptions["CURLOPT_PREQUOTE"] = CURLOPT_PREQUOTE;
		curloptions["CURLOPT_PRIVATE"] = CURLOPT_PRIVATE;
		curloptions["CURLOPT_PROGRESSDATA"] = CURLOPT_PROGRESSDATA;
		curloptions["CURLOPT_PROGRESSFUNCTION"] = CURLOPT_PROGRESSFUNCTION;
		curloptions["CURLOPT_PROTOCOLS"] = CURLOPT_PROTOCOLS;
		curloptions["CURLOPT_PROXY"] = CURLOPT_PROXY;
		curloptions["CURLOPT_PROXYAUTH"] = CURLOPT_PROXYAUTH;
		curloptions["CURLOPT_PROXYPASSWORD"] = CURLOPT_PROXYPASSWORD;
		curloptions["CURLOPT_PROXYPORT"] = CURLOPT_PROXYPORT;
		curloptions["CURLOPT_PROXYTYPE"] = CURLOPT_PROXYTYPE;
		curloptions["CURLOPT_PROXYUSERNAME"] = CURLOPT_PROXYUSERNAME;
		curloptions["CURLOPT_PROXYUSERPWD"] = CURLOPT_PROXYUSERPWD;
		curloptions["CURLOPT_PROXY_TRANSFER_MODE"] = CURLOPT_PROXY_TRANSFER_MODE;
		curloptions["CURLOPT_PUT"] = CURLOPT_PUT;
		curloptions["CURLOPT_QUOTE"] = CURLOPT_QUOTE;
		curloptions["CURLOPT_RANDOM_FILE"] = CURLOPT_RANDOM_FILE;
		curloptions["CURLOPT_RANGE"] = CURLOPT_RANGE;
		curloptions["CURLOPT_READDATA"] = CURLOPT_READDATA;
		curloptions["CURLOPT_READFUNCTION"] = CURLOPT_READFUNCTION;
		curloptions["CURLOPT_REDIR_PROTOCOLS"] = CURLOPT_REDIR_PROTOCOLS;
		curloptions["CURLOPT_REFERER"] = CURLOPT_REFERER;
		curloptions["CURLOPT_RESOLVE"] = CURLOPT_RESOLVE;
		curloptions["CURLOPT_RESUME_FROM"] = CURLOPT_RESUME_FROM;
		curloptions["CURLOPT_RESUME_FROM_LARGE"] = CURLOPT_RESUME_FROM_LARGE;
		curloptions["CURLOPT_RTSP_CLIENT_CSEQ"] = CURLOPT_RTSP_CLIENT_CSEQ;
		curloptions["CURLOPT_RTSP_REQUEST"] = CURLOPT_RTSP_REQUEST;
		curloptions["CURLOPT_RTSP_SERVER_CSEQ"] = CURLOPT_RTSP_SERVER_CSEQ;
		curloptions["CURLOPT_RTSP_SESSION_ID"] = CURLOPT_RTSP_SESSION_ID;
		curloptions["CURLOPT_RTSP_STREAM_URI"] = CURLOPT_RTSP_STREAM_URI;
		curloptions["CURLOPT_RTSP_TRANSPORT"] = CURLOPT_RTSP_TRANSPORT;
		curloptions["CURLOPT_SEEKDATA"] = CURLOPT_SEEKDATA;
		curloptions["CURLOPT_SEEKFUNCTION"] = CURLOPT_SEEKFUNCTION;
		curloptions["CURLOPT_SHARE"] = CURLOPT_SHARE;
		curloptions["CURLOPT_SOCKOPTDATA"] = CURLOPT_SOCKOPTDATA;
		curloptions["CURLOPT_SOCKOPTFUNCTION"] = CURLOPT_SOCKOPTFUNCTION;
		curloptions["CURLOPT_SOCKS5_GSSAPI_NEC"] = CURLOPT_SOCKS5_GSSAPI_NEC;
		curloptions["CURLOPT_SOCKS5_GSSAPI_SERVICE"] = CURLOPT_SOCKS5_GSSAPI_SERVICE;
		curloptions["CURLOPT_SSH_AUTH_TYPES"] = CURLOPT_SSH_AUTH_TYPES;
		curloptions["CURLOPT_SSH_HOST_PUBLIC_KEY_MD5"] = CURLOPT_SSH_HOST_PUBLIC_KEY_MD5;
		curloptions["CURLOPT_SSH_KEYDATA"] = CURLOPT_SSH_KEYDATA;
		curloptions["CURLOPT_SSH_KEYFUNCTION"] = CURLOPT_SSH_KEYFUNCTION;
		curloptions["CURLOPT_SSH_KNOWNHOSTS"] = CURLOPT_SSH_KNOWNHOSTS;
		curloptions["CURLOPT_SSH_PRIVATE_KEYFILE"] = CURLOPT_SSH_PRIVATE_KEYFILE;
		curloptions["CURLOPT_SSH_PUBLIC_KEYFILE"] = CURLOPT_SSH_PUBLIC_KEYFILE;
		curloptions["CURLOPT_SSLCERT"] = CURLOPT_SSLCERT;
		curloptions["CURLOPT_SSLCERTTYPE"] = CURLOPT_SSLCERTTYPE;
		curloptions["CURLOPT_SSLENGINE"] = CURLOPT_SSLENGINE;
		curloptions["CURLOPT_SSLENGINE_DEFAULT"] = CURLOPT_SSLENGINE_DEFAULT;
		curloptions["CURLOPT_SSLKEY"] = CURLOPT_SSLKEY;
		curloptions["CURLOPT_SSLKEYTYPE"] = CURLOPT_SSLKEYTYPE;
		curloptions["CURLOPT_SSLVERSION"] = CURLOPT_SSLVERSION;
		curloptions["CURLOPT_SSL_CIPHER_LIST"] = CURLOPT_SSL_CIPHER_LIST;
		curloptions["CURLOPT_SSL_CTX_DATA"] = CURLOPT_SSL_CTX_DATA;
		curloptions["CURLOPT_SSL_CTX_FUNCTION"] = CURLOPT_SSL_CTX_FUNCTION;
		curloptions["CURLOPT_SSL_SESSIONID_CACHE"] = CURLOPT_SSL_SESSIONID_CACHE;
		curloptions["CURLOPT_SSL_VERIFYHOST"] = CURLOPT_SSL_VERIFYHOST;
		curloptions["CURLOPT_SSL_VERIFYPEER"] = CURLOPT_SSL_VERIFYPEER;
		curloptions["CURLOPT_STDERR"] = CURLOPT_STDERR;
		curloptions["CURLOPT_TELNETOPTIONS"] = CURLOPT_TELNETOPTIONS;
		curloptions["CURLOPT_TFTP_BLKSIZE"] = CURLOPT_TFTP_BLKSIZE;
		curloptions["CURLOPT_TIMECONDITION"] = CURLOPT_TIMECONDITION;
		curloptions["CURLOPT_TIMEOUT"] = CURLOPT_TIMEOUT;
		curloptions["CURLOPT_TIMEOUT_MS"] = CURLOPT_TIMEOUT_MS;
		curloptions["CURLOPT_TIMEVALUE"] = CURLOPT_TIMEVALUE;
		curloptions["CURLOPT_TLSAUTH_PASSWORD"] = CURLOPT_TLSAUTH_PASSWORD;
		curloptions["CURLOPT_TLSAUTH_TYPE"] = CURLOPT_TLSAUTH_TYPE;
		curloptions["CURLOPT_TLSAUTH_USERNAME"] = CURLOPT_TLSAUTH_USERNAME;
		curloptions["CURLOPT_TRANSFERTEXT"] = CURLOPT_TRANSFERTEXT;
		curloptions["CURLOPT_TRANSFER_ENCODING"] = CURLOPT_TRANSFER_ENCODING;
		curloptions["CURLOPT_UNRESTRICTED_AUTH"] = CURLOPT_UNRESTRICTED_AUTH;
		curloptions["CURLOPT_UPLOAD"] = CURLOPT_UPLOAD;
		curloptions["CURLOPT_URL"] = CURLOPT_URL;
		curloptions["CURLOPT_USERAGENT"] = CURLOPT_USERAGENT;
		curloptions["CURLOPT_USERNAME"] = CURLOPT_USERNAME;
		curloptions["CURLOPT_USERPWD"] = CURLOPT_USERPWD;
		curloptions["CURLOPT_USE_SSL"] = CURLOPT_USE_SSL;
		curloptions["CURLOPT_VERBOSE"] = CURLOPT_VERBOSE;
		curloptions["CURLOPT_WILDCARDMATCH"] = CURLOPT_WILDCARDMATCH;
		curloptions["CURLOPT_WRITEDATA"] = CURLOPT_WRITEDATA;
		curloptions["CURLOPT_WRITEFUNCTION"] = CURLOPT_WRITEFUNCTION;
		curloptions["CURLOPT_SSL_OPTIONS"] = CURLOPT_SSL_OPTIONS;
		curloptions["CURLOPT_INTERFACE"] = CURLOPT_INTERFACE;
		curloptions["CURLOPT_MAIL_AUTH"] = CURLOPT_MAIL_AUTH;
		curloptions["CURLOPT_TCP_KEEPALIVE"] = CURLOPT_TCP_KEEPALIVE;
		curloptions["CURLOPT_TCP_KEEPIDLE"] = CURLOPT_TCP_KEEPIDLE;
		curloptions["CURLOPT_TCP_KEEPINTVL"] = CURLOPT_TCP_KEEPINTVL;
		curloptions["CURLOPT_TCP_NODELAY"] = CURLOPT_TCP_NODELAY;

#ifndef MAVERICK
		curloptions["CURLOPT_UNIX_SOCKET_PATH"] = CURLOPT_UNIX_SOCKET_PATH;
		curloptions["CURLOPT_XFERINFODATA"] = CURLOPT_XFERINFODATA;
		curloptions["CURLOPT_XFERINFOFUNCTION"] = CURLOPT_XFERINFOFUNCTION;
		curloptions["CURLOPT_XOAUTH2_BEARER"] = CURLOPT_XOAUTH2_BEARER;
		curloptions["CURLOPT_SSL_ENABLE_ALPN"] = CURLOPT_SSL_ENABLE_ALPN;
		curloptions["CURLOPT_SSL_ENABLE_NPN"] = CURLOPT_SSL_ENABLE_NPN;
		curloptions["CURLOPT_SSL_FALSESTART"] = CURLOPT_SSL_FALSESTART;
		curloptions["CURLOPT_SASL_IR"] = CURLOPT_SASL_IR;
		curloptions["CURLOPT_SERVICE_NAME"] = CURLOPT_SERVICE_NAME;
		curloptions["CURLOPT_PROXYHEADER"] = CURLOPT_PROXYHEADER;
		curloptions["CURLOPT_PATH_AS_IS"] = CURLOPT_PATH_AS_IS;
		curloptions["CURLOPT_PINNEDPUBLICKEY"] = CURLOPT_PINNEDPUBLICKEY;
		curloptions["CURLOPT_PIPEWAIT"] = CURLOPT_PIPEWAIT;
		curloptions["CURLOPT_LOGIN_OPTIONS"] = CURLOPT_LOGIN_OPTIONS;
		curloptions["CURLOPT_HEADEROPT"] = CURLOPT_HEADEROPT;
		curloptions["CURLOPT_DNS_INTERFACE"] = CURLOPT_DNS_INTERFACE;
		curloptions["CURLOPT_DNS_LOCAL_IP4"] = CURLOPT_DNS_LOCAL_IP4;
		curloptions["CURLOPT_DNS_LOCAL_IP6"] = CURLOPT_DNS_LOCAL_IP6;
		curloptions["CURLOPT_EXPECT_100_TIMEOUT_MS"] = CURLOPT_EXPECT_100_TIMEOUT_MS;
		curloptions["CURLOPT_PROXY_SERVICE_NAME"] = CURLOPT_PROXY_SERVICE_NAME;
		curloptions["CURLOPT_SSL_VERIFYSTATUS"] = CURLOPT_SSL_VERIFYSTATUS;
#endif
	}
}

static size_t call_writing(char *ptr, size_t size, size_t nmemb, Tamgucurl *userdata)
{
	int max = size * nmemb;

	TamguCallFunction kfunc(userdata->function);

	string s;
	for (int i = 0; i < max; i++)
		s += ptr[i];

	Tamgustring *kstr = globalTamgu->Providestring(s);
	kstr->Setreference();
	userdata->object->Setreference();
	kfunc.arguments.push_back(kstr);
	kfunc.arguments.push_back(userdata->object);

	kfunc.Eval(aNULL, aNULL, userdata->thread_id);

	kstr->Resetreference();
	userdata->object->Protect();

	return max;
}

// We need to declare once again our local definitions.
basebin_hash<curlMethod> Tamgucurl::methods;
ThreadLock classlock;
short Tamgucurl::idtype = 0;

// MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgucurl::AddMethod(TamguGlobal *global, string name, curlMethod func, unsigned long arity, string infos)
{
	short idname = global->Getid(name);
	methods[idname] = func;
	if (global->infomethods.find(idtype) != global->infomethods.end() &&
		global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
		return;

	global->infomethods[idtype][name] = infos;
	global->RecordArity(idtype, idname, arity);
}

Tamgu *Tamgucurl::errormsg(short idthread, CURLcode res)
{
	classlock.Locking();
	const char *errmsg = curl_easy_strerror(res);
	classlock.Unlocking();
	char ch[1024];
	sprintf_s(ch, 1024, "URL(%d): %s", res, errmsg);
	return globalTamgu->Returnerror(ch, idthread);
}

//------------------------------------------------------------------------------------------------------------------
// If you need to implement an external library... Otherwise remove it...
// When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C"
{
	Exporting bool tamgucurl_InitialisationModule(TamguGlobal *global, string version)
	{
		if (version != TamguVersion())
			return false;

		global->Update();

		return Tamgucurl::InitialisationModule(global, version);
	}
}

Tamgucurl::Tamgucurl(Tamgu *f, TamguGlobal *g) : TamguReference(g)
{
	// Do not forget your variable initialisation
	classlock.Locking();
	curl = curl_easy_init();
	classlock.Unlocking();
	thread_id = 0;
	function = f;
	object = aNULL;
	urlsize = 4096;
	urlbuffer = (char *)malloc(urlsize);
}

Tamgucurl::~Tamgucurl()
{
	if (urlbuffer != NULL)
		free(urlbuffer);
	if (curl != NULL)
	{
		classlock.Locking();
		curl_easy_cleanup(curl);
		classlock.Unlocking();
	}
}

void Tamgucurl::Setidtype(TamguGlobal *global)
{
	Locking lock(classlock);
	if (Tamgucurl::methods.isEmpty())
		Tamgucurl::InitialisationModule(global, "");
}

bool Tamgucurl::InitialisationModule(TamguGlobal *global, string version)
{
	methods.clear();

	Init();
	Tamgucurl::idtype = global->Getid("curl");
	Tamgucurl::AddMethod(global, "_initial", &Tamgucurl::MethodInitial, P_ONE, "_initial(obj): catch an object for function call");
	Tamgucurl::AddMethod(global, "proxy", &Tamgucurl::MethodProxy, P_ONE, "proxy(string prox): Proxy connection");
	Tamgucurl::AddMethod(global, "password", &Tamgucurl::MethodPWD, P_TWO, "password(string user, string pwd): Provide the user password");
	Tamgucurl::AddMethod(global, "url", &Tamgucurl::MethodURL, P_ONE | P_TWO, "url(string path): Load a URL\rurl(string pathstring filename): Load a url into a filename");
	Tamgucurl::AddMethod(global, "execute", &Tamgucurl::MethodExecute, P_NONE | P_ONE, "execute(string filename): Execute a curl action filename is optional.");
	Tamgucurl::AddMethod(global, "options", &Tamgucurl::MethodOptions, P_TWO, "options(string option, string parameter): Set the options of the curl object");

	global->newInstance[Tamgucurl::idtype] = new Tamgucurl(global);
	global->RecordCompatibilities(Tamgucurl::idtype);

	return true;
}

Tamgu *Tamgucurl::Put(Tamgu *index, Tamgu *value, short idthread)
{
	return this;
}

Tamgu *Tamgucurl::Eval(Tamgu *context, Tamgu *index, short idthread)
{
	return this;
}

Tamgu *Tamgucurl::MethodProxy(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
	// In our example, we have only two parameters
	// 0 is the first parameter and so on...
	string sproxy = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	strcpy_s(urlbuffer, urlsize, STR(sproxy));
	CURLcode res = curl_easy_setopt(curl, CURLOPT_PROXY, urlbuffer);
	if (res == 0)
		return aTRUE;
	return errormsg(idthread, res);
}

Tamgu *Tamgucurl::MethodPWD(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
	// In our example, we have only two parameters
	// 0 is the first parameter and so on...
	string user = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	string pwd = callfunc->Evaluate(1, contextualpattern, idthread)->String();
	user += ":";
	user += pwd;
	strcpy_s(urlbuffer, urlsize, STR(user));
	CURLcode res = curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, urlbuffer);
	if (res == 0)
		return aTRUE;
	return errormsg(idthread, res);
}

Tamgu *Tamgucurl::MethodURL(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
	// In our example, we have only two parameters
	// 0 is the first parameter and so on...
	thread_id = idthread;
	urlstr = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (urlstr != "")
	{
		if (urlstr.size() >= urlsize)
		{
			free(urlbuffer);
			urlsize = urlstr.size() * 1.5;
			urlbuffer = (char *)malloc(urlsize);
		}
		strcpy_s(urlbuffer, urlsize, STR(urlstr));
		curl_easy_setopt(curl, CURLOPT_URL, urlbuffer);
	}
	FILE *tmp = NULL;
	if (callfunc->Size() == 1)
	{
		if (function != NULL)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_writing);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		}
	}
	else
	{
		string filename = callfunc->Evaluate(1, contextualpattern, idthread)->String();
#ifdef WIN32
		fopen_s(&tmp, STR(filename), "w");
#else
		tmp = fopen(STR(filename), "w");
#endif

		if (tmp == NULL)
		{
			return globalTamgu->Returnerror("URL(009): Wrong filename", idthread);
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, tmp);
	}
	CURLcode res = curl_easy_perform(curl);

	if (tmp != NULL)
		fclose(tmp);
	if (res == 0)
		return aTRUE;
	return errormsg(idthread, res);
}

Tamgu *Tamgucurl::MethodExecute(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
	// In our example, we have only two parameters
	// 0 is the first parameter and so on...
	thread_id = idthread;

	FILE *tmp = NULL;
	if (callfunc->Size() == 0)
	{
		if (function != NULL)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, call_writing);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		}
	}
	else
	{
		string filename = callfunc->Evaluate(0, contextualpattern, idthread)->String();
#ifdef WIN32
		fopen_s(&tmp, STR(filename), "w");
#else
		tmp = fopen(STR(filename), "w");
#endif

		if (tmp == NULL)
		{
			return globalTamgu->Returnerror("URL(009): Wrong filename", idthread);
		}

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, tmp);
	}

	CURLcode res = curl_easy_perform(curl);

	if (tmp != NULL)
		fclose(tmp);
	if (res == 0)
		return aTRUE;
	return errormsg(idthread, res);
}

Tamgu *Tamgucurl::MethodOptions(Tamgu *contextualpattern, short idthread, TamguCall *callfunc)
{
	string option = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	if (curloptions.find(option) == curloptions.end())
		return globalTamgu->Returnerror("URL(031): Unknown option", idthread);
	CURLcode res;
	CURLoption noption = curloptions[option];
	Tamgu *kdata = callfunc->Evaluate(1, contextualpattern, idthread);
	if (kdata->isNumber())
	{
		long data = kdata->Integer();
		res = curl_easy_setopt(curl, noption, data);
	}
	else
	{
		string data;
		if (kdata->isContainer())
			data = kdata->JSonString();
		else
			data = kdata->String();

		if (data.size() >= urlsize)
		{
			free(urlbuffer);
			urlsize = data.size() * 1.5;
			urlbuffer = (char *)malloc(urlsize);
		}
		strcpy_s(urlbuffer, urlsize, STR(data));
		res = curl_easy_setopt(curl, noption, urlbuffer);
	}

	if (res == 0)
		return aTRUE;
	return errormsg(idthread, res);
}
