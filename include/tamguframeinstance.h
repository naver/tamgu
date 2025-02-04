/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguframeinstance.h
 Date       : 2017/09/01
 Purpose    :  
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguframeinstance_h
#define tamguframeinstance_h

#include "instructions.h"
//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguframeinstance;
//This typedef defines a type "frameinstanceMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguframeinstance::*frameinstanceMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamguframeseeder : public TamguObject {
public:
    static Exchanging basebin_hash<frameinstanceMethod> methods;

    TamguFrame* frame;


    Tamguframeseeder(TamguFrame* f, TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
        frame = f;
    }

    Tamguframeseeder(TamguFrame* f) {
        frame = f;
    }
    
    short Name() {
        if (frame == NULL)
            return a_none;
        return frame->name;
    }
    
    short Type() {
        if (frame == NULL)
            return a_frame;
        return frame->Name();
    }
    
    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "frameinstance";
    }
    
    bool isFrame() {
        return true;
    }
    
    Tamgu* Frame() {
        return frame;
    }
    
    bool duplicateForCall() {
        return false;
    }

    virtual bool isDeclared(short id) {
        if (id == a_this)
            return true;
        else
            return frame->isDeclared(id);
    }
    
    virtual void Declare(short id, Tamgu* a) {
        frame->Declare(id, a);
    }
    
    virtual Tamgu* Declaration(short name) {
        return frame->Declaration(name);
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL);
    
    static void AddMethod(TamguGlobal* g, string name, frameinstanceMethod func, unsigned long arity, string infos);
    
    static bool InitialisationModule(TamguGlobal* g, string version);
    
    
    void Methods(Tamgu* v) {
            for (const auto& it : globalTamgu->infomethods[a_frameinstance])
                 v->storevalue(it.first);
      }

      string Info(string n) {
            if (globalTamgu->infomethods[a_frameinstance].find(n) !=  globalTamgu->infomethods[a_frameinstance].end())
              return globalTamgu->infomethods[a_frameinstance][n];
             return "Unknown method";
    }
    
    static Tamguframeseeder* RecordFrame(short name, TamguFrame* v, TamguGlobal* g);

};

class TamguframeBaseInstance : public TamguObject {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	//We associate the method pointers with their names in the linkedmethods map

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...

    TamguFrame* frame;
    bool setReference;
	//---------------------------------------------------------------------------------------------------------------------

    TamguframeBaseInstance(TamguFrame* f)  {
        frame = f;
        investigate |= is_frameinstance;
        investigate |= f->investigating;
        setReference = false;
    }

	//----------------------------------------------------------------------------------------------------------------------
	Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    virtual Tamgu* Getvariable(short id) {
        return aNOELEMENT;
    }
    
	void Variables(vector<short>& vars) {
		for (int it = 0; it < frame->vnames.last; it++)
			vars.push_back(frame->vnames[it]);
	}

    short Name() {
        return frame->name;
    }
    
    short Type() {
        return frame->Name();
    }
    
    string Typename() {
        return "frameinstance";
    }
    
    Tamgu* Frame() {
        return frame;
    }
    
    bool duplicateForCall() {
        return false;
    }

    Tamgu* Atom(bool forced = false) {
        return this;
    }
    
    bool isLong() {
        return (frame->numbers[a_long] != NULL);
    }
    
    bool isShort() {
        return (frame->numbers[a_short] != NULL);
    }

    bool isFloat() {
        return (frame->numbers[a_float] != NULL);
    }

    bool isInteger() {
        return (frame->numbers[a_int] != NULL);
    }

    bool isDecimal() {
        return (frame->numbers[a_decimal] != NULL);
    }

    bool isBoolean() {
        return (frame->numbers[a_boolean] != NULL);
    }

    bool frameHasSame() {
        return (frame->Declaration(a_same) != NULL);
    }
    
	//---------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
	//---------------------------------------------------------------------------------------------------------------------
	void MethodFinal() {
		Tamgu* func = frame->Declaration(a_final);
		if (func == NULL)
			return;
		
		func = Execute(func, globalTamgu->GetThreadid());

		func->Release();
	}

