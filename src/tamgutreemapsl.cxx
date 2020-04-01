/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgutreemapsl.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgusvector.h"
#include "tamgustring.h"
//#prime
#include "tamgutreemapsl.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<treemapslMethod>  Tamgutreemapsl::methods;
Exporting hmap<string, string> Tamgutreemapsl::infomethods;
Exporting bin_hash<unsigned long> Tamgutreemapsl::exported;

Exporting short Tamgutreemapsl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgutreemapsl::AddMethod(TamguGlobal* global, string name,treemapslMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgutreemapsl::Setidtype(TamguGlobal* global) {
    Tamgutreemapsl::InitialisationModule(global,"");
}


   bool Tamgutreemapsl::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgutreemapsl::idtype = global->Getid("treemapsl");

    Tamgutreemapsl::AddMethod(global, "clear", &Tamgutreemapsl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgutreemapsl::AddMethod(global, "invert", &Tamgutreemapsl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgutreemapsl::AddMethod(global, "find", &Tamgutreemapsl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgutreemapsl::AddMethod(global, "items", &Tamgutreemapsl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgutreemapsl::AddMethod(global, "join", &Tamgutreemapsl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgutreemapsl::AddMethod(global, "test", &Tamgutreemapsl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgutreemapsl::AddMethod(global, "keys", &Tamgutreemapsl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgutreemapsl::AddMethod(global, "values", &Tamgutreemapsl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgutreemapsl::AddMethod(global, "sum", &Tamgutreemapsl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgutreemapsl::AddMethod(global, "product", &Tamgutreemapsl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgutreemapsl::AddMethod(global, "pop", &Tamgutreemapsl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgutreemapsl::AddMethod(global, "merge", &Tamgutreemapsl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->newInstance[Tamgutreemapsl::idtype] = new Tamgutreemapsl(global);
        
        global->RecordMethods(Tamgutreemapsl::idtype, Tamgutreemapsl::exported);
    }

    return true;
}


Exporting TamguIteration* Tamgutreemapsl::Newiteration(bool direction) {
    return new TamguIterationtreemapsl(this, direction);
}


Exporting Tamgu* Tamgutreemapsl::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    string val = a->String();

    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Doublelocking _lock(this, v);
        try {
            values.at(val);
            v->values.push_back(val);
        }
        catch(const std::out_of_range& oor) {}

        return v;
    }

    if (context->isString()) {
        locking();
        try {
            values.at(val);
            unlocking();
            return globalTamgu->Providewithstring(val);
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



Exporting Tamgu* Tamgutreemapsl::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
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



Exporting Tamgu* Tamgutreemapsl::Push(Tamgu* k, Tamgu* v) {
    locking();
    string s = k->String();
    values[s] = v->Long();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapsl::Pop(Tamgu* kkey) {
    string k = kkey->String();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgutreemapsl::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgutreemapsl::String() {
    locking();
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
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgutreemapsl::JSonString() {
    locking();
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
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgutreemapsl::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgutreemapsl::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgutreemapsl::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgutreemapsl::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgutreemapsl::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgutreemapsl::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgutreemapsl::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgutreemapsl*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keystring()] = itr->Valuelong();
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
                values[buff] = ke->getinteger(it);
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
                values[buff] = it->Long();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgutreemapsl::idtype)
            values = ((Tamgutreemapsl*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keystring()] = itr->Valuelong();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    locking();
    values[skey] = ke->Long();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgutreemapsl::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            map<string,BLONG>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Providestring(it->first));
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
        Tamgutreemapsl* kmap = new Tamgutreemapsl;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        map<string,BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        map<string, BLONG>::iterator itr= values.end();
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

    string skey;
    ((TamguIndex*)idx)->left->Setstring(skey, idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamgutreemapsl::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgutreemapsl* m = (Tamgutreemapsl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    map<string,BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgutreemapsl::xorset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);
    
    Tamgutreemapsl* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        hmap<string, BLONG> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    long v = b->Integer();
    for (auto& it : values)
        it.second = it.second ^ v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl* res;
    if (itself)
        res = this;
    else
        res = (Tamgutreemapsl*)Atom(true);
    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second | v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        res = new Tamgutreemapsl;
        BLONG v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
            if (values.find(itr->Keystring()) != values.end() && values[itr->Keystring()]==v)
                res->values[itr->Keystring()] = v;
        }
        itr->Release();
        return res;
    }
    if (itself)
        res = this;
    else
        res = (Tamgutreemapsl*)Atom(true);
    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second & v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second -= v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    BLONG v = b->Long();
    for (auto& it : res->values)
        it.second *= v;
    return res;

}

Exporting Tamgu* Tamgutreemapsl::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second /= v;
    return res;

}

Exporting Tamgu* Tamgutreemapsl::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (auto& it : res->values)
        it.second = it.second % v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second >> v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    long v = b->Integer();
    for (auto& it : res->values)
        it.second = it.second << v;
    return res;
}

Exporting Tamgu* Tamgutreemapsl::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgutreemapsl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgutreemapsl;
        string k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keystring();
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
        res = (Tamgutreemapsl*)Atom(true);

    double v = b->Float();
    for (auto& it : res->values)
        it.second = pow(it.second, v);
    return res;
}

Exporting Tamgu* Tamgutreemapsl::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    map<string, BLONG>::iterator it;
    
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

    unlocking();
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}

