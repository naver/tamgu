/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguwinput.h
 Date       : 2017/07/22
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguwinput_h
#define tamguwinput_h

#include "tamguwidget.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguwinput;
//This typedef defines a type "winputMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguwinput::*winputMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguwinput : public Tamguwidget {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<winputMethod> methods;
    
    

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Fl_Input* input;
	char txt[1024];
	

	bool multiline;
	string buf;

    //---------------------------------------------------------------------------------------------------------------------
	Tamguwinput(TamguGlobal* g, Tamgu* parent = NULL) : Tamguwidget(g, parent) {
        //Do not forget your variable initialisation
		input = NULL;
		multiline = false;
		
	}

	Fl_Widget* Widget() {
		return input;
	}

	void itemclose() {
		if (function != NULL)
			return;
		if (input != NULL)
			delete input;
		input = NULL;
		multiline = false;
		clean();
	}

    void ResetWidget() {
        if (input != NULL)
            delete input;
        input = NULL;
    }

    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* index, short idthread);


    short Type() {
        return Tamguwinput::idtype;
    }

    string Typename() {
        return "winput";
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamguwinput* a = new Tamguwinput(globalTamgu);
		a->function = f;
		return a;
    }

	TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, winputMethod func, unsigned long arity, string infos);
    
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



	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInsert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodSelection(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodColor(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodWord(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalTamgu->Error(idthread))
			return globalTamgu->Errorobject(idthread);

		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
		
    }

    void Clean() {
        //Your cleaning code
    }

	long Size() {
		return String().size();
	}


	long Integer() {
		
		return String().size();
	}
	
	double Float() {
		
		return String().size();
	}
	
	string String() {
        if (input == NULL)
            return "";
		return input->value();
	}

	
	bool Boolean() {
        if (input)
            return true;
        return false;
	}	
};

#endif








