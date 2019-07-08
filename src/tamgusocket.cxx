/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusocket.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgumap.h"
#include "tamgubyte.h"
#include "tamgusocket.h"

const int MAXSIZEINPUT = 256;
const int POSSIGNATURE = 5;
const char* FORMATSIGNATURE = "%4x";

static vector<Tamgusocket*> sockets;
static vector<long> isockets;

#ifdef WIN32
#define BIND ::bind
#else
#define BIND bind
#endif

string Msgerror() {
    char* s = strerror(errno);
    if (strstr(s, "No error") != NULL)
        return "";
    return s;
}


static short XConvert(char* number, int nb) {
    unsigned short v = 0;
    for (int i = 0; i<nb; i++) {
        switch (number[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            v <<= 4;
            v |= number[i] - 48;
            break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            v <<= 4;
            v |= number[i] - 55;
            break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            v <<= 4;
            v |= number[i] - 87;
        }
    }
    return v;
}

static bool checkipaddres(string serveraddr) {
    int countpoint = 0;
    for (int i = 0; i<serveraddr.size(); i++) {
        if (serveraddr[i] == '.') {
            countpoint++;
            if (countpoint>3)
                return false;
        }
        else
            if (serveraddr[i]<48 || serveraddr[i]>57)
                return false;
    }
    return true;
}

static bool validstream(int nb) {
    if (nb == SOCKET_ERROR || nb == 0)
        return false;
    return true;
}


//----------------------------------------------------------------------------------------------
//We need to declare once again our local definitions.
Exporting basebin_hash<socketMethod>  Tamgusocket::methods;
Exporting hmap<string, string> Tamgusocket::infomethods;
Exporting bin_hash<unsigned long> Tamgusocket::exported;

Exporting short Tamgusocket::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgusocket::AddMethod(TamguGlobal* global, string name, socketMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

#ifdef WIN32
static WSADATA WSAData;
static bool wsainit = true;
#endif



    void Tamgusocket::Setidtype(TamguGlobal* global) {
        Tamgusocket::idtype = global->Getid("socket");
    }

   bool Tamgusocket::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();
    sockets.clear();
    isockets.clear();

    Tamgusocket::idtype = global->Getid("socket");

    Tamgusocket::AddMethod(global, "createserver", &Tamgusocket::MethodCreateServer, P_THREE | P_TWO, "createserver(string hostname,int port,int nblients): create a server if hostname is omitted then the local hostname is used");
    Tamgusocket::AddMethod(global, "connect", &Tamgusocket::MethodCreateClient, P_TWO, "connect(string hostname,int port): connect to the server");
    Tamgusocket::AddMethod(global, "wait", &Tamgusocket::MethodWait, P_NONE, "wait(): wait for a client to connect and returns its socket id");
    Tamgusocket::AddMethod(global, "read", &Tamgusocket::MethodRead, P_ONE | P_NONE, "read(int num): read a string on a socket. On the server side 'num' is the value returned by 'wait()'. Use 'read()' on the client side");
    Tamgusocket::AddMethod(global, "write", &Tamgusocket::MethodWrite, P_ATLEASTONE, "write(int num,string s): write the string s on the socket. On the server side num is the value returned by wait()'. Use 'write(string s)' on the client side");
    Tamgusocket::AddMethod(global, "receive", &Tamgusocket::MethodReadRaw, P_TWO | P_ONE | P_NONE, "receive(int num,int nb): read a string on a socket in a non Tamgu environment. On the server side 'num' is the value returned by 'wait()'. Use 'receive()' on the client side");
    Tamgusocket::AddMethod(global, "get", &Tamgusocket::MethodGet, P_ONE | P_NONE, "get(int num): get one character from a socket in a non Tamgu environment. On the server side 'num' is the value returned by 'wait()'. Use 'get()' on the client side");
    Tamgusocket::AddMethod(global, "send", &Tamgusocket::MethodWriteRaw, P_ATLEASTONE, "send(int num,string s): write the string s on the socket in a non Tamgu environment. On the server side num is the value returned by wait()'. Use 'send(string s)' on the client side");
    Tamgusocket::AddMethod(global, "close", &Tamgusocket::MethodClose, P_NONE | P_ONE, "close(int num): Close a socket. On the server side if 'num' is provided (it is the value returned by wait()) it closes the client socket otherwise it closes the current socket.");
    Tamgusocket::AddMethod(global, "blocking", &Tamgusocket::MethodBlock, P_ONE, "blocking(bool flag): if 'flag' is true the socket works in 'blocking' mode otherwise in 'non blocking' mode");
    Tamgusocket::AddMethod(global, "settimeout", &Tamgusocket::MethodTimeout, P_ONE, "settimeout(int t): Set a time out of 't' seconds on the socket");
    Tamgusocket::AddMethod(global, "gethostname", &Tamgusocket::MethodGethostname, P_NONE, "gethostname(): return the current host name");
    Tamgusocket::AddMethod(global, "port", &Tamgusocket::MethodPort, P_NONE, "port(): return the current port number");
    Tamgusocket::AddMethod(global, "getpeername", &Tamgusocket::MethodGetpeername, P_ONE, "getpeername(int num): return the current peer name on the socket 'num'");

    Tamgusocket* s = new Tamgusocket(global);
    s->root = true;
    global->newInstance[Tamgusocket::idtype] = s;
    global->RecordMethods(Tamgusocket::idtype,Tamgusocket::exported);

    #ifdef WIN32
    if (wsainit) {
        WSAStartup(MAKEWORD(2, 0), &WSAData);
        wsainit = false;
    }
    #endif

    return true;
}

Tamgusocket::Tamgusocket(TamguGlobal* g, Tamgu* parent) : TamguObject(g, parent) {
    
    root = false;
    //Do not forget your variable initialisation
    hp = NULL;
    sock = -1;
    idclient = 0;
    blocking = true;
    timeout = false;
    g->RecordInTrackerProtected(this);
    sockets.push_back(this);
    isockets.push_back(idtracker);
}


Tamgusocket::~Tamgusocket() {
    if (root) {
        #ifdef WIN32
        WSACleanup();
        wsainit = true;
        #endif
        for (size_t i = 0; i < isockets.size(); i++) {
            if (globalTamgu->Checktracker(sockets[i], isockets[i]))
                sockets[i]->Close();
        }

        sockets.clear();
        isockets.clear();

    }
}

Tamgu* Tamgusocket::Put(Tamgu* index, Tamgu* value, short idthread) {
    return this;
}

Tamgu* Tamgusocket::Get(Tamgu* context, Tamgu* index, short idthread) {
    return this;
}


Tamgu* Tamgusocket::MethodCreateServer(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //TamguThreadLock _lock(general);
    //In our example, we have only two parameters
    if (hp != NULL)
        return globalTamgu->Returnerror("SOCKET(831): Server already launched on this socket", idthread);
    //0 is the first parameter and so on...
    Tamgu* kport;
    Tamgu* knbclients;
    if (callfunc->Size() == 2) {
        kport = callfunc->Evaluate(0, aNULL, idthread);
        gethostname(servername, MAXHOSTNAME);
        knbclients = callfunc->Evaluate(1, aNULL, idthread);
    }
    else {
        Tamgu* kserver = callfunc->Evaluate(0, aNULL, idthread);
        string s = kserver->String();
        if (s.size() >= MAXHOSTNAME)
            return globalTamgu->Returnerror("SOCKET(850): Wrong server name", idthread);
        strcpy(servername, (char*)s.c_str());
        kport = callfunc->Evaluate(1, aNULL, idthread);
        knbclients = callfunc->Evaluate(2, aNULL, idthread);
    }
    int nbclients = knbclients->Integer();
    port = kport->Integer();
    Tamgu* ret = CreateSocket(idthread);
    if (ret != aTRUE)
        return ret;
    #ifndef WIN32
    int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        string errmessage = "SOCKET(872): Error on reuse addess";
        errmessage += Msgerror();
        return globalTamgu->Returnerror(errmessage, idthread);
    }
    #endif
    servAddr.sin_addr.s_addr = INADDR_ANY;
    if (BIND(sock, (struct sockaddr*)&servAddr, len) < 0) {
        string errmessage = "SOCKET(853): Error on bind ";
        errmessage += Msgerror();
        return globalTamgu->Returnerror(errmessage, idthread);
    }
    if (listen(sock, nbclients) < 0) {
        string errmessage = "SOCKET(854): Error on listen ";
        errmessage += Msgerror();
        return globalTamgu->Returnerror(errmessage, idthread);
    }
    server = true;
    return aTRUE;
}

