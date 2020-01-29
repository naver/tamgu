/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapsf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
//#prime
#include "tamgumapsf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapsfMethod>  Tamgumapsf::methods;
Exporting hmap<string, string> Tamgumapsf::infomethods;
Exporting bin_hash<unsigned long> Tamgumapsf::exported;

Exporting short Tamgumapsf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapsf::AddMethod(TamguGlobal* global, string name,mapsfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapsf::Setidtype(TamguGlobal* global) {
        Tamgumapsf::idtype = global->Getid("mapsf");
    }

   bool Tamgumapsf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgumapsf::idtype = global->Getid("mapsf");

    Tamgumapsf::AddMethod(global, "clear", &Tamgumapsf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapsf::AddMethod(global, "invert", &Tamgumapsf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapsf::AddMethod(global, "find", &Tamgumapsf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapsf::AddMethod(global, "items", &Tamgumapsf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapsf::AddMethod(global, "join", &Tamgumapsf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapsf::AddMethod(global, "test", &Tamgumapsf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapsf::AddMethod(global, "keys", &Tamgumapsf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapsf::AddMethod(global, "values", &Tamgumapsf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapsf::AddMethod(global, "sum", &Tamgumapsf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgumapsf::AddMethod(global, "product", &Tamgumapsf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgumapsf::AddMethod(global, "pop", &Tamgumapsf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapsf::AddMethod(global, "merge", &Tamgumapsf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgumapsf::idtype] = new Tamgumapsf(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("sfmap")] = new Tamgumapsf(global);

    global->RecordMethods(global->Getid("sfmap"), Tamgumapsf::exported);
    #endif
    global->RecordMethods(Tamgumapsf::idtype, Tamgumapsf::exported);

    return true;
}


Exporting TamguIteration* Tamgumapsf::Newiteration(bool direction) {
    return new TamguIterationmapsf(this, direction);
}


Exporting Tamgu* Tamgumapsf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    string val = a->String();

     if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isString()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Providestring(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgumapsf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
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
            return globalTamgu->Providestring(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapsf::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    string s = k->String();
    values[s] = v->Float();
    return aTRUE;
}

Exporting Tamgu* Tamgumapsf::Pop(Tamgu* kkey) {
    string k = kkey->String();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgumapsf::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgumapsf::String() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        stringing(res, sx);
        res << ":" << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamgumapsf::JSonString() {
    Locking _lock(this);
    stringstream res;

    res << "{";
    bool beg = true;
    string sx;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;
        jstringing(res, sx);
        res << ":" << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamgumapsf::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgumapsf::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgumapsf::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgumapsf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgumapsf::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgumapsf::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapsf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapsf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valuefloat();
            itr->Release();
            return aTRUE;
        }
        char buff[20];
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(buff, 20, "%ld", nb);
                values[buff] = ke->getfloat(it);
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
                sprintf_s(buff, 20, "%ld", nb);
                values[buff] = it->Float();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        Locking* _lock = _getlock(this);
        values.clear();
        if (ke->Type() == Tamgumapsf::idtype)
            values = ((Tamgumapsf*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keystring()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Locking* _lock = _getlock(this);
    values[idx->String()] = ke->Float();
    _cleanlock(_lock);
    return aTRUE;
}


Exporting Tamgu* Tamgumapsf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            hmap<string,double>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providestring(it->first));
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
        Tamgumapsf* kmap = new Tamgumapsf;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        hmap<string,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<string, double>::iterator itr= values.end();
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

    string skey = key->String();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgumapsf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapsf* m = (Tamgumapsf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<string,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapsf::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgumapsf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        hmap<string, double> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
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
        res = (Tamgumapsf*)Atom(true);

    double v = b->Float();
    for (auto& it : values)
        it.second = DoubleXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapsf*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgumapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgumapsf*)Atom(true);
    double v = b->Float();
    for (auto& it : res->values)
        it.second = DoubleAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf* res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapsf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgumapsf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgumapsf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgumapsf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = (long)it.second % v;
    return res;
}

Exporting Tamgu* Tamgumapsf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftright(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = DoubleShiftleft(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapsf * res;
    if (b->isMapContainer()) {
        hmap<string,double>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapsf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keystring());
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
        res = (Tamgumapsf*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapsf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<string, double>::iterator it;
    
    Tamgu* a;
    vector<string> keys;

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
