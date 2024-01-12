/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgudate.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgudate_h
#define tamgudate_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgudate;
//This typedef defines a type "dateMethod", which expose the typical parameters of a new a method implementation
typedef Tamgu* (Tamgudate::*dateMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgudate : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map
	static Exchanging basebin_hash<dateMethod> methods;
	
	

	static Exchanging short idtype;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	time_t value;

	//---------------------------------------------------------------------------------------------------------------------
	Tamgudate(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
		//Do not forget your variable initialisation
		value = NULL;
	}

    Tamgudate(time_t v);

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);

	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


	short Type() {
		return Tamgudate::idtype;
	}

	

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
		return "date";
	}

	

	

	bool isBoolean() {
		return false;
	}

	bool isFrame() {
		return false;
	}

	Tamgu* Atom(bool forced = false) {
		if (forced || !protect || reference)
			return new  Tamgudate(value);
		return this;
	}

	bool duplicateForCall() {
		return false;
	}


	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in aexportedmethods... See MethodInitialization below
	bool isDeclared(short n) {
        return methods.check(n);
	}

	Tamgu* Newinstance(short, Tamgu* f = NULL) {
		return new Tamgudate((time_t)0);
	}

	TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
	}

	static void AddMethod(TamguGlobal* g, string name, dateMethod func, unsigned long arity, string infos);
	static bool InitialisationModule(TamguGlobal* g, string version);

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
	Tamgu* MethodInitial(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodYear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMonth(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodYearday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWeekday(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodHour(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMinute(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFormat(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSecond(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new a methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

	void Clear() {
		//To set a variable back to empty
	}

	
    Tamgu* set_the_date(Tamguivector* kinit);
    Tamgu* setstringdate(string& v, short idthread);
    
	string String();
    void Setstring(string& s, short idthread);
    void Setstring(wstring& s, short idthread);
    
	//wstring UString() {
	//   
	//  return L"";
	//	}

	long Integer() {

		return value;
	}
	double Float() {

		return value;
	}
	BLONG Long() {

		return value;
	}

	bool Boolean() {
        return value;
	}

	Tamgu* plus(Tamgu* b, bool autoself) {
		if (autoself) {
			value += b->Integer();
			return this;
		}

		time_t i = value + b->Integer();
		return new Tamgudate(i);
	}

	Tamgu* minus(Tamgu* b, bool autoself) {
		if (autoself) {
			value = difftime(value, b->Integer());
			return this;
		}
		time_t i = difftime(value, b->Integer());
		return new Tamgudate(i);
	}

	//Basic operations

	Tamgu* less(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value<a->Integer())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* more(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value>a->Integer())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* same(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
        return booleantamgu[value == a->asInteger()];
	}
	Tamgu* different(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aTRUE;
#endif
		if (value != a->asInteger())
			return aTRUE;
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value <= a->asInteger())
			return aTRUE;
		return aFALSE;
	}
	Tamgu* moreequal(Tamgu* a) {
#ifdef TAMGUSTRICTCOMPARISON
        if (!a->isNumber())
            return aFALSE;
#endif
		if (value >= a->asInteger())
			return aTRUE;
		return aFALSE;
	}
	long Size() {
		return value;
	}
};

#endif