Tamgu* Tamgusocket::MethodCreateClient(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //TamguThreadLock _lock(general);
    //In our example, we have only two parameters
    if (hp != NULL)
        return globalTamgu->Returnerror("SOCKET(831): Server already launched on this socket", idthread);
    //0 is the first parameter and so on...
    Tamgu* kserver = callfunc->Evaluate(0, aNULL, idthread);
    Tamgu* kport = callfunc->Evaluate(1, aNULL, idthread);
    port = kport->Integer();
    strcpy(servername, (char*)kserver->String().c_str());
    Tamgu* ret = CreateSocket(idthread);
    if (ret != aTRUE)
        return ret;
    server = false;
    if (connect(sock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        string errmessage = "SOCKET(857): Error on connection ";
        errmessage += Msgerror();
        return globalTamgu->Returnerror(errmessage, idthread);
    }
    return aTRUE;
}

Tamgu* Tamgusocket::MethodWait(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //TamguThreadLock _lock(waiting);
    struct sockaddr_in cliAddr;
    SOCKET socketclient = -1;
    if (server == true) {
        if ((socketclient = accept(sock, (struct sockaddr*)&cliAddr, &len)) < 0) {
            string errmessage = "SOCKET(855): Error on read ";
            errmessage += Msgerror();
            return globalTamgu->Returnerror(errmessage, idthread);
        }

        if (globalTamgu->Error(idthread) == true)
            return globalTamgu->Errorobject(idthread);

        socketclients[socketclient] = true;
        return globalTamgu->Provideint(socketclient);
    }
    return aMINUSONE;
}

Tamgu* Tamgusocket::MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    short ssz;
    string res;
    SOCKET currentsock;
    int socketclient;
    if (server == true) {
        if (callfunc == NULL || callfunc->Size() == 0)
            return globalTamgu->Returnerror("SOCKET(820): Missing parameter in read", idthread);
        socketclient = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (socketclient == SOCKET_ERROR)
            return globalTamgu->Returnerror("SOCKET(824): Non connected socket", idthread);
        currentsock = socketclient;
    }
    else
        currentsock = sock;
    if (currentsock == SOCKET_ERROR)
        return globalTamgu->Returnerror("SOCKET(858): No client connected", idthread);
    char inputstr[MAXSIZEINPUT + 1];
    int nbcharread = 0;
    int nbloc;
    while (nbcharread < POSSIGNATURE) {
        if (TestTimeOutRead(idthread, currentsock) == false)
            return aNULL;
        nbloc = readsock(currentsock, inputstr + nbcharread, POSSIGNATURE - nbcharread);
        if (validstream(nbloc) == false) {
            string errmessage = "SOCKET(861): Error on read";
            errmessage += Msgerror();
            return globalTamgu->Returnerror(errmessage, idthread);
        }
        nbcharread += nbloc;
    }
    inputstr[POSSIGNATURE] = 0;
    ssz = XConvert(inputstr + 1, POSSIGNATURE - 1);
    int maxtoread;
    //cout<<"Reading:"<<ssz<<":"<<inputstr<<endl;
    while (ssz > 0) {
        inputstr[0] = 0;
        if (TestTimeOutRead(idthread, currentsock) == false)
            return aNULL;
        maxtoread = ssz;
        if (maxtoread > MAXSIZEINPUT)
            maxtoread = MAXSIZEINPUT;
        nbcharread = readsock(currentsock, inputstr, maxtoread);
        if (validstream(nbcharread) == false) {
            string errmessage = "SOCKET(861): Error on read";
            errmessage += Msgerror();
            return globalTamgu->Returnerror(errmessage, idthread);
        }
        inputstr[nbcharread] = 0;
        ssz -= nbcharread;
        res += inputstr;
    }
    return globalTamgu->Providestring(res);
}

