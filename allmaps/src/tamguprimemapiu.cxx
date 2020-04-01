/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapiu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamguprimemapiu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapiuMethod>  Tamguprimemapiu::methods;
Exporting hmap<string, string> Tamguprimemapiu::infomethods;
Exporting bin_hash<unsigned long> Tamguprimemapiu::exported;

Exporting short Tamguprimemapiu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapiu::AddMethod(TamguGlobal* global, string name,primemapiuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguprimemapiu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamguprimemapiu::idtype = global->Getid("primemapiu");

    Tamguprimemapiu::AddMethod(global, "clear", &Tamguprimemapiu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapiu::AddMethod(global, "invert", &Tamguprimemapiu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapiu::AddMethod(global, "find", &Tamguprimemapiu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemapiu::AddMethod(global, "items", &Tamguprimemapiu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemapiu::AddMethod(global, "join", &Tamguprimemapiu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapiu::AddMethod(global, "test", &Tamguprimemapiu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapiu::AddMethod(global, "keys", &Tamguprimemapiu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapiu::AddMethod(global, "values", &Tamguprimemapiu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapiu::AddMethod(global, "pop", &Tamguprimemapiu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapiu::AddMethod(global, "merge", &Tamguprimemapiu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamguprimemapiu::idtype] = new Tamguprimemapiu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iuprimemap")] = new Tamguprimemapiu(global);

    global->RecordMethods(global->Getid("iuprimemap"), Tamguprimemapiu::exported);
    #endif
    global->RecordMethods(Tamguprimemapiu::idtype, Tamguprimemapiu::exported);

    return true;
}


Exporting TamguIteration* Tamguprimemapiu::Newiteration(bool direction) {
    return new TamguIterationprimemapiu(this, direction);
}


Exporting Tamgu* Tamguprimemapiu::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    long val = a->Integer();

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

    if (context->isNumber()) {
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Provideint(val);
        }
        unlocking();
        return aNOELEMENT;;
    }
    
    locking();
    if (values.find(val)!=values.end()) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;

}


Exporting Tamgu* Tamguprimemapiu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<long, wstring>::iterator it;
    
    wstring val = callfunc->Evaluate(0, context, idthread)->UString();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                v->values.push_back(it->first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (it = values.begin(); it != values.end(); it++) {
        if (it->second == val)
            return globalTamgu->Provideint(it->first);
    }

    return aNULL;

}



Exporting Tamgu* Tamguprimemapiu::Push(Tamgu* k, Tamgu* v) {
    locking();
    long s = k->Integer();
    values[s] = v->UString();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapiu::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamguprimemapiu::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamguprimemapiu::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long,wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapiu::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long,wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapiu::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamguprimemapiu::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamguprimemapiu::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamguprimemapiu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemapiu::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamguprimemapiu::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        
        if (ke->Type() == Tamguprimemapiu::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapiu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyinteger()] = itr->Valueustring();
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                values[nb] = (*it)->UString();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapiu::idtype) {
            Tamguprimemapiu* kmap = (Tamguprimemapiu*)ke;
            //We copy all values from ke to this
            prime_hash<long,wstring>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyinteger()] = itr->Valueustring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }

    locking();
    values[idx->Getinteger(idthread)] = ke->UString();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapiu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            prime_hash<long,wstring>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideint(it->first));
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
        Tamguprimemapiu* kmap = new Tamguprimemapiu;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        prime_hash<long,wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<long, wstring>::iterator itr= values.end();
        if (keyright != aNULL) {
            itr = values.find(vright);
            if (itr == values.end()) {
                keyright->Release();
                return kmap;
            }
        }
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

    long skey = idx->Getinteger(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamguprimemapiu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemapiu* m = (Tamguprimemapiu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<long,wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapiu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamguprimemapiu* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapiu;
        hmap<long, wstring> keys;

        prime_hash<long, wstring>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
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

    prime_hash<long, wstring>::iterator it;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapiu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringXor(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapiu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,wstring>::iterator it;
    Tamguprimemapiu* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapiu*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapiu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringOr(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapiu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,wstring>::iterator it;
    Tamguprimemapiu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapiu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()]==v)
                res->values[itr->Keyinteger()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapiu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringAnd(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapiu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,wstring>::iterator it;
    Tamguprimemapiu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapiu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->values[it->first] = it->second + v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapiu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapiu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,wstring>::iterator it;
    Tamguprimemapiu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapiu;
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->values[it->first] = StringMinus(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapiu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapiu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    prime_hash<long, wstring>::iterator it;
    
    Tamgu* a;
    vector<long> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    a = aNULL;
    bool testcond = false;

    long sz = keys.size();
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

