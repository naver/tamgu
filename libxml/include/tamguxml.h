/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguxml.h
 Date       : 2017/09/01
 Purpose    : TAMGU API to XML nodes 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */


#ifndef tamguxml_h
#define tamguxml_h

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguxml;
//This typedef defines a type "xmlMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguxml::*xmlMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguxml : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static basebin_hash<xmlMethod> methods;
	static short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	xmlNodePtr node;

	//---------------------------------------------------------------------------------------------------------------------
	Tamguxml(TamguGlobal* g, xmlNodePtr v)  {
		//Do not forget your variable initialisation
		node = v;
		g->RecordInTrackerProtected(this);
	}

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);

	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


	short Type() {
		return Tamguxml::idtype;
	}

	string Typename() {
		return "xml";
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

	Tamgu* Atom(bool forced = false) {
		return this;
	}

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
		return new Tamguxml(globalTamgu, NULL);
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, xmlMethod func, unsigned long arity, string infos);
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
	void TraverseXML(xmlNodePtr n, string& contenu, bool first) {
		if (n == NULL)
			return;
		if (n->content != NULL)
			contenu += (char*)n->content;
		TraverseXML(n->children, contenu, false);
		if (!first)
			TraverseXML(n->next, contenu, false);
	}


	Tamgu* MethodRoot(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDocument(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNext(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodUnlink(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDelete(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPrevious(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodParent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodChild(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodName(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodLine(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodId(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodxmlType(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodProperties(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodContent(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNamespace(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodNew(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* Methodxmlstring(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		return (this->*Tamguxml::methods.get(idname))(contextualpattern, idthread, callfunc);
	}

	void Clear() {
		//To set a variable back to empty
	}

	void Clean() {
		//Your cleaning code
	}

	string String() {
		Locking _lock(this);
		if (node == NULL)
			return "";
		return (char*)node->name;
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
		if (node == NULL && a->isNULL())
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