Tamgu* Tamgusocket::MethodWrite(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return Write("N", contextualpattern, idthread, callfunc);
}

Tamgu* Tamgusocket::MethodReadRaw(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    string res;
    int nb = -1;
    SOCKET currentsock;
    //TamguThreadLock _lock(reading);
    int nbbytes = MAXSIZEINPUT;
    bool decrement = false;
    if (server == true) {
        if (callfunc->Size() == 0)
            return globalTamgu->Returnerror("SOCKET(821): Missing parameter in receive", idthread);
        currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (callfunc->Size() == 2) {
            nbbytes = callfunc->Evaluate(1, aNULL, idthread)->Integer();
            decrement = true;
        }
        if (currentsock == SOCKET_ERROR)
            return globalTamgu->Returnerror("SOCKET(824): Non connected socket", idthread);
    }
    else {
        if (callfunc->Size() == 2)
            return globalTamgu->Returnerror("SOCKET(823): Too many parameter in receive", idthread);
        currentsock = sock;
        if (callfunc->Size() == 1) {
            nbbytes = callfunc->Evaluate(0, aNULL, idthread)->Integer();
            decrement = true;
        }
    }
    if (currentsock == SOCKET_ERROR)
        return globalTamgu->Returnerror("SOCKET(858): No client connected", idthread);
    if (TestTimeOutRead(idthread, currentsock) == false)
        return aNULL;
    char inputstr[MAXSIZEINPUT + 1];
    if (nbbytes >= MAXSIZEINPUT)
        nb = readsock(currentsock, inputstr, MAXSIZEINPUT);
    else
        nb = readsock(currentsock, inputstr, nbbytes);
    if (validstream(nb) == false) {						//In the case of non blocking socket, we simply return the empty string
        string errmessage = "SOCKET(860): Error on RECEIVE";
        errmessage += Msgerror();
        return globalTamgu->Returnerror(errmessage, idthread);
    }
    if (nb != 0) {
        inputstr[nb] = 0;
        res += inputstr;
    }
    if (decrement)
        nbbytes -= nb;
    while (nb != 0 && nbbytes > 0) {
        if (TestTimeOutRead(idthread, currentsock) == false)
            return aNULL;
        if (nbbytes >= MAXSIZEINPUT)
            nb = readsock(currentsock, inputstr, MAXSIZEINPUT);
        else
            nb = readsock(currentsock, inputstr, nbbytes);
        if (decrement)
            nbbytes -= nb;
        if (nb != 0) {
            inputstr[nb] = 0;
            res += inputstr;
        }
        if (validstream(nb) == false) {
            string errmessage = "SOCKET(860): Error on RECEIVE";
            errmessage += Msgerror();
            return globalTamgu->Returnerror(errmessage, idthread);
        }
    }
    return globalTamgu->Providestring(res);
}

