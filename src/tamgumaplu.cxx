/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumaplu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgumaplu.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapluMethod>  Tamgumaplu::methods;
static ThreadLock classlock;

Exporting short Tamgumaplu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumaplu::AddMethod(TamguGlobal* global, string name,mapluMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgumaplu::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgumaplu::methods.isEmpty())
    Tamgumaplu::InitialisationModule(global,"");
}


   bool Tamgumaplu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    
    Tamgumaplu::idtype = global->Getid("maplu");

    
    

    Tamgumaplu::AddMethod(global, "clear", &Tamgumaplu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumaplu::AddMethod(global, "invert", &Tamgumaplu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumaplu::AddMethod(global, "find", &Tamgumaplu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumaplu::AddMethod(global, "items", &Tamgumaplu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumaplu::AddMethod(global, "join", &Tamgumaplu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumaplu::AddMethod(global, "test", &Tamgumaplu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumaplu::AddMethod(global, "keys", &Tamgumaplu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumaplu::AddMethod(global, "values", &Tamgumaplu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumaplu::AddMethod(global, "pop", &Tamgumaplu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumaplu::AddMethod(global, "merge", &Tamgumaplu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->returnindextypes[Tamgumaplu::idtype] = a_ustring;        
    global->minimal_indexes[Tamgumaplu::idtype] = true;

        global->newInstance[Tamgumaplu::idtype] = new Tamgumaplu(global);
        
        global->RecordCompatibilities(Tamgumaplu::idtype);
    }

    return true;
}


Exporting TamguIteration* Tamgumaplu::Newiteration(bool direction) {
    return new TamguIterationmaplu(this, direction);
}


Exporting Tamgu* Tamgumaplu::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    BLONG val = a->Long();

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return globalTamgu->Providelong(val);
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


Exporting Tamgu* Tamgumaplu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    wstring val = callfunc->Evaluate(0, context, idthread)->UString();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (const auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
        Doublelocking _lock(this, v);
        for (const auto& it : values) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (const auto& it : values) {
        if (it.second == val)
            return globalTamgu->Providelong(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumaplu::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    values[s] = v->UString();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumaplu::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumaplu::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumaplu::String() {
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

Exporting string Tamgumaplu::JSonString() {
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


Exporting long Tamgumaplu::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumaplu::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumaplu::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumaplu::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumaplu::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumaplu::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumaplu::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumaplu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valueustring();
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

            for (const auto& it : kvect->values) {
                values[nb] = it->UString();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgumaplu::idtype)
            values = ((Tamgumaplu*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keylong()] = itr->Valueustring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getlong(idthread)] = ke->UString();
    unlocking();
    return aTRUE;
}



Tamgu* Tamgumaplu::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    BLONG skey = key->Getlong(idthread);

    Tamgu* val = Value(skey);
    if (val == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return val;
}

Exporting Tamgu* Tamgumaplu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<BLONG,wstring>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providelong(it->first));
            unlocking();
            return vect;
        }

        if (contextualpattern->Type() == a_int || contextualpattern->Type() == a_float)
            return globalTamgu->Providelong(values.size());

        return this;
    }

    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgumaplu* kmap = new Tamgumaplu;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        hmap<BLONG,wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<BLONG, wstring>::iterator itr= values.end();
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

    BLONG skey = idx->Getlong(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumaplu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumaplu* m = (Tamgumaplu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<BLONG,wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumaplu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgumaplu* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumaplu;
        hmap<BLONG, wstring> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueustring();
            else {
                if (values[v]==itr->Valueustring())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (const auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }

    if (itself)
        res = this;
    else
        res = (Tamgumaplu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringXor(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumaplu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumaplu* res;
    if (itself)
        res = this;
    else
        res = (Tamgumaplu*)Atom(true);

    if (b->isMapContainer()) {
        hmap<BLONG,wstring>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumaplu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringOr(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumaplu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumaplu* res;
    if (b->isMapContainer()) {
        hmap<BLONG,wstring>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumaplu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumaplu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringAnd(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumaplu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumaplu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumaplu;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumaplu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumaplu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumaplu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumaplu;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
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
        res = (Tamgumaplu*)Atom(true);

    wstring v = b->UString();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumaplu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<BLONG, wstring>::iterator it;
    
    Tamgu* a;
    vector<BLONG> keys;

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

