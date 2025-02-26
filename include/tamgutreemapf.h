/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapf.h
 Date       : 2017/09/01
 Purpose    : map implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef Tamgutreemapf_h
#define Tamgutreemapf_h

//#prime
#include "tamguint.h"
#include "tamgustring.h"
#include "tamguvector.h"
#include "tamguconstants.h"
#include "tamgufvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutreemapf;
//This typedef defines a type "treemapfMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutreemapf::*treemapfMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamgutreemapf : public TamguObjectLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<treemapfMethod> methods;
    
    

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    map<double, Tamgu*> values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgutreemapf(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        isconst = false;
    }

    Tamgutreemapf() {
        //Do not forget your variable initialisation
        isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);
    Exporting Tamgu* EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign);
    void SetConst() { isconst = true;}

    short Type() {
        return Tamgutreemapf::idtype;
    }

    string Typename() {
        return "treemapf";
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
        map<double, Tamgu*>::iterator it;

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
        map<double, Tamgu*>::iterator it;

        for (it = values.begin(); it != values.end(); it++)
            it->second->Popping();
        unlockingmark();
    }

    void Protectcontainervalues() {
        if (!lockingmark())
            return;

        protect = true;
        map<double, Tamgu*>::iterator it;
        
        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(true);
        unlockingmark();
    }


    Tamgu* Atom(bool forced) {
        if (forced) {
            Tamgutreemapf* m = new Tamgutreemapf;
            locking();
            Tamgu* v;

            for (auto& it : values) {
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
        map<double, Tamgu*>::iterator itx;
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

        for (auto& itx : values)
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
        Tamgutreemapf* m = new Tamgutreemapf;
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->Push(it->Keyfloat(),it->IteratorValue());
            }
            it->Release();
            return m;
        }
        
        map<double,Tamgu*>::iterator ist;
        for (ist=values.begin(); ist!=values.end();ist++)
            m->Push(ist->first,a);
        return m;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgutreemapf();
    }

    Exporting TamguIteration* Newiteration(bool direction);


    static void AddMethod(TamguGlobal* g, string name, treemapfMethod func, unsigned long arity, string infos);
    
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
    
    void unmark() {
        if (!lockingmark())
            return;
            
        usermark=false;

        for (auto& it : values)
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

        for (auto& it : values) {
            a = globalTamgu->Providefloat(it.first);
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

        Tamgutreemapf* item;
        Tamgu* vect = Selectavector(contextualpattern);
        for (auto& itr : values) {
            item=new Tamgutreemapf;
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
        return globalTamgu->Providefloat(v);
    }

    Tamgu* MethodKeys(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamgufvector* vstr = (Tamgufvector*)Selectafvector(contextualpattern);
        map<double, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        unlocking();
        return vstr;
    }

    Tamgu* MethodValues(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamguvector* v = (Tamguvector*)Selectavector(contextualpattern);
        map<double, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            v->Push(it->second);
        unlocking();
        return v;
    }

    Tamgu* MethodTest(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        double  v = callfunc->Evaluate(0, contextualpattern, idthread)->Float();
        try {
            values.at(v);
            unlocking();
            return aTRUE;
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aFALSE;
        }
    }

    Tamgu* MethodProduct(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        double v = Product();
        return globalTamgu->Providefloat(v);
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
        for (auto& it : values) {
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
    Tamgu* Push(double k, Tamgu* a) {

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
        Doublelocking(this, ke);
        Tamguvector* v;


        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            double n = itr->Keyfloat();
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
        return this;
    }

    Tamgu* Merging(Tamgu* ke) {
        //Three cases:
        if (!ke->isContainer())
            return this;
        Doublelocking(this, ke);


        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            double n = itr->Keyfloat();
            if (values.find(n) == values.end())
                Push(n, itr->Value());
        }
        itr->Release();
        return this;
    }

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*Tamgutreemapf::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    

    Exporting string String();
    Exporting string JSonString();

    Tamgu* Value(string& val) {
        double n = convertdouble(val);
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
    
    Tamgu* Value(Tamgu* a) {
        double n =  a->Float();
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
class TamguIterationtreemapf : public TamguIteration {
    public:

    map<double, Tamgu*>::iterator it;
    Tamgutreemapf* map;
    Locking _lock;

    TamguIterationtreemapf(Tamgutreemapf* m, bool d, TamguGlobal* g = NULL) : map(m), _lock(m), TamguIteration(d, g) {}

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it->first);
    }

    Tamgu* Key() {
        return globalTamgu->Providefloat(it->first);
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

#endif