Tamgu* Tamgusocket::MethodGet(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    int nb = -1;
    SOCKET currentsock;
    if (server == true) {
        if (callfunc->Size() == 0)
            return globalTamgu->Returnerror("SOCKET(821): Missing parameter in receive", idthread);
        currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (currentsock == SOCKET_ERROR)
            return globalTamgu->Returnerror("SOCKET(824): Non connected socket", idthread);
    }
    else
        currentsock = sock;
    if (currentsock == SOCKET_ERROR)
        return globalTamgu->Returnerror("SOCKET(858): No client connected", idthread);
    if (TestTimeOutRead(idthread, currentsock) == false)
        return aNULL;
    char rd[] = { 0, 0, 0 };
    nb = readsock(currentsock, rd, 1);
    if (validstream(nb) == false) {						//In the case of non blocking socket, we simply return the empty string
        string errmessage = "SOCKET(860): Error on RECEIVE ";
        errmessage += Msgerror();
        return globalTamgu->Returnerror(errmessage, idthread);
    }
    if (!nb)
        return aNULL;
    if (contextualpattern->Type() == a_byte) {
        ((Tamgubyte*)contextualpattern)->value = rd[0];
        return contextualpattern;
    }
    return new Tamgubyte(rd[0]);
}

Tamgu* Tamgusocket::MethodWriteRaw(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    SOCKET currentsock = sock;
    int first = 0;
    if (server == true) {
        first = 1;
        if (callfunc->Size() <= 1)
            return globalTamgu->Returnerror("SOCKET(827): Missing parameter in send", idthread);
        currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (currentsock == SOCKET_ERROR)
            return globalTamgu->Returnerror("SOCKET(858): No client connected", idthread);
    }
    if (TestTimeOutWrite(idthread, currentsock) == false)
        return aNULL;
    Tamgu* kvalue;
    string strc;
    char* buff;
    int sz;
    for (int i = first; i<callfunc->Size(); i++) {
        kvalue = callfunc->Evaluate(i, aNULL, idthread);
        strc = kvalue->String();
        buff = STR(strc);
        sz = strc.size();
        while (sz>0) {
            int nbsz = sz;
            if (nbsz > MAXSIZEINPUT)
                nbsz = MAXSIZEINPUT;
            if (writesock(currentsock, buff, nbsz) < 0) {
                string errmessage = "SOCKET(859): Error sending";
                errmessage += Msgerror();
                return globalTamgu->Returnerror(errmessage, idthread);
            }
            buff += MAXSIZEINPUT;
            sz -= MAXSIZEINPUT;
        }
    }
    return aTRUE;
}