	long Setprotect() {
		return globalTamgu->RecordInTrackerProtected(this);
	}

	//---------------------------------------------------------------------------------------------------------------------
	Tamgu* Execute(Tamgu* body, VECTE<Tamgu*>& arguments, short idthread);
	Tamgu* Execute(Tamgu* body, short idthread);
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	Tamgu* MethodInitial(Tamgu* domain, short idthread, TamguCall* callfunc) {
		Tamgu* func = frame->Declaration(a_initial);
		while (func != NULL && func->Size() != callfunc->Size())
			func = func->Nextfunction();

		if (func == NULL) {
			if (callfunc->Size() == 0)
				return aTRUE;
			return globalTamgu->Returnerror(e_missing__initial_function02, idthread);
		}

		VECTE<Tamgu*> arguments(callfunc->arguments.last);
		Tamgu* a;
		long i;
        short idname;
		for (i = 0; i < callfunc->arguments.last; i++) {
            idname = callfunc->arguments[i]->Name();
            a = callfunc->arguments[i]->Eval(domain, aNULL, idthread);            
			a->Setreference();
			arguments.push_back(a);
		}

		func = Execute(func, arguments, idthread);
        
		for (i = 0; i < arguments.size(); i++)
			arguments[i]->Resetreference();
        
		return func;
	}

	Tamgu* MethodType(Tamgu* domain, short idthread, TamguCall* callfunc) {
		return globalTamgu->Providestring(globalTamgu->Getsymbol(frame->name));
	}

	//This is associated to all functions that have been declared within the frame...
	Tamgu* MethodMethod(Tamgu* domain, short idthread, TamguCall* callfunc) {
		Tamgu* func = frame->Declaration(callfunc->Name());
		if (func == NULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->CallMethod(callfunc->Name(), aNULL, idthread, callfunc);
            }
			stringstream message;
			message << e_unknown_method << globalTamgu->Getsymbol(callfunc->Name()) << "' in frame: '" << globalTamgu->Getsymbol(frame->Name()) << "'";
			return globalTamgu->Returnerror(message.str(), idthread);
		}

		VECTE<Tamgu*> arguments(callfunc->arguments.last);
		Tamgu* a;
		long i;

		for (i = 0; i < callfunc->arguments.last; i++) {
			a = callfunc->arguments[i]->Eval(domain, aNULL, idthread);
			a->Setreference();
			arguments.push_back(a);
		}

		func = Execute(func, arguments, idthread);

		for (i = 0; i < arguments.size(); i++)
			arguments[i]->Resetreference();


