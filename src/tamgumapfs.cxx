/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapfs.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgufvector.h"
#include "tamgufloat.h"
//#prime
#include "tamgumapfs.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapfsMethod>  Tamgumapfs::methods;
static ThreadLock classlock;

Exporting short Tamgumapfs::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapfs::AddMethod(TamguGlobal* global, string name, mapfsMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}




    void Tamgumapfs::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgumapfs::methods.isEmpty())
    Tamgumapfs::InitialisationModule(global,"");
}


   bool Tamgumapfs::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgumapfs::idtype = global->Getid("mapfs");

    
    

    Tamgumapfs::AddMethod(global, "clear", &Tamgumapfs::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapfs::AddMethod(global, "invert", &Tamgumapfs::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapfs::AddMethod(global, "find", &Tamgumapfs::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapfs::AddMethod(global, "items", &Tamgumapfs::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapfs::AddMethod(global, "join", &Tamgumapfs::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapfs::AddMethod(global, "test", &Tamgumapfs::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapfs::AddMethod(global, "keys", &Tamgumapfs::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapfs::AddMethod(global, "values", &Tamgumapfs::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapfs::AddMethod(global, "pop", &Tamgumapfs::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapfs::AddMethod(global, "merge", &Tamgumapfs::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    if (version != "") {
        global->returnindextypes[Tamgumapfs::idtype] = a_string;        
    global->minimal_indexes[Tamgumapfs::idtype] = true;

        global->newInstance[Tamgumapfs::idtype] = new Tamgumapfs(global);
        
        global->RecordCompatibilities(Tamgumapfs::idtype);
    }

    return true;
}


Exporting TamguIteration* Tamgumapfs::Newiteration(bool direction) {
    return new TamguIterationmapfs(this, direction);
}


Exporting Tamgu* Tamgumapfs::in(Tamgu* context, Tamgu* a, short idthread) {
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


Exporting Tamgu* Tamgumapfs::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
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



Exporting Tamgu* Tamgumapfs::Push(Tamgu* k, Tamgu* v) {
    locking();
    double s = k->Float();
    values[s] = v->String();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgumapfs::Pop(Tamgu* kkey) {
    double k = kkey->Float();
    locking();
    if (values.erase(k)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;
}

Exporting void Tamgumapfs::Clear() {
    locking();
    values.clear();
    unlocking();
}



Exporting string Tamgumapfs::String() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it.first << ":";
        stringing(res, it.second);
    }
    unlocking();
    res << "}";
    return res.str();
}

Exporting string Tamgumapfs::JSonString() {
    locking();
    stringstream res;

    res << "{";
    bool beg = true;
    for (auto& it : values) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it.first << '"' << ":";
        jstringing(res, it.second);
    }
    unlocking();
    res << "}";
    return res.str();
}


Exporting long Tamgumapfs::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting double Tamgumapfs::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting BLONG Tamgumapfs::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}

Exporting bool Tamgumapfs::Boolean() {
    locking();
    bool b = values.empty();
    unlocking();
    return !b;
}


//Basic operations
Exporting long Tamgumapfs::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting Tamgu*  Tamgumapfs::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapfs::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapfs*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuestring();
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

            for (const auto& it : kvect->values) {
                values[nb] = it->String();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror(e_wrong_map_initialization, idthread);
        locking();
        values.clear();
        if (ke->Type() == Tamgumapfs::idtype)
            values = ((Tamgumapfs*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyfloat()] = itr->Valuestring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }
    locking();
    values[idx->Getfloat(idthread)] = ke->String();
    unlocking();
    return aTRUE;
}



Tamgu* Tamgumapfs::EvalWithSimpleIndex(Tamgu* key, short idthread, bool sign) {
    double skey = key->Getfloat(idthread);

    Tamgu* val = Value(skey);
    if (val == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror(e_wrong_index, idthread);
        return aNOELEMENT;

    }
    return val;
}

Exporting Tamgu* Tamgumapfs::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            hmap<double, string>::iterator it;
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
        Tamgumapfs* kmap = new Tamgumapfs;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        double vleft = key->Float();
        double vright = keyright->Float();
        hmap<double, string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<double, string>::iterator itr = values.end();
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

Exporting Tamgu* Tamgumapfs::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapfs* m = (Tamgumapfs*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<double, string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapfs::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgumapfs* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfs;
        hmap<double, string> keys;

        for (const auto& it : values)
            keys[it.first] = it.second;
            
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuestring();
            else {
                if (values[v]==itr->Valuestring())
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
        res = (Tamgumapfs*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringXor(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfs::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfs* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapfs*)Atom(true);

    if (b->isMapContainer()) {
        hmap<double, string>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfs*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringOr(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfs::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfs* res;
    if (b->isMapContainer()) {
        hmap<double, string>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfs;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keyfloat()) != values.end() && values[itr->Keyfloat()] == v)
                res->values[itr->Keyfloat()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapfs*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringAnd(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfs::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfs* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfs;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = values.at(k) + itr->Valuestring();
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
        res = (Tamgumapfs*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapfs::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapfs * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapfs;
        double k;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            k = itr->Keyfloat();
            try {
                res->values[k] = StringMinus(values.at(k), itr->Valuestring());
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
        res = (Tamgumapfs*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringMinus(it.second, v);
    return res;
}

Exporting Tamgu* Tamgumapfs::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<double, string>::iterator it;
    
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

