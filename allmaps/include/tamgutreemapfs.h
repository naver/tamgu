/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapfs.h
 Date       : 2017/09/01
 Purpose    : map implementation 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef Tamgutreemapfs_h
#define Tamgutreemapfs_h

//#prime
#include "tamguint.h"
#include "tamgustring.h"
#include "tamguvector.h"
#include "tamguconstants.h"
#include "tamgufvector.h"
#include "tamgusvector.h"

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgutreemapfs;
//This typedef defines a type "treemapfsMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgutreemapfs::*treemapfsMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
//---------------------------------------------------------------------------------------------------------------------
class Tamgutreemapfs : public TamguLockContainer {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static Exchanging basebin_hash<treemapfsMethod> methods;
    
    

    static Exchanging short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
    map<double, string> values;
    bool isconst;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgutreemapfs(TamguGlobal* g, Tamgu* parent = NULL) : TamguLockContainer(g, parent) {
     investigate |= is_string;
        //Do not forget your variable initialisation
        isconst = false; 

    }

    Tamgutreemapfs() {
     investigate |= is_string;
        //Do not forget your variable initialisation
        isconst = false; 

    }

    //----------------------------------------------------------------------------------------------------------------------
    Exporting Tamgu* Loopin(TamguInstruction* ins, Tamgu* context, short idthread);
    Exporting Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
    Exporting Tamgu* Eval(Tamgu* context, Tamgu* value, short idthread);

    void SetConst() { isconst = true;}

    short Type() {
        return Tamgutreemapfs::idtype;
    }

    

    void Setidtype(TamguGlobal* global);
    
    string Typename() {
        return "treemapfs";
    }

    

    bool isContainerClass() {
        return true;
    }

    bool isMapContainer() {
        return true;
    }

    bool isValueContainer() {
        return true;
    }


    Tamgu* Atom(bool forced) {
        if (forced || !protect || reference) {
            locking();
            Tamgutreemapfs * m = new Tamgutreemapfs;
            m->values = values;
            unlocking();
            return m;
        }
        return this;
    }
    
    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        return methods.check(n);
    }

    
    Tamgu* Newvalue(Tamgu* a, short idthread) {
        Tamgutreemapfs* m = new Tamgutreemapfs;
        if (a->isContainer()) {
            TamguIteration* it = a->Newiteration(false);
            for (it->Begin(); it->End() == aFALSE; it->Next()) {
                m->values[it->Keyfloat()]=it->Valuestring();
            }
            it->Release();
            return m;
        }
        
        string val=a->String();
        map<double,string>::iterator ist;
        for (ist=values.begin(); ist!=values.end();ist++)
            m->values[ist->first]=val;
        return m;
    }

    Tamgu* Newinstance(short idthread, Tamgu* f = NULL) {
        return new Tamgutreemapfs();
    }

    Exporting TamguIteration* Newiteration(bool direction);

    bool duplicateForCall() {
        return isconst;
    }




    static void AddMethod(TamguGlobal* global, string name, treemapfsMethod func, unsigned long arity, string infos);
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
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.

    
    Tamgu*
    MethodInvert(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
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

        Tamgutreemapfs* item;
        Tamgu* vect = Selectavector(contextualpattern);
        for (auto& itr : values) {
            item=new Tamgutreemapfs;
            item->values[itr.first] = itr.second;
            vect->Push(item);
        }
        return vect;
    }
    
    Tamgu* MethodMerge(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {

        
        Tamgu* v = callfunc->Evaluate(0, contextualpattern, idthread);
        Merging(v);
        return this;
    }

    Tamgu* MethodKeys(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamgufvector* vstr = (Tamgufvector*)Selectafvector(contextualpattern);
        map<double, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->first);
        unlocking();
        return vstr;
    }

    Tamgu* MethodValues(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        locking();
        Tamgusvector * vstr = (Tamgusvector*)Selectasvector(contextualpattern);
        map<double, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            vstr->values.push_back(it->second);
        unlocking();
        return vstr;
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
            res << it.first << keysep << it.second;
        }

        unlocking();
        return globalTamgu->Providestring(res.str());
    }


    //------------------------------------------------------------------------------------------
    Exporting Tamgu* Push(Tamgu* k, Tamgu* v);
    Exporting Tamgu* Pop(Tamgu* kkey);

    Tamgu* Push(double k, Tamgu* a) {
        locking();
        values[k] = a->String();
        unlocking();
        return this;
    }

    Tamgu* Combine(Tamgu* ke) {
        //Three cases:
        if (!ke->isContainer())
            return this;
        Doublelocking(this, ke);


        TamguIteration* itr = ke->Newiteration(false);
        for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
            double n = itr->Keyfloat();
            if (values.find(n) == values.end())
                values[n] = itr->Valuestring();
            else
                values[n] += itr->Valuestring();
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
                values[n] = itr->Valuestring();
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
        return (this->*Tamgutreemapfs::methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    Exporting void Clear();
    

    Exporting string String();
    Exporting string JSonString();

    Tamgu* Value(string& n) {
        double v = convertdouble(n);
        locking();
        try {
            Tamgu* res = globalTamgu->Providestring(values.at(v));
            unlocking();
            return res;
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(long v) {
        locking();
        try {
            Tamgu* res = globalTamgu->Providestring(values.at(v));
            unlocking();
            return res;
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(Tamgu* a) {
        double v =  a->Float();
        locking();
        try {
            Tamgu* res = globalTamgu->Providestring(values.at(v));
            unlocking();
            return res;
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }

    Tamgu* Value(double v) {
        locking();
        try {
            Tamgu* res = globalTamgu->Providestring(values.at(v));
            unlocking();
            return res;
        }
        catch(const std::out_of_range& oor) {
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

    Exporting Tamgu* same(Tamgu* a);

};

//--------------------------------------------------------------------------------------------------
class TamguIterationtreemapfs : public TamguIteration {
    public:

    map<double, string>::iterator it;
    Tamgutreemapfs map;

    TamguIterationtreemapfs(Tamgutreemapfs* m, bool d, TamguGlobal* g = NULL) : TamguIteration(d, g) {
        map.values = m->values;
    }

    
    void Setvalue(Tamgu* recipient, short idthread) {
        recipient->storevalue(it->first);
    }

    Tamgu* Key() {
        return globalTamgu->Providefloat(it->first);
    }

    
    

    Tamgu* Value() {
        return globalTamgu->Providestring(it->second);
    }

    string Keystring() {
        
        return convertfromnumber(it->first);
        
    }

    string Valuestring() {
        return it->second;
    }

    
    BLONG Keylong() {
        return it->first;
    }

    long Keyinteger() {
        return it->first;
    }

    long Valueinteger() {
        return convertinteger(it->second);
    }

    double Keyfloat() {
        return it->first;
    }

    double Valuefloat() {
        return convertfloat(it->second);
    }

    void Next() {
        it++;
    }

    Tamgu* End() {
        if (it == map.values.end())
            return aTRUE;
        return aFALSE;
    }

    Tamgu* Begin() {
        it = map.values.begin();
        return aTRUE;
    }

};


//-------------------------------------------------------------------------------------------

#endif
