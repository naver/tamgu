/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusocket.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgusocket_h
#define tamgusocket_h

#ifdef UNIX
#include <unistd.h>
#endif

#ifdef WIN32
#include <WinSock.h>
#define closesock closesocket
#define readsock(x,y,z) recv(x,y,z,0)
#define writesock(x,y,z) send(x,y,z,0)
#else
#include <sys/utsname.h>        /* for uname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#define SOCKET_ERROR -1
#define closesock close
#define readsock read
#define writesock write
#define SOCKET int
#endif

const int MAXHOSTNAME = 1000;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgusocket;
//This typedef defines a type "socketMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgusocket::*socketMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgusocket : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<socketMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	

	struct sockaddr_in servAddr;
	int idclient;
	bool blocking;
	struct hostent *hp;
	char servername[MAXHOSTNAME + 10];
	int port;
	SOCKET sock;
	bool server;
	//For inserting a time out
	fd_set readfds;
	fd_set writefds;
	struct timeval tv;
	bool timeout;
	int v_timeout;

	map<SOCKET, bool> socketclients;
#ifdef WIN32
	int len;
#else
	socklen_t len;
#endif

	bool root;
    //---------------------------------------------------------------------------------------------------------------------
	Tamgusocket(TamguGlobal* g, Tamgu* parent = NULL);

	~Tamgusocket();

	void Methods(Tamgu* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}


	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgusocket::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "socket";
    }

    

    

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false....
	bool duplicateForCall() {
		return false;
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
        return new Tamgusocket(globalTamgu);
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, socketMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

	Tamgu* MethodCreateServer(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCreateClient(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWait(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReadRaw(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGet(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWriteRaw(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodBlock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodTimeout(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGethostname(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodGetpeername(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

	//---------------------------------------------------------------------------------------------------------------------

	void Close() {
		if (hp != NULL && sock != -1) {
			closesock(sock);

			for (auto& it : socketclients)
				closesock(it.first);
			socketclients.clear();
		}

		hp = NULL;
		sock = -1;
	}

	Tamgu* Write(string act, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* CreateSocket(short);
	bool TestTimeOutRead(short idthread, SOCKET currentsock);
	bool TestTimeOutWrite(short idthread, SOCKET currentsock);


    void Clear() {
        //To set a variable back to empty
    }

    

    Tamgu* same(Tamgu* a) {
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* different(Tamgu* a) {
        if (a != this)
            return aTRUE;
        return aFALSE;
    }

};

#endif




