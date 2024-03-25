/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguxmldoc.h
 Date       : 2017/09/01
 Purpose    : TAMGU API to xmldoc nodes 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamguxmldoc_h
#define tamguxmldoc_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguxmldoc;
//This typedef defines a type "xmldocMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguxmldoc::*xmldocMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguxmldoc : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<xmldocMethod> methods;
    
    

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	
	string filename;
	char fname[1024];

	xmlSAXHandler* sax;
	xmlDocPtr doc;
	Tamgu* function;
	Tamgu* object;

	Tamgu* functionend;
	Tamgu* objectend;
	long idnode;
	bool stop;

	xmlXPathContextPtr xpathcontext;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguxmldoc(TamguGlobal* g, Tamgu* parent) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		doc = NULL;
		object = aNULL;
		function = NULL;
		functionend = NULL;
		objectend = aNULL;
		sax = NULL;
		idnode = 1;
		xpathcontext = NULL;
		stop = false;
	}

    Tamguxmldoc(xmlDocPtr v) : TamguObject() {
        //Do not forget your variable initialisation
		doc = v;
		function = NULL;
		object = aNULL;
		functionend = NULL;
		objectend = aNULL;
		sax = NULL;
		idnode = 1;
		xpathcontext = NULL;
		stop = false;
	}

    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);

    unsigned long CallBackArity() {
        return P_TWO;
    }


    short Type() {
        return Tamguxmldoc::idtype;
    }

    string Typename() {
        return "xmldoc";
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

	bool duplicateForCall() {
		return false;
	}

	void Addfunction(Tamgu* a) {
		function = a;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamguxmldoc* a = new Tamguxmldoc(NULL);
		a->function = f;
		return a;
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, xmldocMethod func, unsigned long arity, string infos, short returntype);
    static bool InitialisationModule(TamguGlobal* g, string version);

	
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


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

	Tamgu* MethodLoadXML(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOnClosing(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLoadXMLBuffer(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodXPath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSave(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodxmlEncoding(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSerialize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSerializeString(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//We are interested by the variable...
		object = callfunc->arguments[0];
		if (!object->isCallVariable())
			object = callfunc->Evaluate(0, aNULL, idthread);
		return aTRUE;
	}

	xmlNodePtr buildnode(xmlNodePtr r, Tamgu* e);
    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
		if (doc == NULL)
			return "";
		xmlChar* buff = NULL;
		int longueur;
		xmlDocDumpMemoryEnc(doc, &buff, &longueur, "UTF-8");
		string s((char*)buff);
		xmlFree(buff);
		return s;
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


};

#endif


