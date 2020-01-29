/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapff.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufvector.h"
#include "tamgufloat.h"
//#prime
#include "tamgumapff.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapffMethod>  Tamgumapff::methods;
Exporting hmap<string, string> Tamgumapff::infomethods;
Exporting bin_hash<unsigned long> Tamgumapff::exported;

Exporting short Tamgumapff::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapff::AddMethod(TamguGlobal* global, string name, mapffMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapff::Setidtype(TamguGlobal* global) {
        Tamgumapff::idtype = global->Getid("mapff");
    }

   bool Tamgumapff::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgumapff::idtype = global->Getid("mapff");

    Tamgumapff::AddMethod(global, "clear", &Tamgumapff::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapff::AddMethod(global, "invert", &Tamgumapff::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapff::AddMethod(global, "find", &Tamgumapff::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapff::AddMethod(global, "items", &Tamgumapff::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapff::AddMethod(global, "join", &Tamgumapff::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapff::AddMethod(global, "test", &Tamgumapff::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapff::AddMethod(global, "keys", &Tamgumapff::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapff::AddMethod(global, "values", &Tamgumapff::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapff::AddMethod(global, "sum", &Tamgumapff::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapff::AddMethod(global, "product", &Tamgumapff::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapff::AddMethod(global, "pop", &Tamgumapff::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapff::AddMethod(global, "merge", &Tamgumapff::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgumapff::idtype] = new Tamgumapff(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ffmap")] = new Tamgumapff(global);

    global->RecordMethods(global->Getid("ffmap"), Tamgumapff::exported);
    #endif
    global->RecordMethods(Tamgumapff::idtype, Tamgumapff::exported);

    return true;
}


Exporting TamguIteration* Tamgumapff::Newiteration(bool direction) {
    return new TamguIterationmapff(this, direction);
}


Exporting Tamgu* Tamgumapff::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    double val = a->Float();

     if (context->isVectorContainer()) {
        Tamgufvector* v = (Tamgufvector*)Selectafvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Providefloat(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgumapff::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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



Exporting Tamgu* Tamgumapff::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    double s = k->Float();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Tamgu* Tamgumapff::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgumapff::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgumapff::String() {
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

Exporting string Tamgumapff::JSonString() {
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


Exporting long Tamgumapff::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgumapff::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgumapff::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgumapff::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgumapff::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgumapff::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapff::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapff*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuefloat();
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
        Locking* _lock = _getlock(this);
        values.clear();
        if (ke->Type() == Tamgumapff::idtype)
            values = ((Tamgumapff*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Locking* _lock = _getlock(this);
    values[idx->Float()] = ke->Float();
    _cleanlock(_lock);
    return aTRUE;
}


Exporting Tamgu* Tamgumapff::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            hmap<double, double>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providefloat(it->first));
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
        Tamgumapff* kmap = new Tamgumapff;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        hmap<double, double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<double, double>::iterator itr = values.end();
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

    double skey = key->Float();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgumapff::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapff* m = (Tamgumapff*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<double, double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapff::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    Tamgumapff* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        hmap<double, double> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
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
        res = (Tamgumapff*)Atom(true);

    double v = b->Float();
    for (auto& it : values)
        it.second = DoubleXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapff::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapff*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapff::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()] == v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapff*)Atom(true);
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapff::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff* res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgumapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapff::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgumapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapff::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgumapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgumapff::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keyfloat());
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
        res = (Tamgumapff*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgumapff::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapff*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapff::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = DoubleShiftright(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapff*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftright(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapff::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyfloat());
            if (it != values.end()) {
                res->values[it->first] = DoubleShiftleft(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapff*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftleft(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapff::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapff * res;
    if (b->isMapContainer()) {
        hmap<double, double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapff;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyfloat());
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
        res = (Tamgumapff*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapff::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<double, double>::iterator it;
    
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
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}
