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
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	string lastcommand;
    bool getcharhasbeenused;
    bool mouseenabled;
	Tamgu* function;

#ifdef WIN32
	bool tracking;
#else
    uchar vstart;
    uchar vstop;
    uchar vsusp;
#endif
    
    //---------------------------------------------------------------------------------------------------------------------
    Tamgusys(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        getcharhasbeenused = false;
        mouseenabled = false;
		function = NULL;
#ifdef WIN32
        tracking = false;
#endif
    }

    Tamgusys() {
        //Do not forget your variable initialisation
        getcharhasbeenused = false;
        mouseenabled = false;
		function = NULL;
        #ifdef WIN32
                tracking = false;
        #endif
    }

    ~Tamgusys();
    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgusys::idtype;
    }

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "sys";
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

    void Reset();

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
    Tamgu* MethodPopen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodGETCH(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSCROLLMARGIN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodDELETECHAR(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodUP(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodDOWN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRIGHT(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodLEFT(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodNEXT_LINE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPREVIOUS_LINE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCOLUMN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodROW_COLUMN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodHOME(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodHOR_VERT(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCLEARSCREEN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCLEAR(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodERASE_LINE(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSCROLL_UP(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodSCROLL_DOWN(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCLS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodFGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodBGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRGBFGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodRGBBGCOLORS(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodCoordinates(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodScreenHasResized(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCursorPosition(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodisEscapeSequence(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodInitMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodCloseMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodIsActionMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPositionMouse(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPositionScrollingUp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodPositionScrollingDown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodClickFirstMouseDown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodClickSecondMouseDown(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodClickMouseUp(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodMouseTrack(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    Tamgu* MethodShowCursor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodResizeCallBack(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
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

    void Setstring(string& v, short idthread) {
        v = lastcommand;
    }

};

class TamguConstsys : public Tamgusys {
public:

	TamguConstsys(TamguGlobal* g) : Tamgusys(g) {}

	void Resetreference(long inc) {}
	void Release() {}

};
#endif



