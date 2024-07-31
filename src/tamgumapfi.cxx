/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapfi.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufvector.h"
#include "tamgufloat.h"
//#prime
#include "tamgumapfi.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapfiMethod>  Tamgumapfi::methods;
static ThreadLock classlock;

Exporting short Tamgumapfi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapfi::AddMethod(TamguGlobal* global, string name, mapfiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgumapfi::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgumapfi::methods.isEmpty())
    Tamgumapfi::InitialisationModule(global,"");
}


   bool Tamgumapfi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgumapfi::idtype = global->Getid("mapfi");

    
    

    Tamgumapfi::AddMethod(global, "clear", &Tamgumapfi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapfi::AddMethod(global, "invert", &Tamgumapfi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapfi::AddMethod(global, "find", &Tamgumapfi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapfi::AddMethod(global, "items", &Tamgumapfi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapfi::AddMethod(global, "join", &Tamgumapfi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapfi::AddMethod(global, "test", &Tamgumapfi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapfi::AddMethod(global, "keys", &Tamgumapfi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapfi::AddMethod(global, "values", &Tamgumapfi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapfi::AddMethod(global, "sum", &Tamgumapfi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapfi::AddMethod(global, "product", &Tamgumapfi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapfi::AddMethod(global, "pop", &Tamgumapfi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapfi::AddMethod(global, "merge", &Tamgumapfi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->returnindextypes[Tamgumapfi::idtype] = a_int;        
    global->minimal_indexes[Tamgumapfi::idtype] = true;

        global->newInstance[Tamgumapfi::idtype] = new Tamgumapfi(global);
        
        global->RecordCompatibilities(Tamgumapfi::idtype);
    }

    return true;
}


Exporting TamguIteration* Tamgumapfi::Newiteration(bool direction) {
    return new TamguIterationmapfi(this, direction);
}


Exporting Tamgu* Tamgumapfi::in(Tamgu* context, Tamgu* a, short idthread) {
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
            return globalTamgu->ProvideConstfloat(val);
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


Exporting Tamgu* Tamgumapfi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    long val = callfunc->Evaluate(0, context, idthread)->Integer();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (const auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
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
            return globalTamgu->ProvideConstfloat(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapfi::Push(Tamgu* k, Tamgu* v) {
    locking();
    double s = k->Float();
    values[s] = v->Integer();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumapfi::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumapfi::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumapfi::String() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (const auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgumapfi::JSonString() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (const auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":" << it.second;
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgumapfi::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapfi::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapfi::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapfi::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapfi::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapfi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapfi::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapfi*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valueinteger();
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

            for (const auto& it : kvect->values) {
                values[nb] = it->Integer();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgumapfi::idtype)
            values = ((Tamgumapfi*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyfloat()] = itr->Valueinteger();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getfloat(idthread)] = ke->Integer();
    unlocking();
    return aTRUE;
}



Tamgu* Tamgumapfi::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    double skey = key->Getfloat(idthread);

    Tamgu* val = Value(skey);
    if (val == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return val;
}

Exporting Tamgu* Tamgumapfi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


    if (!idx->isIndex()) {
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isMapContainer())
            return this;
        
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            locking();
            hmap<double, long>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providefloat(it->first));
            unlocking();
            return vect;
        }

        if (contextualpattern->isNumber()) {
            long v = Size();
            return globalTamgu->ProvideConstint(v);
        }

        return this;
    }

    if (idx->isInterval()) {
        Locking _lock(this);
        Tamgumapfi* kmap = new Tamgumapfi;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        hmap<double, long>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<double, long>::iterator itr = values.end();
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
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapfi::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapfi* m = (Tamgumapfi*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<double, long>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapfi::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgumapfi* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
        hmap<double, long> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valueinteger();
            else {
                if (values[v]==itr->Valueinteger())
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
        res = (Tamgumapfi*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgumapfi::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapfi*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgumapfi::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapfi;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()] == v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapfi*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgumapfi::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = values.at(k) + itr->Valueinteger();
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
        res = (Tamgumapfi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapfi::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = values.at(k) - itr->Valueinteger();
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
        res = (Tamgumapfi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapfi::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = values.at(k) * itr->Valueinteger();
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
        res = (Tamgumapfi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgumapfi::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    long v;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                itr->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            try {
                res->values[k] = values.at(k) / v;
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
        res = (Tamgumapfi*)Atom(true);

    v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgumapfi::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    long v;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                itr->Release();
                return globalTamgu->Returnerror(e_error_divided_by);
            }
            try {
                res->values[k] = values.at(k) % v;
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
        res = (Tamgumapfi*)Atom(true);

    v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror(e_error_divided_by);
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapfi::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
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
        res = (Tamgumapfi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgumapfi::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
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
        res = (Tamgumapfi*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgumapfi::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfi;
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
        res = (Tamgumapfi*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<double, long>::iterator it;
    
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

