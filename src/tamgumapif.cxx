/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapif.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgumapif.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapifMethod>  Tamgumapif::methods;
Exporting hmap<string, string> Tamgumapif::infomethods;
Exporting basebin_hash<unsigned long> Tamgumapif::exported;

Exporting short Tamgumapif::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapif::AddMethod(TamguGlobal* global, string name, mapifMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapif::Setidtype(TamguGlobal* global) {
    Tamgumapif::InitialisationModule(global,"");
}


   bool Tamgumapif::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgumapif::idtype = global->Getid("mapif");

    
    global->minimal_indexes[Tamgumapif::idtype] = true;

    Tamgumapif::AddMethod(global, "clear", &Tamgumapif::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapif::AddMethod(global, "invert", &Tamgumapif::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapif::AddMethod(global, "find", &Tamgumapif::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapif::AddMethod(global, "items", &Tamgumapif::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapif::AddMethod(global, "join", &Tamgumapif::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapif::AddMethod(global, "test", &Tamgumapif::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapif::AddMethod(global, "keys", &Tamgumapif::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapif::AddMethod(global, "values", &Tamgumapif::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapif::AddMethod(global, "sum", &Tamgumapif::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapif::AddMethod(global, "product", &Tamgumapif::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapif::AddMethod(global, "pop", &Tamgumapif::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapif::AddMethod(global, "merge", &Tamgumapif::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgumapif::idtype] = new Tamgumapif(global);
        
        global->RecordMethods(Tamgumapif::idtype, Tamgumapif::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgumapif::Newiteration(bool direction) {
    return new TamguIterationmapif(this, direction);
}


Exporting Tamgu* Tamgumapif::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    long val = a->Integer();

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
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
            return globalTamgu->ProvideConstint(val);
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


Exporting Tamgu* Tamgumapif::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (const auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
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
            return globalTamgu->ProvideConstint(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapif::Push(Tamgu* k, Tamgu* v) {
    locking();
    long s = k->Integer();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumapif::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumapif::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumapif::String() {
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

Exporting string Tamgumapif::JSonString() {
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


Exporting long Tamgumapif::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapif::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapif::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapif::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapif::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapif::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapif::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapif*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyinteger()] = itr->Valuefloat();
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

            for (const auto& it : kvect->values) {
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
        if (ke->Type() == Tamgumapif::idtype)
            values = ((Tamgumapif*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyinteger()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getinteger(idthread)] = ke->Float();
    unlocking();
    return aTRUE;
}



Tamgu* Tamgumapif::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    long skey = key->Getinteger(idthread);

    Tamgu* val = Value(skey);
    if (val == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return val;
}

Exporting Tamgu* Tamgumapif::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<long, double>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->ProvideConstint(it->first));
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
        Tamgumapif* kmap = new Tamgumapif;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        hmap<long, double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<long, double>::iterator itr = values.end();
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

    long skey = idx->Getinteger(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgumapif::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapif* m = (Tamgumapif*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<long, double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapif::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgumapif* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        hmap<long, double> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuefloat();
            else {
                if (values[v]==itr->Valuefloat())
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
        res = (Tamgumapif*)Atom(true);

    double v = b->Float();
    for (auto& it : values)
        it.second = DoubleXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapif::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapif*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapif::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapif;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()] == v)
                res->values[itr->Keyinteger()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapif*)Atom(true);
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapif::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapif::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapif::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgumapif::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgumapif::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapif::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftright(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapif::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftleft(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapif::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapif * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapif;
        long k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyinteger();
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
        res = (Tamgumapif*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapif::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<long, double>::iterator it;
    
    Tamgu* a;
    vector<long> keys;

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

