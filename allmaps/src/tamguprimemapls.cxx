/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapls.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamguprimemapls.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemaplsMethod>  Tamguprimemapls::methods;
Exporting hmap<string, string> Tamguprimemapls::infomethods;
Exporting basebin_hash<unsigned long> Tamguprimemapls::exported;

Exporting short Tamguprimemapls::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapls::AddMethod(TamguGlobal* global, string name,primemaplsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguprimemapls::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamguprimemapls::idtype = global->Getid("primemapls");

    Tamguprimemapls::AddMethod(global, "clear", &Tamguprimemapls::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapls::AddMethod(global, "invert", &Tamguprimemapls::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapls::AddMethod(global, "find", &Tamguprimemapls::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemapls::AddMethod(global, "items", &Tamguprimemapls::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemapls::AddMethod(global, "join", &Tamguprimemapls::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapls::AddMethod(global, "test", &Tamguprimemapls::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapls::AddMethod(global, "keys", &Tamguprimemapls::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapls::AddMethod(global, "values", &Tamguprimemapls::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapls::AddMethod(global, "pop", &Tamguprimemapls::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapls::AddMethod(global, "merge", &Tamguprimemapls::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamguprimemapls::idtype] = new Tamguprimemapls(global);
    
    global->RecordMethods(Tamguprimemapls::idtype, Tamguprimemapls::exported);

    return true;
}


Exporting TamguIteration* Tamguprimemapls::Newiteration(bool direction) {
    return new TamguIterationprimemapls(this, direction);
}


Exporting Tamgu* Tamguprimemapls::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    BLONG val = a->Long();

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

    if (context->isNumber()) {
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Providelong(val);
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


Exporting Tamgu* Tamguprimemapls::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases aBLONG the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<BLONG, string>::iterator it;
    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return globalTamgu->Providelong(it->first);
    }

    return aNULL;

}



Exporting Tamgu* Tamguprimemapls::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    values[s] = v->String();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapls::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamguprimemapls::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamguprimemapls::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        stringing(res, it->second);
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapls::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":";
        jstringing(res, it->second);
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapls::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamguprimemapls::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamguprimemapls::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamguprimemapls::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemapls::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamguprimemapls::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        
        if (ke->Type() == Tamguprimemapls::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapls*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valuestring();
            itr->Release();
            return aTRUE;
        }
        
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getstring(it);
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
                values[nb] = (*it)->String();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapls::idtype) {
            Tamguprimemapls* kmap = (Tamguprimemapls*)ke;
            //We copy all values from ke to this
            prime_hash<BLONG,string>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keylong()] = itr->Valuestring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }

    locking();
    values[idx->Getlong(idthread)] = ke->String();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapls::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            prime_hash<BLONG,string>::iterator it;
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
        Tamguprimemapls* kmap = new Tamguprimemapls;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        prime_hash<BLONG,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<BLONG, string>::iterator itr= values.end();
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
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamguprimemapls::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemapls* m = (Tamguprimemapls*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<BLONG,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapls::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamguprimemapls* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapls;
        hmap<BLONG, string> keys;

        prime_hash<BLONG, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuestring();
            else {
                if (values[v]==itr->Valuestring())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first]= a.second;

        return res;


    }

    prime_hash<BLONG, string>::iterator it;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringXor(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapls::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Tamguprimemapls* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapls*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringOr(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapls::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Tamguprimemapls* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringAnd(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapls::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Tamguprimemapls* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapls::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,string>::iterator it;
    Tamguprimemapls * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapls;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemapls*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapls::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    prime_hash<BLONG, string>::iterator it;
    
    Tamgu* a;
    vector<BLONG> keys;

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

