/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgucurl.h
 Date       : 2017/09/01
 Purpose    : TAMGU API for cURL 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamgucurl_h
#define tamgucurl_h

#include <curl/curl.h>

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgucurl;
//This typedef defines a type "curlMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgucurl::*curlMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgucurl : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<curlMethod> methods;
    
    

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	Tamgu* function;
	Tamgu* object;
	string urlstr;
	CURL *curl;

	char* urlbuffer;
	int urlsize;

    //---------------------------------------------------------------------------------------------------------------------
	void curl_init(Tamgu* f) {
		curl = curl_easy_init();
		function = f;
		urlsize = 2048;
		urlbuffer = (char*)malloc(urlsize);
	}


    Tamgucurl(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		curl = NULL;
		function = NULL;
		object = aNULL;
		urlsize = 0;
		urlbuffer = NULL;
    }

	~Tamgucurl() {
		if (urlbuffer != NULL)
			free(urlbuffer);
		if (curl != NULL)
			curl_easy_cleanup(curl);
	}
	
	
     void Setidtype(TamguGlobal* global);
     void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[idtype])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[idtype].find(n) !=  globalTamgu->infomethods[idtype].end())
              return globalTamgu->infomethods[idtype][n];
             return "Unknown method";
	}

    unsigned long CallBackArity() {
        return P_TWO;
    }

    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgucurl::idtype;
    }

    string Typename() {
        return "curl";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
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
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamgucurl* a = new Tamgucurl(globalTamgu);
		a->curl_init(f);
		return a;
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

	void Addfunction(Tamgu* a) {
		function = a;
	}

    static void AddMethod(TamguGlobal* g, string name, curlMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//We are interested by the variable...
		object = callfunc->arguments[0];
		if (!object->isCallVariable())
			object = callfunc->Evaluate(0, aNULL, idthread);
		return aTRUE;
	}

	Tamgu* MethodProxy(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPWD(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodURL(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodExecute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOptions(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

	Tamgu* errormsg(short idthread, CURLcode res) {
		const char* errmsg = curl_easy_strerror(res);
		char ch[1024];
		sprintf_s(ch, 1024, "URL(%d): %s", res, errmsg);
		return globalTamgu->Returnerror(ch, idthread);
	}

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return "";
    }

    //wstring UString() {
    //   Locking _lock(this);
    //  return L"";
    //	}

    long Integer() {
        Locking _lock(this);
        return 0;
    }
    double Float() {
        Locking _lock(this);
        return 0;
    }
    BLONG Long() {
        Locking _lock(this);
        return 0;
    }

    bool Boolean() {
        Locking _lock(this);
        return false;
    }

    
    //Basic operations
    long Size() {
        Locking _lock(this);
        return 0;
    }
    
    Tamgu* andset(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* orset(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* xorset(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    
    Tamgu* plus(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* minus(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* multiply(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* divide(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* power(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    
    Tamgu* less(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }

    Tamgu* same(Tamgu* a) {
        Locking _lock(this);
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* different(Tamgu* a) {
        Locking _lock(this);
        if (same(a) == aFALSE)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* lessequal(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }

};

#endif


