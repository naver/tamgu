/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgugrammar.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgugrammar_h
#define tamgugrammar_h
#include "tamgu.h"
#include "instructions.h"

class gramstate;
class Tamgugrammar;
class GrammarBaseAutomaton;
class GrammarAutomaton;

class gramarc {
public:
	gramstate* state;
	gramstate* from;
	wstring wlabel;
	short idrule;
	unsigned char status;

	gramarc();

	virtual char test(wstring& c);
	virtual long test(wstring& l, wstring& r, long i);
	char test(TAMGUCHAR c);
};

class gramstate {
public:
	unsigned char status;
	vector<gramarc*> arcs;

	gramstate() {
		status = 0;
	}

	char comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, vector<wstring>& labels, long& i, Tamgu* m, bool);
	char compare(short idthread, GrammarBaseAutomaton* gram, vector<wstring>& labels, long& i, Tamgu* v, bool);


	char comparemap(short idthread, wstring& lkey, GrammarBaseAutomaton* gram, wstring& labels, long& i, Tamgu* m, bool);
	char compare(short idthread, GrammarBaseAutomaton* gram, wstring& labels, long& i, Tamgu* v, bool);
};

class GrammarBaseAutomaton {
public:
	

	vector<gramstate*> states;
	vector<gramarc*> arcs;
	vector<gramstate*> rules;

	Tamgu* function;
	unsigned char skipblanks;

	GrammarBaseAutomaton() {		
		function = NULL;
		skipblanks = 0;
	}

	~GrammarBaseAutomaton() {
		long i;

		for (i = 0; i < states.size(); i++)
			delete states[i];

		for (i = 0; i < arcs.size(); i++)
			delete arcs[i];
	}
	bool callfunction(short idthread, string&, Tamgu*, long pos);
	bool callfunction(short idthread, wstring&, Tamgu*, long pos);
	bool callfunction(Tamgu* func, short idthread, Tamgu*, long pos);

    unsigned long CallBackArity() {
        return P_TWO;
    }

};

class subgrammar : public gramarc {
public:

	gramstate substate;
	GrammarBaseAutomaton gram;

	subgrammar(GrammarBaseAutomaton* ga)  {}

	char test(wstring& c);
	long test(wstring& l, wstring& r, long i);
};


class GrammarAutomaton : public GrammarBaseAutomaton {
public:

	hmap<wstring, long> dictionary;
	hmap<long, wstring> rdictionary;

	Tamgu* Apply(short, Tamgu* res, wstring& labels, bool asstring);
	Tamgu* Apply(short, Tamgu* res, vector<wstring>& labels, bool asstring);
};


//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgugrammar;
//This typedef defines a type "grammarMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgugrammar::*grammarMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgugrammar : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<grammarMethod> methods;
	static Exchanging hmap<string, string> infomethods;
	static Exchanging bin_hash<unsigned long> exported;

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	
	GrammarAutomaton* automaton;
	Tamgu* function;
	unsigned char skipblanks;
	bool clean;


	//---------------------------------------------------------------------------------------------------------------------
	Tamgugrammar(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
		//Do not forget your variable initialisation
		automaton = NULL;
		clean = false;
		skipblanks = 0;
	}

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

	Tamgu* in(Tamgu* context, Tamgu* a, short idthread);

	short Type() {
		return Tamgugrammar::idtype;
	}

	

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "grammar";
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

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
	}

	void Addfunction(Tamgu* a) {
		function = a;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in atanexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
		if (exported.find(n) != exported.end())
			return true;
		return false;
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamgugrammar* a = new Tamgugrammar(NULL);
		a->function = f;
		return a;
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, grammarMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	//Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodApply(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		Locking _lock(this);
		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
	}

	//------------------------------------------------------------------------------------------------------------------------
	Tamgu* Apply(short, Tamgu* v, vector<wstring>& labels, bool asstring);
	Tamgu* Apply(short, Tamgu* v, wstring& labels, bool asstring);
	Tamgu* Loader(string& v, short idthread);

	void Clear() {
		//To set a variable back to empty
	}

	

};

#endif




