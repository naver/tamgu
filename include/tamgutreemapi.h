/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapi.h
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef Tamgutreemapi_h
#define Tamgutreemapi_h

//#prime
#include "tamguint.h"
#include "tamgustring.h"
#include "tamguvector.h"
#include "tamguconstants.h"
#include "tamguivector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutreemapi;
//This typedef defines a type "treemapiMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutreemapi::*treemapiMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamgutreemapi : public TamguObjectLockContainer {
public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<treemapiMethod> methods;
    
    
    
    static Exchanging short idtype;
    
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    map<long, Tamgu*> values;
    bool isconst;
    
    //---------------------------------------------------------------------------------------------------------------------
    Tamgutreemapi(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        isconst = false;
    }
    
    Tamgutreemapi() {
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
        return a_treemapi;
    }
    
    
    
    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "treemapi";
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
    
    void Setprotect(bool n) {
        if (!lockingmark())
            return;

        protect = n;
        map<long, Tamgu*>::iterator it;
        

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
        map<long, Tamgu*>::iterator it;
        

        for (it = values.begin(); it != values.end(); it++)
            it->second->Popping();
        unlockingmark();
    }
    
    void Protectcontainervalues() {
        if (!lockingmark())
            return;

        protect = true;
        map<long, Tamgu*>::iterator it;
        

        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(true);
        unlockingmark();
    }
    
    
    Tamgu* Atom(bool forced) {
        if (forced) {
            Tamgutreemapi* m = new Tamgutreemapi;
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
    
    bool duplicateForCall() {
        return isconst;
    }
    
    double Sum() {
        locking();
        double v = 0;
        map<long, Tamgu*>::iterator itx;
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
        Tamgutreemapi* m = new Tamgutreemapi;
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->Push(it->Keyinteger(),it->IteratorValue());
            }
            it->Release();
            return m;
        }
        
        map<long,Tamgu*>::iterator ist;
        for (ist=values.begin(); ist!=values.end();ist++)
            m->Push(ist->first,a);
        return m;
    }
    
    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgutreemapi();
    }
    
    Exporting TamguIteration* Newiteration(bool direction);
    
    
    static void AddMethod(TamguGlobal* g, string name, treemapiMethod func, unsigned long arity, string infos);
    
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
            a = globalTamgu->ProvideConstint(it.first);
            contextualpattern->Push(it.second, a);
        }
        
        return contextualpattern;
    }
    
    Exporting Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }
    
    
    
    Tamgu* MethodItems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        
        Tamgutreemapi* item;
        Tamgu* vect = Selectavector(contextualpattern);
        for (const auto& itr : values) {
            item=new Tamgutreemapi;
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
        Tamguivector* vstr = (Tamguivector*)Selectaivector(contextualpattern);
        map<long, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        unlocking();
        return vstr;
    }
    
    Tamgu* MethodValues(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamguvector* v = (Tamguvector*)Selectavector(contextualpattern);
        map<long, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            v->Push(it->second);
        unlocking();
        return v;
    }
    
    Tamgu* MethodTest(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        long  v = callfunc->Evaluate(0, contextualpattern, idthread)->Integer();
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
        string keysep = callfunc->Evaluate(0, contextualpattern, idthread)->String();
        //The separator between values
        string sep = callfunc->Evaluate(1, contextualpattern, idthread)->String();
        
        bool beg = true;
        stringstream res;
        for (const auto& it : values) {
            if (beg == false)
                res << sep;
            beg = false;
            res << it.first << keysep << it.second->String();
        }
        
        unlocking();
        return globalTamgu->Providestring(res.str());
    }
    
    
    //------------------------------------------------------------------------------------------
    Exporting Tamgu* Push(Tamgu* k, Tamgu* v);
    Exporting Tamgu* Pop(Tamgu* k);
    
    Tamgu* Push(long k, Tamgu* a) {        
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
    
    Tamgu* Combine(Tamgu* ke) {
        //Three cases:
        if (!ke->isContainer())
            return this;
        
        ((TamguObject*)ke)->locking();
        Tamguvector* v;
        
        
        long n;
        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            n = itr->Keyinteger();
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
        
        
        long n;
        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            n = itr->Keyinteger();
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
        return (this->*Tamgutreemapi::methods.get(idname))(contextualpattern, idthread, callfunc);
    }
    
    Exporting void Clear();
    
    
    Exporting string String();
    Exporting string JSonString();
    
    Tamgu* Value(string& val) {
        long n = convertlong(val);
        Tamgu* v;
        if (globalTamgu->threadMODE) {
            locking();
            v = values[n];
            if (v == NULL) {
                values.erase(n);
                v = aNOELEMENT;
            }
            unlocking();
            return v;
        }
        
        v = values[n];
        if (v == NULL) {
            values.erase(n);
            return aNOELEMENT;
        }
        return v;
    }
    
    Tamgu* Value(Tamgu* a) {
        long n =  a->Integer();
        Tamgu* v;
        if (globalTamgu->threadMODE) {
            locking();
            v = values[n];
            if (v == NULL) {
                values.erase(n);
                v = aNOELEMENT;
            }
            unlocking();
            return v;
        }
        
        v = values[n];
        if (v == NULL) {
            values.erase(n);
            return aNOELEMENT;
        }
        return v;
    }
    
    Tamgu* Value(long n) {
        Tamgu* v;
        if (globalTamgu->threadMODE) {
            locking();
            v = values[n];
            if (v == NULL) {
                values.erase(n);
                v = aNOELEMENT;
            }
            unlocking();
            return v;
        }
        
        v = values[n];
        if (v == NULL) {
            values.erase(n);
            return aNOELEMENT;
        }
        return v;
    }
    
    Tamgu* Value(double n) {
        Tamgu* v;
        if (globalTamgu->threadMODE) {
            locking();
            v = values[n];
            if (v == NULL) {
                values.erase(n);
                v = aNOELEMENT;
            }
            unlocking();
            return v;
        }
        
        v = values[n];
        if (v == NULL) {
            values.erase(n);
            return aNOELEMENT;
        }
        return v;
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
class TamguIterationtreemapi : public TamguIteration {
public:
    
    map<long, Tamgu*>::iterator it;
    Tamgutreemapi* map;
    Locking _lock;
    
    TamguIterationtreemapi(Tamgutreemapi* m, bool d, TamguGlobal* g = NULL) : map(m), _lock(m), TamguIteration(d, g) {}
    
    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it->first);
    }
    
    Tamgu* Key() {
        return globalTamgu->ProvideConstint(it->first);
    }
    
    Tamgu* Value() {
        return it->second;
    }
    
    string Keystring() {
        
        return convertfromnumber(it->first);
        
    }
    
    string Valuestring() {
        return it->second->String();
    }
    
    wstring Keyustring() {
        
        return wconvertfromnumber(it->first);
        
    }
    
    wstring Valueustring() {
        return it->second->UString();
    }
    
    
    BLONG Keylong() {
        return it->first;
    }
    
    long Keyinteger() {
        return it->first;
    }
    
    
    BLONG Valuelong() {
        return it->second->Long();
    }
    
    long Valueinteger() {
        return it->second->Float();
    }
    
    double Keyfloat() {
        return it->first;
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
class Tamguframetreemapi : public Tamgutreemapi {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    Tamgu* frame;

    //---------------------------------------------------------------------------------------------------------------------
    Tamguframetreemapi(Tamgu* fr, TamguGlobal* g, Tamgu* parent = NULL) : Tamgutreemapi(g, parent) {
        //Do not forget your variable initialisation
        frame = fr;
    }

    Tamguframetreemapi(Tamgu* fr) {
        frame = fr;
    }

    //----------------------------------------------------------------------------------------------------------------------
    short Type() {
        return a_frametreemapi;
    }

    string Typename() {
        return "frametreemapi";
    }

    Tamgu* Atom(bool forced) {
        if (forced) {
            Tamguframetreemapi* m = new Tamguframetreemapi(frame);
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
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below

    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamguframetreemapi* m = new Tamguframetreemapi(frame);
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->Push(it->Keyinteger(), it->IteratorValue());
            }
            it->Release();
            return m;
        }
        
        map<long,Tamgu*>::iterator ist;
        for (ist=values.begin(); ist!=values.end();ist++)
            m->Push(ist->first,a);
        return m;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamguframetreemapi(frame);
    }

    short Typeinfered() {
        return frame->Name();
    }

    inline bool check_frame(Tamgu* value) {
        return globalTamgu->Compatible(frame->Name(), value->Type());
    }

    Tamgu* Frame() {
        return frame;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //------------------------------------------------------------------------------------------
    Exporting Tamgu* Push(Tamgu* k, Tamgu* v);
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    
    Tamgu* Push(long k, Tamgu* a) {
        if (!check_frame(a)) {
            return globalTamgu->Returnerror(e_error_on_frame_map);
        }

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
    //---------------------------------------------------------------------------------------------------------------------

};

#endif
