/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemaplf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgutreemaplf.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemaplfMethod>  Tamgutreemaplf::methods;
Exporting map<string, string> Tamgutreemaplf::infomethods;
Exporting basebin_hash<unsigned long> Tamgutreemaplf::exported;

Exporting short Tamgutreemaplf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemaplf::AddMethod(TamguGlobal* global, string name, treemaplfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutreemaplf::Setidtype(TamguGlobal* global) {
    Tamgutreemaplf::InitialisationModule(global,"");
}


   bool Tamgutreemaplf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgutreemaplf::idtype = global->Getid("treemaplf");

    Tamgutreemaplf::AddMethod(global, "clear", &Tamgutreemaplf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemaplf::AddMethod(global, "invert", &Tamgutreemaplf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemaplf::AddMethod(global, "find", &Tamgutreemaplf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemaplf::AddMethod(global, "items", &Tamgutreemaplf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemaplf::AddMethod(global, "join", &Tamgutreemaplf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemaplf::AddMethod(global, "test", &Tamgutreemaplf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemaplf::AddMethod(global, "keys", &Tamgutreemaplf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemaplf::AddMethod(global, "values", &Tamgutreemaplf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemaplf::AddMethod(global, "sum", &Tamgutreemaplf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemaplf::AddMethod(global, "product", &Tamgutreemaplf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemaplf::AddMethod(global, "pop", &Tamgutreemaplf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemaplf::AddMethod(global, "merge", &Tamgutreemaplf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgutreemaplf::idtype] = new Tamgutreemaplf(global);
        
        global->RecordMethods(Tamgutreemaplf::idtype, Tamgutreemaplf::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemaplf::Newiteration(bool direction) {
    return new TamguIterationtreemaplf(this, direction);
}


Exporting Tamgu* Tamgutreemaplf::in(Tamgu* context, Tamgu* a, short idthread) {
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


Exporting Tamgu* Tamgutreemaplf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
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
            return globalTamgu->Providelong(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgutreemaplf::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemaplf::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemaplf::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemaplf::String() {
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

Exporting string Tamgutreemaplf::JSonString() {
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


Exporting long Tamgutreemaplf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemaplf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemaplf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemaplf::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemaplf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgutreemaplf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemaplf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemaplf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keylong()] = itr->Valuefloat();
            itr->Release();
            return aTRUE;
        }
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getfloat(it);
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
                values[nb] = it->Float();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgutreemaplf::idtype)
            values = ((Tamgutreemaplf*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keylong()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getlong(idthread)] = ke->Float();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgutreemaplf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            map<BLONG, double>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providelong(it->first));
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
        Tamgutreemaplf* kmap = new Tamgutreemaplf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        map<BLONG, double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<BLONG, double>::iterator itr = values.end();
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

Exporting Tamgu* Tamgutreemaplf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemaplf* m = (Tamgutreemaplf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<BLONG, double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemaplf::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgutreemaplf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        map<BLONG, double> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keylong();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuefloat();
            else {
                if (values[v]==itr->Valuefloat())
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
        res = (Tamgutreemaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : values)
        it.second = DoubleXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemaplf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemaplf*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemaplf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()] == v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemaplf*)Atom(true);
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemaplf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) + itr->Valuefloat();
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
        res = (Tamgutreemaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemaplf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) - itr->Valuefloat();
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
        res = (Tamgutreemaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgutreemaplf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) * itr->Valuefloat();
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
        res = (Tamgutreemaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgutreemaplf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = values.at(k) / itr->Valuefloat();
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
        res = (Tamgutreemaplf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgutreemaplf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = (long)values.at(k) % itr->Valueinteger();
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
        res = (Tamgutreemaplf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Tamgu* Tamgutreemaplf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = DoubleShiftright(values.at(k), itr->Valueinteger());
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
        res = (Tamgutreemaplf*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftright(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemaplf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
        BLONG k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keylong();
            try {
                res->values[k] = DoubleShiftleft(values.at(k), itr->Valueinteger());
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
        res = (Tamgutreemaplf*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftleft(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemaplf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemaplf;
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
        res = (Tamgutreemaplf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemaplf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    map<BLONG, double>::iterator it;
    
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

