/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapll.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgutreemapll.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapllMethod>  Tamgutreemapll::methods;
Exporting map<string, string> Tamgutreemapll::infomethods;
Exporting bin_hash<unsigned long> Tamgutreemapll::exported;

Exporting short Tamgutreemapll::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapll::AddMethod(TamguGlobal* global, string name,treemapllMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutreemapll::Setidtype(TamguGlobal* global) {
    Tamgutreemapll::InitialisationModule(global,"");
}


   bool Tamgutreemapll::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgutreemapll::idtype = global->Getid("treemapll");

    Tamgutreemapll::AddMethod(global, "clear", &Tamgutreemapll::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapll::AddMethod(global, "invert", &Tamgutreemapll::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapll::AddMethod(global, "find", &Tamgutreemapll::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapll::AddMethod(global, "items", &Tamgutreemapll::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapll::AddMethod(global, "join", &Tamgutreemapll::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapll::AddMethod(global, "test", &Tamgutreemapll::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapll::AddMethod(global, "keys", &Tamgutreemapll::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapll::AddMethod(global, "values", &Tamgutreemapll::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapll::AddMethod(global, "sum", &Tamgutreemapll::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapll::AddMethod(global, "product", &Tamgutreemapll::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapll::AddMethod(global, "pop", &Tamgutreemapll::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapll::AddMethod(global, "merge", &Tamgutreemapll::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgutreemapll::idtype] = new Tamgutreemapll(global);
        
        global->RecordMethods(Tamgutreemapll::idtype, Tamgutreemapll::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemapll::Newiteration(bool direction) {
    return new TamguIterationtreemapll(this, direction);
}


Exporting Tamgu* Tamgutreemapll::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
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
            return new Tamgulong(val);
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


Exporting Tamgu* Tamgutreemapll::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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



Exporting Tamgu* Tamgutreemapll::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    values[s] = v->Long();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapll::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemapll::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemapll::String() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgutreemapll::JSonString() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgutreemapll::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemapll::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemapll::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemapll::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemapll::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgutreemapll::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemapll::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemapll*)ke)->values;
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
        locking();
        values.clear();
        if (ke->Type() == Tamgutreemapll::idtype)
            values = ((Tamgutreemapll*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keylong()] = itr->Valuelong();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getlong(idthread)] = ke->Long();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapll::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            map<BLONG,BLONG>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(new Tamgulong(it->first));
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
        Tamgutreemapll* kmap = new Tamgutreemapll;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        map<BLONG,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<BLONG, BLONG>::iterator itr= values.end();
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

    BLONG skey = idx->Getlong(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgutreemapll::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapll* m = (Tamgutreemapll*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<BLONG,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapll::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapll* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        map<BLONG, BLONG> keys;

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
        res = (Tamgutreemapll*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapll*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapll;
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
        res = (Tamgutreemapll*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) + itr->Valuelong();
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
        res = (Tamgutreemapll*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) - itr->Valuelong();
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
        res = (Tamgutreemapll*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) * itr->Valuelong();
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
        res = (Tamgutreemapll*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgutreemapll::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) / itr->Valuelong();
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
        res = (Tamgutreemapll*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgutreemapll::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) % itr->Valueinteger();
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
        res = (Tamgutreemapll*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = (BLONG)values.at(k)>> itr->Valueinteger();
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
        res = (Tamgutreemapll*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) << itr->Valueinteger();
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
        res = (Tamgutreemapll*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgutreemapll::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapll * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapll;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = pow(values.at(k), itr->Valuefloat());
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
        res = (Tamgutreemapll*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapll::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    map<BLONG, BLONG>::iterator it;
    
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