		return func;
	}

    //This is associated to all functions that have been declared within the frame...
    Tamgu* MethodTopMethod(Tamgu* domain, short idthread, TamguCall* callfunc, TamguFrame* topframe) {
        Tamgu* func = topframe->Declaration(callfunc->Name());
        if (func == NULL) {
            if (topframe->theextensionvar) {
                return Declaration(topframe->theextensionvar)->CallMethod(callfunc->Name(), aNULL, idthread, callfunc);
            }
            stringstream message;
            message << e_unknown_method << globalTamgu->Getsymbol(callfunc->Name()) << "' in frame: '" << globalTamgu->Getsymbol(frame->Name()) << "'";
            return globalTamgu->Returnerror(message.str(), idthread);
        }
        
        VECTE<Tamgu*> arguments(callfunc->arguments.last);
        Tamgu* a;
        long i;
        
        for (i = 0; i < callfunc->arguments.last; i++) {
            a = callfunc->arguments[i]->Eval(domain, aNULL, idthread);
            a->Setreference();
            arguments.push_back(a);
        }
        
        func = Execute(func, arguments, idthread);
        
        for (i = 0; i < arguments.size(); i++)
            arguments[i]->Resetreference();
        
        
        return func;
    }
    
    //A bit of explanation. If we try to execute a method belonging to a frame, from which the current
    //frame derives, then the reference to variables is local to the frame, in which the method was declared.
    //We then declare each potential target as a local variable for each top frames.
    void Pushframe(short idthread) {
        if (!frame->topframe)
            globalTamgu->Storevariable(idthread, frame->name, this);
        else {
            TamguFrame* fr = frame;
            while (fr != NULL) {
                globalTamgu->Storevariable(idthread, fr->name, this);
                fr = fr->topframe;
            }
        }
    }
    
    void Cleanframevariable(short idthread) {
        Tamgu* o;
        for (long i = 0; i < frame->variables.size(); i++) {
            o = frame->variables[i];
            globalTamgu->Removevariable(idthread, o->Name());
        }
    }
    
    void Popframe(short idthread) {
        if (!frame->topframe) {
            globalTamgu->Removevariable(idthread, frame->name);
        }
        else {
            TamguFrame* fr = frame;
            while (fr != NULL) {
                globalTamgu->Removevariable(idthread, fr->name);
                fr = fr->topframe;
            }
        }
    }
    
    //---------------------------------------------------------------------------------------------------------------------
	Tamgu* Callconversion(short id) {
		Tamgu* func = frame->Declaration(id);
		if (func == NULL)
			return aNULL;

		return Execute(func, globalTamgu->GetThreadid());
	}

	Tamgu* Callnumberconversion(short id) {
		Tamgu* func = frame->numbers[id];
		if (func == NULL)
			return aNULL;

		return Execute(func, globalTamgu->GetThreadid());
	}

	Tamgu* Calloperation(short id, Tamgu* a) {
		Tamgu* func = frame->Declaration(id);
		if (func == NULL)
			return aNULL;

		VECTE<Tamgu*> arguments;
		arguments.push_back(a);
		
        return Execute(func, arguments, globalTamgu->GetThreadid());
	}

	Tamgu* in(Tamgu* context, Tamgu* a, short idthread) {
		Tamgu* func = frame->Declaration(a_in);
        if (func == NULL) {
            if (frame->theextensionvar) {
                Tamgu* var = Declaration(frame->theextensionvar);
                if (var != NULL) {
                    return var->in(context, a, idthread);
                }
            }
			return aFALSE;
        }


		VECTE<Tamgu*> arguments;
		arguments.push_back(context);
		arguments.push_back(a);

		return Execute(func, arguments, idthread);
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
	//is returned by the Declaration method.
	Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		//In the case of a frame, either it is one of the predefined function
        switch (idname) {
            case a_initial:
                return MethodInitial(contextualpattern, idthread, callfunc);
            case a_frametype:
                return MethodType(contextualpattern, idthread, callfunc);
            default:
                //Or a basic function...
                return MethodMethod(contextualpattern, idthread, callfunc);
        }
	}
    
    Tamgu* CallTopMethod(short idname, TamguFrame* topframe, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        return MethodTopMethod(contextualpattern, idthread, callfunc, topframe);
    }

	void Clear() {
		//To set a variable back to empty
	}

    bool Stringpredicatekey(string& v) {
        if (!frameHasSame()) {
            Setstring(v, 0);
            return !v.empty();
        }
        return false;
    }

    bool Stringpredicatekeysecond(string& v) {
        if (!frameHasSame()) {
            Setstring(v, 0);
            return !v.empty();
        }
        return false;
    }

    bool Stringpredicatekeythird(string& v) {
        if (!frameHasSame()) {
            Setstring(v, 0);
            return !v.empty();
        }
        return false;
    }

	wstring UString() {
		Tamgu* res = Callconversion(a_ustring);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->UString();
            }
            return L"";
        }
		wstring s = res->UString();
		res->Release();
		return s;
	}

	string String() {
		Tamgu* res = Callconversion(a_string);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->String();
            }
            return "";
        }
		string s = res->String();
		res->Release();
		return s;
	}

    void Setstring(string& v, short idthread) {
        Tamgu* res = Callconversion(a_string);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Setstring(v, idthread);
            }
            return;
        }
        res->Setstring(v, idthread);
        res->Release();
    }
    
    void Setstring(wstring& v, short idthread) {
        Tamgu* res = Callconversion(a_ustring);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Setstring(v, idthread);
            }
            return;
        }
        res->Setstring(v, idthread);
        res->Release();
    }

    string JSonString() {
        Tamgu* res = Callconversion(a_string);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->String();
            }
            return "";
        }
        string s = res->JSonString();
        res->Release();
        return s;
    }

	short Short() {
		Tamgu* res = Callnumberconversion(a_short);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Short();
            }
            return 0;
        }
		short s = res->Short();
		res->Release();
		return s;
	}

	long Integer() {
		Tamgu* res = Callnumberconversion(a_int);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Integer();
            }
            return 0;
        }
		long s = res->Integer();
		res->Release();
		return s;
	}

	double Float() {
		Tamgu* res = Callnumberconversion(a_float);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Float();
            }
            return 0;
        }
		double s = res->Float();
		res->Release();
		return s;
	}

	BLONG Long() {
		Tamgu* res = Callnumberconversion(a_long);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Long();
            }
            return 0;
        }

		BLONG s = res->Long();
		res->Release();
		return s;
	}

	bool Boolean() {
		Tamgu* res = Callconversion(a_boolean);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Boolean();
            }
            return false;
        }

		bool s = res->Boolean();
		res->Release();
		return s;
	}

	uchar Byte() {
		Tamgu* res = Callconversion(a_byte);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->Byte();
            }
            return 0;
        }

		uchar s = res->Byte();
		res->Release();
		return s;
	}

	//Basic operations
	long Size() {
		return frame->vnames.size();
	}

	Tamgu* andset(Tamgu* a, bool itself) {
		Tamgu* res = Calloperation(a_and, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->andset(a, itself);
            }
        }
        return res;
	}

	Tamgu* orset(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_or, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->orset(a, itself);
            }
        }
        return res;
	}

	Tamgu* xorset(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_xor, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->xorset(a, itself);
            }
        }
        return res;
	}


	Tamgu* plus(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_plus, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->plus(a, itself);
            }
        }
        return res;
	}

	Tamgu* minus(Tamgu* a, bool itself) {
		Tamgu* res = Calloperation(a_minus, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->minus(a, itself);
            }
        }
        return res;
	}

	Tamgu* multiply(Tamgu* a, bool itself) {
		Tamgu* res = Calloperation(a_multiply, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->multiply(a, itself);
            }
        }
        return res;
	}

    Tamgu* divide(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_divide, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->divide(a, itself);
            }
        }
        return res;
    }

    Tamgu* divideinteger(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_divideinteger, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->divide(a, itself);
            }
        }
        return res;
    }

	Tamgu* power(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_power, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->power(a, itself);
            }
        }
        return res;
	}

	Tamgu* shiftleft(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_shiftleft, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->shiftleft(a, itself);
            }
        }
        return res;
	}

    Tamgu* shiftright(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_shiftright, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->shiftright(a, itself);
            }
        }
        return res;
    }

	Tamgu* mod(Tamgu* a, bool itself) {
        Tamgu* res = Calloperation(a_mod, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->mod(a, itself);
            }
        }
        return res;
	}

	Tamgu* less(Tamgu* a) {
        Tamgu* res = Calloperation(a_less, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->less(a);
            }
        }
        return res;
	}

	Tamgu* more(Tamgu* a) {
        Tamgu* res = Calloperation(a_more, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->more(a);
            }
        }
        return res;
	}

	Tamgu* lessequal(Tamgu* a) {
        Tamgu* res = Calloperation(a_lessequal, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->lessequal(a);
            }
        }
        return res;
	}

	Tamgu* moreequal(Tamgu* a) {
        Tamgu* res = Calloperation(a_moreequal, a);
        if (res == aNULL) {
            if (frame->theextensionvar) {
                return Declaration(frame->theextensionvar)->moreequal(a);
            }
        }
        return res;
	}


};