Tamgu* Tamgusocket::MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //TamguThreadLock _lock(general);
    if (server == true) {
        if (hp == NULL || sock == SOCKET_ERROR)
            return aFALSE;
        //We clean a client connection
        if (callfunc->Size() == 1) {
            int socketclient = callfunc->Evaluate(0, aNULL, idthread)->Integer();
            if (socketclient == SOCKET_ERROR)
                return globalTamgu->Returnerror("SOCKET(824): Non connected socket", idthread);
            closesock(socketclient);
            if (socketclients.find(socketclient) != socketclients.end())
                socketclients.erase(socketclient);
            return aTRUE;
        }
    }
    //otherwise we clean the current socket
    if (hp != NULL && sock != SOCKET_ERROR)
        closesock(sock);
    hp = NULL;
    return aTRUE;
}

Tamgu* Tamgusocket::MethodBlock(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //TamguThreadLock _lock(general);
    blocking = callfunc->Evaluate(0, aNULL, idthread)->Boolean();
    #ifdef WIN32
    u_long iMode = 0;
    if (blocking == false)
        iMode = 1;
    ioctlsocket(sock, FIONBIO, &iMode);
    #else
    int flags;
    flags = fcntl(sock, F_GETFL, 0);
    if (blocking == false)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);
    #endif
    return aTRUE;
}

Tamgu* Tamgusocket::MethodTimeout(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    //TamguThreadLock _lock(general);
    //In our example, we have only two parameters
    Tamgu* ktimeout = callfunc->Evaluate(0, aNULL, idthread);
    v_timeout = ktimeout->Integer();
    if (v_timeout == -1) {
        timeout = false;
        return aTRUE;
    }
    //We create our timeout
    tv.tv_sec = v_timeout;
    tv.tv_usec = 0;
    timeout = true;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    return aTRUE;
}

Tamgu* Tamgusocket::MethodGethostname(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    gethostname(servername, MAXHOSTNAME);           /* who are we? */
    return globalTamgu->Providestring(servername);
}

Tamgu* Tamgusocket::MethodPort(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    return globalTamgu->Provideint(port);
}

Tamgu* Tamgusocket::MethodGetpeername(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
    if (server == true) {
        int socketclient = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (socketclient == SOCKET_ERROR)
            return globalTamgu->Returnerror("SOCKET(824): Non connected socket", idthread);
        struct sockaddr cliAddr;
        #ifdef WIN32
        int len = sizeof(struct sockaddr);
        #else
        socklen_t len = sizeof(struct sockaddr);
        #endif
        getpeername(socketclient, &cliAddr, &len);
        struct sockaddr_in* client = (struct sockaddr_in*)&cliAddr;
        char* nm = inet_ntoa(client->sin_addr);
        Tamgumap* kmap = globalTamgu->Providemap();
        string key1 = "port";
        kmap->Push(key1, globalTamgu->Provideint(client->sin_port));
        string key2 = "address";
        string nms = nm;
        kmap->Push(key2, globalTamgu->Providestring(nms));
        return kmap;
    }
    return globalTamgu->Returnerror("SOCKET(852): You cannot use GETPEERNAME on the client side", idthread);
}


Tamgu* Tamgusocket::CreateSocket(short idthread) {

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == SOCKET_ERROR)
        return globalTamgu->Returnerror("SOCKET(856): Socket error", idthread);
    #ifdef WIN32
    memset((char *)&servAddr, 0, sizeof(servAddr));
    #else
    bzero((char *)&servAddr, sizeof(servAddr));
    #endif

    if (checkipaddres(servername) == false) {
        hp = gethostbyname(servername);                  /* get our address info */
        if (hp == NULL)                             /* we don't exist !? */
        return globalTamgu->Returnerror("SOCKET(851): Cannot find host by name on this machine", idthread);
        memcpy((char *)&servAddr.sin_addr, hp->h_addr, hp->h_length);     /* set address */
        servAddr.sin_family = hp->h_addrtype;              /* this is our host address */
        /* this is our port number */
    }
    else {
        servAddr.sin_addr.s_addr = inet_addr(servername);
        servAddr.sin_family = AF_INET;
    }

    servAddr.sin_port = htons(port);

    len = sizeof(servAddr);


    return aTRUE;
}

