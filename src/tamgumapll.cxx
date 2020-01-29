/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapll.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgumapll.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapllMethod>  Tamgumapll::methods;
Exporting hmap<string, string> Tamgumapll::infomethods;
Exporting bin_hash<unsigned long> Tamgumapll::exported;

Exporting short Tamgumapll::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapll::AddMethod(TamguGlobal* global, string name,mapllMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapll::Setidtype(TamguGlobal* global) {
        Tamgumapll::idtype = global->Getid("mapll");
    }

   bool Tamgumapll::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgumapll::idtype = global->Getid("mapll");

    Tamgumapll::AddMethod(global, "clear", &Tamgumapll::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapll::AddMethod(global, "invert", &Tamgumapll::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapll::AddMethod(global, "find", &Tamgumapll::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapll::AddMethod(global, "items", &Tamgumapll::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapll::AddMethod(global, "join", &Tamgumapll::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapll::AddMethod(global, "test", &Tamgumapll::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapll::AddMethod(global, "keys", &Tamgumapll::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapll::AddMethod(global, "values", &Tamgumapll::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapll::AddMethod(global, "sum", &Tamgumapll::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapll::AddMethod(global, "product", &Tamgumapll::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapll::AddMethod(global, "pop", &Tamgumapll::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapll::AddMethod(global, "merge", &Tamgumapll::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgumapll::idtype] = new Tamgumapll(global);
    
    global->RecordMethods(Tamgumapll::idtype, Tamgumapll::exported);

    return true;
}


Exporting TamguIteration* Tamgumapll::Newiteration(bool direction) {
    return new TamguIterationmapll(this, direction);
}


Exporting Tamgu* Tamgumapll::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
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
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return new Tamgulong(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgumapll::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    BLONG val = callfunc->Evaluate(0, context, idthread)->Long();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return new Tamgulong(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapll::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    BLONG s = k->Long();
    values[s] = v->Long();
    return aTRUE;
}

Exporting Tamgu* Tamgumapll::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgumapll::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgumapll::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":" << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamgumapll::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamgumapll::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgumapll::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgumapll::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgumapll::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgumapll::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgumapll::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapll::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapll*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valuelong();
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getinteger(it);
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
                values[nb] = it->Long();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        Locking* _lock = _getlock(this);
        values.clear();
        if (ke->Type() == Tamgumapll::idtype)
            values = ((Tamgumapll*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keylong()] = itr->Valuelong();
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Locking* _lock = _getlock(this);
    values[idx->Long()] = ke->Long();
    _cleanlock(_lock);
    return aTRUE;
}


Exporting Tamgu* Tamgumapll::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

    Locking _lock(this);

    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            hmap<BLONG,BLONG>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(new Tamgulong(it->first));
            return vect;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->Provideint(v);
        }

        return this;
    }

    Tamgu* key;
    if (idx->isInterval()) {
        Tamgumapll* kmap = new Tamgumapll;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        hmap<BLONG,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<BLONG, BLONG>::iterator itr= values.end();
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

    key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
    
    if (key == aNULL) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }

    BLONG skey = key->Long();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgumapll::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapll* m = (Tamgumapll*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<BLONG,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapll::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapll* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        hmap<BLONG, BLONG> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuelong();
            else {
                if (values[v]==itr->Valuelong())
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
        res = (Tamgumapll*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgumapll::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgumapll::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapll;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgumapll::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll* res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Tamgumapll*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapll::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = it->second - v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapll::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = it->second * v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgumapll::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = it->second / v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgumapll::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = it->second % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapll::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = (BLONG)it->second>> v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgumapll::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = it->second << v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgumapll::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapll * res;
    if (b->isMapContainer()) {
        hmap<BLONG,BLONG>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapll;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = pow(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapll*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapll::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<BLONG, BLONG>::iterator it;
    
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
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}