class Tamguframemininstance : public TamguframeBaseInstance {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    
    basemin_hash<Tamgu*> declarations;
    //---------------------------------------------------------------------------------------------------------------------
    
    Tamguframemininstance(TamguFrame* f) : TamguframeBaseInstance(f) {
        declarations.init(f->minvar, f->maxvar);
    }
    
    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Tamgu* Putvalue(Tamgu* value, short idthread);
    Tamgu* Clonevalue(Tamgu* value, short idthread);
    void Postinstantiation(short idthread, bool setreference);

    
    bool isDeclared(short id) {
        return declarations.check(id);
    }
    
    void Declare(short id, Tamgu* a) {
        declarations[id] = a;
    }
    
    Tamgu* Declaration(short id) {
        return declarations[id];
    }

    Tamgu* Getvariable(short id) {
        if (!hasLock())
            return declarations[id];
        
        Tamgu* v = declarations[id];
        v->Enablelock(is_tobelocked);
        return v;
    }

    inline void Finalclean(std::set<Tamgu*>& elements_to_delete) {
        short nm;
        Tamgu* k;
        for (short i = 0; i < frame->vnames.last; i++) {
            nm = frame->vnames.vecteur[i];
            if (declarations.check(nm)) {
                k = declarations[nm];
                if (k != NULL && elements_to_delete.find(k) == elements_to_delete.end()) {
                    k->Deletion(elements_to_delete);
                }
            }
        }
        declarations.clear();
    }

