/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemap.h
 Date       : 2017/09/01
 Purpose    : map implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgutreemap_h
#define tamgutreemap_h

#include "tamguint.h"
#include "tamgustring.h"
#include "tamgusvector.h"
#include "tamguvector.h"
#include "tamguconstants.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutreemap;
//This typedef defines a type "treemapMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutreemap::*treemapMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------

class Tamgutreemap : public TamguObjectLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<treemapMethod> methods;
    static Exchanging hmap<string, string> infomethods;
    static Exchanging basebin_hash<unsigned long> exported;

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    map<string, Tamgu*> values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgutreemap(TamguGlobal* g, Tamgu* parent = NULL) : TamguObjectLockContainer(g, parent) {
        //Do not forget your variable initialisation
        isconst = false;
    }

    Tamgutreemap() {
        //Do not forget your variable initialisation
        isconst = false;
    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Loopin(TamguInstruction*, Tamgu* context, short idthread);
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    void SetConst() { isconst = true;}

    short Type() {
        return a_treemap;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "treemap";
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


    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }


    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgutreemap* m = new Tamgutreemap;
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->Push(it->Keystring(), it->IteratorValue());
            }
            it->Release();
            return m;
        }
        
        map<string,Tamgu*>::iterator ist;
        for (ist=values.begin(); ist!=values.end();ist++)
            m->Push(ist->first,a);
        return m;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgutreemap();
    }

    Exporting TamguIteration* Newiteration(bool direction);

    void Setprotect(bool n) {
        if (!lockingmark())
            return;

        protect = n;

        map<string, Tamgu*>::iterator it;


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
        map<string, Tamgu*>::iterator it;


        for (it = values.begin(); it != values.end(); it++)
            it->second->Popping();
        unlockingmark();
    }

    void Protectcontainervalues() {
        if (!lockingmark())
            return;

        protect = true;
        map<string, Tamgu*>::iterator it;
        

        for (it = values.begin(); it != values.end(); it++)
            it->second->Setprotect(true);
        unlockingmark();
    }


    Tamgu* Atom(bool forced) {
        if (forced) {
            Tamgutreemap* m = new Tamgutreemap;
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

    static void AddMethod(TamguGlobal* g, string name, treemapMethod func, unsigned long arity, string infos);

    static bool InitialisationModule(TamguGlobal* g, string version);


    void Methods(Tamgu* v) {
        hmap<string, string>::iterator it;
        for (it = infomethods.begin(); it != infomethods.end(); it++)
            v->storevalue(it->first);
    }

    string Info(string n) {

        if (infomethods.find(n) != infomethods.end())
            return infomethods[n];
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

    Tamgu* MethodRead(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
    
    Tamgu* MethodClear(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        Clear();
        return aTRUE;
    }



    
    Tamgu* MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        if (contextualpattern == this || !contextualpattern->isMapContainer() || !contextualpattern->isAffectation())
            contextualpattern = Newinstance(idthread);
        else
            contextualpattern->Clear();
            
        Tamgu* a;

        for (auto& it : values) {
            a = globalTamgu->Providestring(it.first);
            contextualpattern->Push(it.second, a);
            a->Release();
        }

        return contextualpattern;
    }

    Exporting Tamgu* MethodFind(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    Tamgu* MethodItems(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

        Tamgutreemap* item;
        Tamgu* vect = Selectavector(contextualpattern);
        for (auto& itr : values) {
            item = new Tamgutreemap;
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
        Tamgusvector* vstr = (Tamgusvector*)Selectasvector(contextualpattern);
        map<string, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        unlocking();
        return vstr;
    }

    Tamgu* MethodValues(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamguvector* v = (Tamguvector*)Selectavector(contextualpattern);
        map<string, Tamgu*>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            v->Push(it->second);
        unlocking();
        return v;
    }

    Tamgu* MethodTest(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        string  v = callfunc->Evaluate(0, contextualpattern, idthread)->String();
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

    //---------------------------------------------------------------------------------------------------------------------

    Exporting Tamgu* Push(Tamgu* k, Tamgu* v);
    Exporting Tamgu* Pop(Tamgu* kkey);

    Tamgu* push(string k, Tamgu* a) {
        Tamgu* v = values[k];
        if (v != NULL)
            v->Removereference(reference + 1);
        values[k] = a;
        a->Addreference(investigate);
        return this;
    }

    inline void pushing(string& k, Tamgu* a) {
        locking();
        Tamgu* v = values[k];
        if (v != NULL)
            v->Removereference(reference + 1);
        a = a->Atom();
        values[k] = a;
        a->Addreference(investigate,reference + 1);
        unlocking();
    }

    Tamgu* Push(string k, Tamgu* a) {
        locking();
        Tamgu* v = values[k];
        if (v != NULL)
            v->Removereference(reference + 1);
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


        string n;
        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            n = itr->Keystring();
            if (values.find(n) == values.end())
                pushing(n, itr->Value());
            else {
                v = globalTamgu->Providevector();
                v->Push(values[n]);
                v->Push(itr->Value());
                pushing(n, v);
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


        string n;
        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            n = itr->Keystring();
            if (values.find(n) == values.end())
                pushing(n, itr->Value());
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
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    

    Exporting string String();
    Exporting string JSonString();

    //wstring UString() {}

    Exporting long Integer();
    Exporting double Float();
    Exporting BLONG Long();
    Exporting bool Boolean();


    Tamgu* Value(Tamgu* a) {
        string s =  a->String();
        locking();
        Tamgu* v;
        try {
            v = values.at(s);
            unlocking();
            return v;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(string& s) {
        locking();
        Tamgu* v;
        try {
            v = values.at(s);
            unlocking();
            return v;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(long n) {
        
        string s = convertfromnumber(n);
        Tamgu* v;
        try {
            v = values.at(s);
            unlocking();
            return v;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(double n) {
        
        string s = convertfromnumber(n);
        Tamgu* v;
        try {
            v = values.at(s);
            unlocking();
            return v;
        }
        catch (const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

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
    Exporting Tamgu* power(Tamgu* a, bool itself);
    Exporting Tamgu* shiftleft(Tamgu* a, bool itself);
    Exporting Tamgu* shiftright(Tamgu* a, bool itself);
    Exporting Tamgu* mod(Tamgu* a, bool itself);

    Exporting Tamgu* same(Tamgu* a);

};
//--------------------------------------------------------------------------------------------------
class TamguIterationtreemap : public TamguIteration {
    public:

    map<string, Tamgu*>::iterator it;
    Tamgutreemap* map;
    Locking _lock;

    TamguIterationtreemap(Tamgutreemap* m, bool d, TamguGlobal* g = NULL) : map(m), _lock(m), TamguIteration(d, g) {}

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it->first);
    }

    Tamgu* Key() {
        return globalTamgu->Providestring(it->first);
    }

    Tamgu* Value() {
        return it->second;
    }

    string Keystring() {
        return it->first;
    }

    string Valuestring() {
        return it->second->String();
    }

    wstring Keyustring() {
        wstring ws;
        s_utf8_to_unicode(ws, USTR(it->first), it->first.size());
        return ws;
    }

    wstring Valueustring() {
        return it->second->UString();
    }

    long Keyinteger() {
        return (long)conversionintegerhexa(STR(it->first));
    }

    long Valueinteger() {
        return it->second->Float();
    }

    BLONG Keylong() {
        return (long)conversionintegerhexa(STR(it->first));
    }

    BLONG Valuelong() {
        return it->second->Long();
    }

    double Keyfloat() {
        return convertfloat(STR(it->first));
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


#endif
