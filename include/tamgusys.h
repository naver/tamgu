/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusys.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgusys_h
#define tamgusys_h

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgusys;
//This typedef defines a type "sysMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgusys::*sysMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgusys : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<sysMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging bin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	string lastcommand;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgusys(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        
    }

    Tamgusys() {
        //Do not forget your variable initialisation
        
    }

    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Get(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgusys::idtype;
    }

    

    static void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "sys";
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
        if (forced || !protect || reference)
            return new  Tamgusys;
        return this;
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
		return new Tamgusys;
    }

    TamguIteration* Newiteration(bool direction) {
		return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, sysMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

	void Methods(Tamgu* v) {

		for (auto& it : infomethods)
			v->storevalue(it.first);
	}


	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}


    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

	Tamgu* MethodCommand(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodisDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodCreateDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodListDirectory(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodRealPath(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFileInfo(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEnv(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear(TamguGlobal* g) {
        //To set a variable back to empty
    }

    

    string String() {       
        return lastcommand;
    }

};

class TamguConstsys : public Tamgusys {
public:

	TamguConstsys(TamguGlobal* g) : Tamgusys(g) {}

	void Resetreference(long inc) {}
	void Release() {}

};
#endif