    inline void Cleaning(short idthread) {
        short nm;
        for (short i = 0; i < frame->vnames.last; i++) {
            nm = frame->vnames.vecteur[i];
            declarations[nm]->Resetreference();
            globalTamgu->Removevariable(idthread, nm);
        }
        declarations.clear();
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    //---------------------------------------------------------------------------------------------------------------------
    void Addreference(unsigned short inv, short r=1) {
        investigate |= (inv & is_tobelocked);
        locking();
        if (!setReference && r) {
            for (short i = 0; i < declarations.sz; i++) {
                if (declarations.table[i] != NULL)
                    declarations.table[i]->Addreference(inv);
            }
            setReference = true;
        }
        reference += r;
        protect = false;
        unlocking();
    }
    
    void Setreference(short r) {
        locking();
        if (!setReference && r) {
            for (short i = 0; i < declarations.sz; i++) {
                if (declarations.table[i] != NULL)
                    declarations.table[i]->Setreference();
            }
            setReference = true;
        }
        reference += r;
        protect = false;
        unlocking();
    }
    

    void Setreference() {
        locking();
        if (!setReference) {
            for (short i = 0; i < declarations.sz; i++) {
                if (declarations.table[i] != NULL)
                    declarations.table[i]->Setreference();
            }
            setReference = true;
        }
        ++reference;
        protect = false;
        unlocking();
    }
    
    void Release() {
        if (reference == 0) {
            locking();
            protect = false;
            for (short i = 0; i < declarations.sz; i++) {
                if (declarations.table[i] != NULL)
                    declarations.table[i]->Setprotect(false);
            }
            unlocking();
            Resetreference(0);
        }
    }

    void Resetreference(short r = 1) {
        bool deleted = false;
        locking();
        
        if (reference <= r) {
            //We are about to delete our object... We call our final function then...
            MethodFinal();
            deleted = true;
        }
        
        if (deleted) {
            reference = 0;
            if (protect) {
                for (short i = 0; i < declarations.sz; i++) {
                    if (declarations.table[i] != NULL) {
                        declarations.table[i]->Setprotect(false);
                    }
                }
                protect = false;
                deleted = false;
            }
            else {
                if (idtracker != -1)
                    globalTamgu->RemoveFromTracker(idtracker);
                for (short i = 0; i < declarations.sz; i++) {
                    if (declarations.table[i] != NULL)
                        declarations.table[i]->Resetreference();
                }
                declarations.clear();
            }
        }
        else
            reference -= r;
        
        unlocking();
        
        if (deleted)
            delete this;
    }
    
    void Setprotect(bool n) {
        protect = n;
        locking();
        for (short i = 0; i < declarations.sz; i++) {
            if (declarations.table[i] != NULL)
                declarations.table[i]->Setprotect(n);
        }
        unlocking();
    }
    
    void Popping() {
        protect = false;
        if (reference <= 0)
            protect = true;

        locking();

        for (short i = 0; i < declarations.sz; i++) {
            if (declarations.table[i] != NULL)
                declarations.table[i]->Popping();
        }
        unlocking();
    }
    
    Tamgu* same(Tamgu* a) {
        if (a == this)
            return aTRUE;
        
        Tamgu* res = Calloperation(a_same, a);
        if (res == aNULL) {
            if (frame->theextensionvar)
                return Declaration(frame->theextensionvar)->same(a);
            
            if (a->Frame() != frame)
                return aFALSE;

            locking();
            Tamguframemininstance* tfm = (Tamguframemininstance*)a;
            for (short i = 0; i < declarations.sz; i++) {
                if (declarations.table[i] != NULL && declarations.table[i]->same(tfm->declarations.table[i]) == aFALSE) {
                    unlocking();
                    return aFALSE;
                }
                
            }
            unlocking();
            
            return aTRUE;
        }
        return res;
    }

    Tamgu* different(Tamgu* a) {
        if (a == this)
            return aFALSE;
        
        Tamgu* res = Calloperation(a_different, a);
        if (res == aNULL) {
            if (frame->theextensionvar)
                return Declaration(frame->theextensionvar)->different(a);

            if (a->Frame() != frame)
                return aTRUE;

            locking();
            Tamguframemininstance* tfm = (Tamguframemininstance*)a;
            for (short i = 0; i < declarations.sz; i++) {
                if (declarations.table[i] != NULL && declarations.table[i]->same(tfm->declarations.table[i]) == aFALSE) {
                    unlocking();
                    return aTRUE;
                }                
            }
            unlocking();
            
            return aFALSE;
        }
        return res;
    }
};


class Tamguframeinstance : public TamguframeBaseInstance {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    
    VECTE<Tamgu*> declarations;
    //---------------------------------------------------------------------------------------------------------------------
    
