/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapu.h
 Date       : 2017/09/01
 Purpose    : map implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef Tamgumapu_h
#define Tamgumapu_h

//#prime
#include "tamguint.h"
#include "tamgustring.h"
#include "tamguvector.h"
#include "tamguconstants.h"
#include "tamguuvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgumapu;
//This typedef defines a type "mapuMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgumapu::*mapuMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamgumapu : public TamguObjectLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<mapuMethod> methods;
    
    

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    hmap<wstring, Tamgu*> values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgumapu(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        isconst = false;
    }

    Tamgumapu() {
        //Do not forget your variable initialisation
        isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);

    Tamgu* EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    void SetConst() { isconst = true;}

    short Type() {
        return Tamgumapu::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "mapu";
    }

    bool isObjectContainer() {
        return true;
    }

    bool isContainerClass() {
        return true;
    }

    bool isMapContainer() {
        return true;
    }

    bool duplicateForCall() {
        return isconst;
    }

    void Setprotect(bool n) {
        if (!lockingmark())
            return;
        
        
        protect = n;
        hmap<wstring, Tamgu*>::iterator it;

        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(n);
        unlockingmark();
    }

    void Popping() {
        if (!lockingmark())
            return;
        
        
        protect = false;
        if (Reference() <= 0)
            protect = true;
        hmap<wstring, Tamgu*>::iterator it;

        for (it = values.begin(); it != values.end(); it++)
            it->second->Popping();
        unlockingmark();
    }

    void Protectcontainervalues() {
        if (!lockingmark())
            return;
        
        protect = true;
        hmap<wstring, Tamgu*>::iterator it;
        
        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(true);

        unlockingmark();
    }


    Tamgu* Atom(bool forced) {
        if (forced) {
            Tamgumapu* m = new Tamgumapu;
            locking();
            Tamgu* v;

            for (const auto& it : values) {
                v = it.second->Atom(true);
                m->values[it.first] = v;
                v->Setreference();
            }
            unlocking();
        return m;
        }
        return this;
    }

    double Sum() {
        locking();
        double v = 0;
        hmap<wstring, Tamgu*>::iterator itx;
        for (itx = values.begin(); itx != values.end(); itx++)
            v += itx->second->Sum();
        unlocking();
        return v;
    }

    double Product() {
        locking();
        
        if (values.size() == 0) {
            unlocking();
            return 0;
         }

         double v = 1;

        for (const auto& itx : values)
            v *= itx.second->Product();
        unlocking();
        return v;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }
    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgumapu* m = new Tamgumapu;
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->Push(it->Keyustring(),it->IteratorValue());
            }
            it->Release();
            return m;
        }
        
        hmap<wstring,Tamgu*>::iterator ist;
        for (ist=values.begin(); ist!=values.end();ist++)
            m->Push(ist->first,a);
        return m;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgumapu();
    }

    Exporting TamguIteration* Newiteration(bool direction);


    static void AddMethod(TamguGlobal* global, string name, mapuMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* global, string version);

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
    
    void unmark() {
        if (!lockingmark())
            return;
        
        usermark=false;
        
        for (const auto& it : values)
            it.second->unmark();
        
        unlockingmark();
    }

    Exporting void Cleanreference(short inc);

    Exporting void Setreference(short r);
    Exporting void Setreference();
    Exporting void Resetreference(short r = 1);
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

    
    Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (contextualpattern == this || !contextualpattern->isMapContainer() || !contextualpattern->isAffectation())
            contextualpattern = Newinstance(idthread);
        else
            contextualpattern->Clear();
            
        Tamgu* a;

        for (const auto& it : values) {
            a = globalTamgu->Provideustring(it.first);
            contextualpattern->Push(it.second, a);
            a->Release();
        }

        return contextualpattern;
    }

    Exporting Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);


    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }


    
    Tamgu* MethodItems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

        Tamgumapu* item;
        Tamgu* vect = Selectavector(contextualpattern);
        for (const auto& itr : values) {
            item=new Tamgumapu;
            item->Push(itr.first, itr.second);
            vect->Push(item);
        }
        return vect;
    }
    
    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

        
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Merging(v);
        return this;
    }

    Tamgu* MethodSum(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Sum();
        return globalTamgu->ProvideConstfloat(v);
    }

    Tamgu* MethodKeys(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamguuvector* vstr = (Tamguuvector*)Selectauvector(contextualpattern);
        hmap<wstring, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        unlocking();
        return vstr;
    }

    Tamgu* MethodValues(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamguvector* v = (Tamguvector*)Selectavector(contextualpattern);
        hmap<wstring, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            v->Push(it->second);
        unlocking();
        return v;
    }

    Tamgu* MethodTest(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        wstring  v = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        bool res = values.find(v) != values.end();
        unlocking();
        return booleantamgu[res];
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Product();
        return globalTamgu->ProvideConstfloat(v);
    }

    Tamgu* MethodPop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Tamgu* pos = callfunc->Evaluate(0, contextualpattern, idthread);
        return Pop(pos);
    }

    Tamgu* MethodJoin(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        //The separator between keys
        wstring keysep = callfunc->Evaluate(0, contextualpattern, idthread)->UString();
        //The separator between values
        wstring sep = callfunc->Evaluate(1, contextualpattern, idthread)->UString();

        bool beg = true;
        std::wstringstream res;
        for (const auto& it : values) {
            if (beg == false)
                res << sep;
            beg = false;
            res << it.first << keysep << it.second->UString();
        }

        unlocking();
        return globalTamgu->Provideustring(res.str());
    }


    //------------------------------------------------------------------------------------------
    Exporting Tamgu* Push(Tamgu* k, Tamgu* v);
    
    
    Exporting Tamgu* Pop(Tamgu* k);
    Tamgu* Push(wstring k, Tamgu* a) {

        locking();
        Tamgu* v = values[k];
        if (v != NULL) {
            if (v == a)
                return this;
            v->Removereference(reference + 1);
        }
        a = a->Atom();
        values[k] = a;
        a->Addreference(investigate,reference + 1);
        unlocking();
        return this;
    }

    inline void pushing(wstring& k, Tamgu* a) {

        locking();
        Tamgu* v = values[k];
        if (v != NULL) {
            if (v == a)
                return;
            v->Removereference(reference + 1);
        }
        a = a->Atom();
        values[k] = a;
        a->Addreference(investigate,reference + 1);
        unlocking();
    }


    Tamgu* Combine(Tamgu* ke) {
        //Three cases:
        if (!ke->isContainer())
            return this;
        
        ((TamguObject*)ke)->locking();
        Tamguvector* v;


        wstring n;
        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            n = itr->Keyustring();
            if (values.find(n) == values.end())
                Push(n, itr->Value());
            else {
                v = globalTamgu->Providevector();
                v->Push(values[n]);
                v->Push(itr->Value());
                Push(n, v);
            }
        }
        itr->Release();
        ((TamguObject*)ke)->unlocking();
        return this;
    }

    Tamgu* Merging(Tamgu* ke) {
        //Three cases:
        if (!ke->isContainer())
            return this;
        
        ((TamguObject*)ke)->locking();


        wstring n;
        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            n = itr->Keyustring();
            if (values.find(n) == values.end())
                Push(n, itr->Value());
        }
        itr->Release();
        ((TamguObject*)ke)->unlocking();
        return this;
    }

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*Tamgumapu::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    

    Exporting string String();
    Exporting string JSonString();

    Tamgu* Value(Tamgu* a) {
        wstring s =  a->UString();

        locking();
        try {
            Tamgu* res = values.at(s);
            unlocking();
            return res;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(wstring& s) {
        locking();
        try {
            Tamgu* res = values.at(s);
            unlocking();
            return res;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(string& n) {
        wstring s;
        s_utf8_to_unicode(s, USTR(n), n.size());
        locking();
        try {
            Tamgu* res = values.at(s);
            unlocking();
            return res;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(long n) {
        
        wstring s = wconvertfromnumber(n);
        locking();
        try {
            Tamgu* res = values.at(s);
            unlocking();
            return res;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(double n) {
        
        wstring s = wconvertfromnumber(n);
        locking();
        try {
            Tamgu* res = values.at(s);
            unlocking();
            return res;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }


    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


    //Basic operations
    Exporting long Size();
    
    Exporting Tamgu* in(Tamgu* context, Tamgu* a, short idthread);

    Exporting Tamgu* andset(Tamgu* a, bool itself);
    Exporting Tamgu* orset(Tamgu* a, bool itself);
    Exporting Tamgu* xorset(Tamgu* a, bool itself);
    Exporting Tamgu* plus(Tamgu* a, bool itself);
    Exporting Tamgu* minus(Tamgu* a, bool itself);
    Exporting Tamgu* multiply(Tamgu* a, bool itself);
    Exporting Tamgu* divide(Tamgu* a, bool itself);
    Exporting Tamgu* divideinteger(Tamgu* a, bool itself);
    Exporting Tamgu* power(Tamgu* a, bool itself);
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    Exporting Tamgu* shiftright(Tamgu* a, bool itself);
    Exporting Tamgu* mod(Tamgu* a, bool itself);

    Exporting Tamgu* same(Tamgu* a);

};

//--------------------------------------------------------------------------------------------------
class TamguIterationmapu : public TamguIteration {
    public:

    hmap<wstring, Tamgu*>::iterator it;
    Tamgumapu* map;
    Locking _lock;

    TamguIterationmapu(Tamgumapu* m, bool d, TamguGlobal* g = NULL) : map(m), _lock(m), TamguIteration(d, g) {}

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it->first);
    }

    Tamgu* Key() {
        return globalTamgu->Provideustring(it->first);
    }

    Tamgu* Value() {
        return it->second;
    }

    wstring Keyustring() {
        return it->first;
    }

    string Keystring() {
        string res;
        wstring w = it->first;
        s_unicode_to_utf8(res, w);
        return res;
    }

    long Keyinteger() {
        return convertinteger(it->first);
    }

    double Keyfloat() {
        return convertfloat(it->first);
    }

    string Valuestring() {
        return it->second->String();
    }

    wstring Valueustring() {
        return it->second->UString();
    }

    long Valueinteger() {
        return it->second->Float();
    }

    double Valuefloat() {
        return it->second->Float();
    }

    void Next() {
        it++;
    }

    Tamgu* End() {
        if (it == map->values.end())
            return aTRUE;
        return aFALSE;
    }

    Tamgu* Begin() {
        it = map->values.begin();
        return aTRUE;
    }

};

//-------------------------------------------------------------------------------------------

#endif
