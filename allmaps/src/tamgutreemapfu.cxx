/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapfu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufvector.h"
#include "tamgufloat.h"
//#prime
#include "tamgutreemapfu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapfuMethod>  Tamgutreemapfu::methods;
static ThreadLock classlock;

Exporting short Tamgutreemapfu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapfu::AddMethod(TamguGlobal* global, string name, treemapfuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgutreemapfu::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgutreemapfu::methods.isEmpty())
    Tamgutreemapfu::InitialisationModule(global,"");
}


   bool Tamgutreemapfu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgutreemapfu::idtype = global->Getid("treemapfu");

    Tamgutreemapfu::AddMethod(global, "clear", &Tamgutreemapfu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapfu::AddMethod(global, "invert", &Tamgutreemapfu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapfu::AddMethod(global, "find", &Tamgutreemapfu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapfu::AddMethod(global, "items", &Tamgutreemapfu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapfu::AddMethod(global, "join", &Tamgutreemapfu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapfu::AddMethod(global, "test", &Tamgutreemapfu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapfu::AddMethod(global, "keys", &Tamgutreemapfu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapfu::AddMethod(global, "values", &Tamgutreemapfu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapfu::AddMethod(global, "pop", &Tamgutreemapfu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapfu::AddMethod(global, "merge", &Tamgutreemapfu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->returnindextypes[Tamgutreemapfu::idtype] = a_ustring;
    global->newInstance[Tamgutreemapfu::idtype] = new Tamgutreemapfu(global);
        
        global->RecordCompatibilities(Tamgutreemapfu::idtype);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemapfu::Newiteration(bool direction) {
    return new TamguIterationtreemapfu(this, direction);
}


Exporting Tamgu* Tamgutreemapfu::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    double val = a->Float();

    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        try {
            values.at(val);
            v->values.push_back(val);
        }
        catch(const std::out_of_range& oor) {}

        return v;
    }

    if (context->isNumber()) {
        locking();
        try {
            values.at(val);
            unlocking();
            return globalTamgu->Providefloat(val);
        }
        catch(const std::out_of_range& oor) {
            unlocking();
            return aNOELEMENT;
        }
    }
    
    locking();
    try {
        values.at(val);
        unlocking();
        return aTRUE;
    }
    catch(const std::out_of_range& oor) {
        unlocking();
        return aFALSE;
    }

}


Exporting Tamgu* Tamgutreemapfu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    wstring val = callfunc->Evaluate(0, context, idthread)->UString();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        for (auto& it : values) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (auto& it : values) {
        if (it.second == val)
            return globalTamgu->Providefloat(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgutreemapfu::Push(Tamgu* k, Tamgu* v) {
    locking();
    double s = k->Float();
    values[s] = v->UString();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapfu::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemapfu::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemapfu::String() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgutreemapfu::JSonString() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":";
        w = it.second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgutreemapfu::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemapfu::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemapfu::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemapfu::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemapfu::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgutreemapfu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemapfu::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemapfu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getustring(it);
                nb++;
            }
            return aTRUE;
        }
        if (ke->Type() == a_list) {
            Doublelocking _lock(this, ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            values.clear();
            long nb = 0;

            for (auto& it : kvect->values) {
                values[nb] = it->UString();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgutreemapfu::idtype)
            values = ((Tamgutreemapfu*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyfloat()] = itr->Valueustring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getfloat(idthread)] = ke->UString();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapfu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            locking();
            map<double, wstring>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providefloat(it->first));
            unlocking();
            return vect;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }

        return this;
    }

    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgutreemapfu* kmap = new Tamgutreemapfu;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        map<double, wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<double, wstring>::iterator itr = values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                if (keyright != ((TamguIndex*)idx)->right)
                    keyright->Release();
                return kmap;
            }
        }
        if (keyright != ((TamguIndex*)idx)->right)
            keyright->Release();
        for (; it != values.end(); it++) {
            kmap->values[it->first] = it->second;
            if (it == itr)
                return kmap;
        }

        if (itr != values.end())
            kmap->values.clear();
        return kmap;

    }

    double skey = idx->Getfloat(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgutreemapfu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapfu* m = (Tamgutreemapfu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<double, wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapfu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgutreemapfu* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfu;
        map<double, wstring> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueustring();
            else {
                if (values[v]==itr->Valueustring())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapfu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfu* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapfu*)Atom(true);

    if (b->isMapContainer()) {
        map<double, wstring>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapfu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfu* res;
    if (b->isMapContainer()) {
        map<double, wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()] == v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgutreemapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapfu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfu;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = values.at(k) + itr->Valueustring();
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }


    if (itself)
        res = this;
    else
        res = (Tamgutreemapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemapfu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfu;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = StringMinus(values.at(k), itr->Valueustring());
            }
            catch (const std::out_of_range& oor) {
            }
        }
        itr->Release();
        return res;
    }


    if (itself)
        res = this;
    else
        res = (Tamgutreemapfu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringMinus(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapfu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    map<double, wstring>::iterator it;
    
    Tamgu* a;
    vector<double> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    long sz = keys.size();
    a = aNULL;
    bool testcond = false;
    for (long i = 0; i < sz && !testcond; i++) {
        a->Releasenonconst();
        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        testcond = a->needInvestigate();
    }

    unlocking();
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}