    Tamguframeinstance(TamguFrame* f) : TamguframeBaseInstance(f) {
        declarations.resize(f->vnames.size(), aNULL);
    }
    
    //----------------------------------------------------------------------------------------------------------------------
    Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Tamgu* Putvalue(Tamgu* value, short idthread);
    Tamgu* Clonevalue(Tamgu* value, short idthread);
    void Postinstantiation(short idthread, bool setreference);
    
    
    inline bool isDeclared(short id) {
        return frame->names.check(id);
    }
    
    inline short Idposition(short id) {
        return frame->names.get(id);
    }
    
    inline void Declare(short id, Tamgu* a) {
        declarations.pushat(frame->names.get(id), a);
    }
    
    inline Tamgu* Declaration(short id) {
        return declarations.vecteur[frame->names.get(id)];
    }
    
    Tamgu* Getvariable(short id) {
        if (!hasLock())
            return declarations.vecteur[frame->names.get(id)];
        
        Tamgu* e = declarations.vecteur[frame->names.get(id)];
        e->Enablelock(is_tobelocked);
        return e;
    }
        
    inline void Finalclean(std::set<Tamgu*>& elements_to_delete) {
        Tamgu* k;
        for (short i = 0; i < declarations.last; i++) {
            k = declarations.vecteur[i];
            if (k!= NULL && elements_to_delete.find(k) == elements_to_delete.end()) {
                k->Deletion(elements_to_delete);
            }
        }
        declarations.last = 0;
    }

