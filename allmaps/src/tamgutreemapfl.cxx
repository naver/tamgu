/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapfl.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufvector.h"
#include "tamgufloat.h"
//#prime
#include "tamgutreemapfl.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapflMethod>  Tamgutreemapfl::methods;
static ThreadLock classlock;

Exporting short Tamgutreemapfl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapfl::AddMethod(TamguGlobal* global, string name, treemapflMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgutreemapfl::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgutreemapfl::methods.isEmpty())
    Tamgutreemapfl::InitialisationModule(global,"");
}


   bool Tamgutreemapfl::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgutreemapfl::idtype = global->Getid("treemapfl");

    Tamgutreemapfl::AddMethod(global, "clear", &Tamgutreemapfl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapfl::AddMethod(global, "invert", &Tamgutreemapfl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapfl::AddMethod(global, "find", &Tamgutreemapfl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapfl::AddMethod(global, "items", &Tamgutreemapfl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapfl::AddMethod(global, "join", &Tamgutreemapfl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapfl::AddMethod(global, "test", &Tamgutreemapfl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapfl::AddMethod(global, "keys", &Tamgutreemapfl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapfl::AddMethod(global, "values", &Tamgutreemapfl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapfl::AddMethod(global, "sum", &Tamgutreemapfl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapfl::AddMethod(global, "product", &Tamgutreemapfl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapfl::AddMethod(global, "pop", &Tamgutreemapfl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapfl::AddMethod(global, "merge", &Tamgutreemapfl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->returnindextypes[Tamgutreemapfl::idtype] = a_long;
    global->newInstance[Tamgutreemapfl::idtype] = new Tamgutreemapfl(global);
        
        global->RecordCompatibilities(Tamgutreemapfl::idtype);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemapfl::Newiteration(bool direction) {
    return new TamguIterationtreemapfl(this, direction);
}


Exporting Tamgu* Tamgutreemapfl::in(Tamgu* context, Tamgu* a, short idthread) {
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


Exporting Tamgu* Tamgutreemapfl::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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



Exporting Tamgu* Tamgutreemapfl::Push(Tamgu* k, Tamgu* v) {
    locking();
    double s = k->Float();
    values[s] = v->Long();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapfl::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemapfl::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemapfl::String() {
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

Exporting string Tamgutreemapfl::JSonString() {
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


Exporting long Tamgutreemapfl::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemapfl::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemapfl::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemapfl::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemapfl::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgutreemapfl::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemapfl::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemapfl*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuelong();
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
        if (ke->Type() == Tamgutreemapfl::idtype)
            values = ((Tamgutreemapfl*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuelong();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getfloat(idthread)] = ke->Long();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapfl::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            map<double, BLONG>::iterator it;
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
        Tamgutreemapfl* kmap = new Tamgutreemapfl;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        map<double, BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<double, BLONG>::iterator itr = values.end();
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

Exporting Tamgu* Tamgutreemapfl::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapfl* m = (Tamgutreemapfl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<double, BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapfl::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgutreemapfl* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        map<double, BLONG> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapfl*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapfl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()] == v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemapfl*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgutreemapfl::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgutreemapfl::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = values.at(k) >> itr->Valueinteger();
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
        res = (Tamgutreemapfl*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgutreemapfl::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapfl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapfl;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
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
        res = (Tamgutreemapfl*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapfl::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    map<double, BLONG>::iterator it;
    
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