bool Tamgusocket::TestTimeOutRead(short idthread, SOCKET currentsock) {
    if (timeout == true) {
        tv.tv_sec = v_timeout;
        tv.tv_usec = 0;
        FD_ZERO(&readfds);
        //Then we insert our socket in it
        FD_CLR(currentsock, &readfds);
        FD_SET(currentsock, &readfds);
        //int sl=select(currentsock+1,&readfds,NULL,NULL, &tv);
        int sl = select(FD_SETSIZE, &readfds, NULL, NULL, &tv);
        if (sl < 0) {
            string errmessage = "SOCKET(888): Reading connection refused ";
            errmessage += Msgerror();
            globalTamgu->Returnerror(errmessage, idthread);
            return false;
        }
        sl = FD_ISSET(currentsock, &readfds);
        if (sl == 0) {
            string errmessage = "SOCKET(830): Time out ";
            //errmessage+=Msgerror();
            globalTamgu->Returnerror(errmessage, idthread);
            return false;
        }
    }
    return true;
}

bool Tamgusocket::TestTimeOutWrite(short idthread, SOCKET currentsock) {
    if (timeout == true) {
        tv.tv_sec = v_timeout;
        tv.tv_usec = 0;
        FD_ZERO(&writefds);
        //Then we insert our socket in it
        FD_CLR(currentsock, &writefds);
        FD_SET(currentsock, &writefds);
        //int sl=select(currentsock+1,NULL,&writefds,NULL,&tv);
        int sl = select(FD_SETSIZE, NULL, &writefds, NULL, &tv);
        if (sl < 0) {
            string errmessage = "SOCKET(889): Writing connection refused ";
            errmessage += Msgerror();
            globalTamgu->Returnerror(errmessage, idthread);
            return false;
        }
        sl = FD_ISSET(currentsock, &writefds);
        if (sl == 0) {
            string errmessage = "SOCKET(830): Time out ";
            //errmessage+=Msgerror();
            globalTamgu->Returnerror(errmessage, idthread);
            return false;
        }
    }
    return true;
}

Tamgu* Tamgusocket::Write(string act, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

    Tamgu* kvalue;
    short sz;
    string strc;
    int first = 0;
    char padding[POSSIGNATURE + 1];

    memset(padding, 0, POSSIGNATURE + 1);

    SOCKET currentsock = sock;

    if (server == true) {
        first = 1;
        if (callfunc->Size() <= 1)
            return globalTamgu->Returnerror("SOCKET(822): Missing parameter in write", idthread);

        currentsock = callfunc->Evaluate(0, aNULL, idthread)->Integer();
        if (currentsock == SOCKET_ERROR)
            return globalTamgu->Returnerror("SOCKET(858): No client connected", idthread);
    }

    char* buff;
    bool written = false;
    for (int i = first; i < callfunc->Size(); i++) {
        kvalue = callfunc->Evaluate(i, aNULL, idthread);
        if (kvalue->isError()) {
            strc = "&&ERROR&&:";
            strc += kvalue->String();
        }
        else
            strc = kvalue->String();

        sz = strc.size();
        if (sz>0) {
            written = true;
            sprintf_s(padding + 1, POSSIGNATURE, FORMATSIGNATURE, sz);
            padding[0] = act[0];
            //cout<<"Writing:"<<sz<<":"<<padding<<endl;

            if (TestTimeOutWrite(idthread, currentsock) == false)
                return aNULL;

            if (writesock(currentsock, padding, POSSIGNATURE)<0) {
                string errmessage = "SOCKET(859): Error sending";
                errmessage += Msgerror();
                return globalTamgu->Returnerror(errmessage, idthread);
            }
            buff = STR(strc);
            while (sz>0) {
                int nbsz = sz;
                if (nbsz > MAXSIZEINPUT)
                    nbsz = MAXSIZEINPUT;

                if (TestTimeOutWrite(idthread, currentsock) == false)
                    return aNULL;

                if (writesock(currentsock, buff, nbsz) < 0) {
                    string errmessage = "SOCKET(859): Error sending";
                    errmessage += Msgerror();
                    return globalTamgu->Returnerror(errmessage, idthread);
                }
                buff += MAXSIZEINPUT;
                sz -= MAXSIZEINPUT;
            }
        }
    }

    if (written == false) {
        //Empty strings... We still write it...
        sprintf_s(padding + 1, POSSIGNATURE, FORMATSIGNATURE, 0);
        padding[0] = act[0];
        if (TestTimeOutWrite(idthread, currentsock) == false)
            return aNULL;

        if (writesock(currentsock, padding, POSSIGNATURE) < 0) {
            string errmessage = "SOCKET(859): Error sending";
            errmessage += Msgerror();
            return globalTamgu->Returnerror(errmessage, idthread);
        }
    }
    return aTRUE;
}