    inline void Cleaning(short idthread) {
        for (short i = 0; i < declarations.last; i++) {
            declarations.vecteur[i]->Resetreference();
            globalTamgu->Removevariable(idthread, frame->vnames.vecteur[i]);
        }
        
        declarations.last = 0;
    }
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    //---------------------------------------------------------------------------------------------------------------------
    void Addreference(unsigned short inv, short r=1) {
        locking();
        investigate |= (inv & is_tobelocked);
        if (!setReference) {
            for (short i = 0; i < declarations.last; i++)
                declarations[i]->Addreference(inv);
            setReference = true;
        }
        reference += r;
        protect = false;
        unlocking();
    }

    void Setreference(short r) {
        locking();
        if (!setReference) {
            for (short i = 0; i < declarations.last; i++)
                declarations[i]->Setreference();
            setReference = true;
        }
        reference += r;
        protect = false;
        unlocking();
    }
    
    void Setreference() {
        locking();
        if (!setReference) {
            for (short i = 0; i < declarations.last; i++)
                declarations[i]->Setreference();
            setReference = true;
        }
        ++reference;
        protect = false;
        unlocking();
    }
    
    void Release() {
        if (reference == 0) {
            locking();
            protect = false;
            for (short i = 0; i < declarations.last; i++)
                declarations[i]->Setprotect(false);
            unlocking();
            Resetreference(0);
        }
    }
    
    void Resetreference(short r = 1) {
        bool deleted = false;
        {
            locking();
            
            if (reference <= r) {
                //We are about to delete our object... We call our final function then...
                MethodFinal();
                deleted = true;
            }
            
            if (deleted) {
                reference = 0;
                if (protect) {
                    for (short i = 0; i < declarations.last; i++) {
                        declarations[i]->Setprotect(false);
                    }
                    protect = false;
                    deleted = false;
                }
                else {
                    if (idtracker != -1)
                        globalTamgu->RemoveFromTracker(idtracker);
                    for (short i = 0; i < declarations.last; i++)
                        declarations[i]->Resetreference();
                    declarations.clear();
                }
            }
            else
                reference -= r;
            
            unlocking();
        }
        
        if (deleted)
            delete this;
    }
    
    void Setprotect(bool n) {
        protect = n;
        locking();
        for (short i = 0; i < declarations.last; i++)
            declarations.vecteur[i]->Setprotect(n);
        unlocking();
    }
    
    void Popping() {
        protect = false;
        if (reference <= 0)
            protect = true;

        locking();
        for (short i = 0; i < declarations.last; i++)
            declarations.vecteur[i]->Popping();
        unlocking();
    }
    
    Tamgu* same(Tamgu* a) {
        if (a == this)
            return aTRUE;
        
        Tamgu* res = Calloperation(a_same, a);
        if (res == aNULL) {
            if (frame->theextensionvar)
                return Declaration(frame->theextensionvar)->same(a);

            if (a->Frame() != frame)
                return aFALSE;
            
            Tamguframeinstance* tfm = (Tamguframeinstance*)a;
            for (short i = 0; i < declarations.last; i++) {
                if (declarations.vecteur[i] != NULL && declarations.vecteur[i]->same(tfm->declarations.vecteur[i]) == aFALSE) {
                    unlocking();
                    return aFALSE;
                }
                
            }
            unlocking();
            return aTRUE;
        }
        return res;
    }

    Tamgu* different(Tamgu* a) {
        if (a == this)
            return aFALSE;
        
        Tamgu* res = Calloperation(a_different, a);
        if (res == aNULL) {
            if (frame->theextensionvar)
                return Declaration(frame->theextensionvar)->different(a);
            
            if (a->Frame() != frame)
                return aTRUE;
            
            locking();
            
            Tamguframeinstance* tfm = (Tamguframeinstance*)a;
            for (short i = 0; i < declarations.last; i++) {
                if (declarations.vecteur[i] != NULL && declarations.vecteur[i]->same(tfm->declarations.vecteur[i]) == aFALSE) {
                    unlocking();
                    return aTRUE;
                }
                
            }
            unlocking();
            return aFALSE;
        }
        return res;
    }
    
};

#endif
