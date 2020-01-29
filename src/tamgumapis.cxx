/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgumapis.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgumapis.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<mapisMethod>  Tamgumapis::methods;
Exporting hmap<string, string> Tamgumapis::infomethods;
Exporting bin_hash<unsigned long> Tamgumapis::exported;

Exporting short Tamgumapis::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgumapis::AddMethod(TamguGlobal* global, string name,mapisMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}



    void Tamgumapis::Setidtype(TamguGlobal* global) {
        Tamgumapis::idtype = global->Getid("mapis");
    }

   bool Tamgumapis::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamgumapis::idtype = global->Getid("mapis");

    Tamgumapis::AddMethod(global, "clear", &Tamgumapis::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgumapis::AddMethod(global, "invert", &Tamgumapis::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgumapis::AddMethod(global, "find", &Tamgumapis::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgumapis::AddMethod(global, "items", &Tamgumapis::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgumapis::AddMethod(global, "join", &Tamgumapis::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgumapis::AddMethod(global, "test", &Tamgumapis::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgumapis::AddMethod(global, "keys", &Tamgumapis::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgumapis::AddMethod(global, "values", &Tamgumapis::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgumapis::AddMethod(global, "pop", &Tamgumapis::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgumapis::AddMethod(global, "merge", &Tamgumapis::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgumapis::idtype] = new Tamgumapis(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ismap")] = new Tamgumapis(global);

    global->RecordMethods(global->Getid("ismap"), Tamgumapis::exported);
    #endif
    global->RecordMethods(Tamgumapis::idtype, Tamgumapis::exported);

    return true;
}


Exporting TamguIteration* Tamgumapis::Newiteration(bool direction) {
    return new TamguIterationmapis(this, direction);
}


Exporting Tamgu* Tamgumapis::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    long val = a->Integer();

     if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

   if (context->isNumber()) {
        Locking _lock(this);
        if (values.find(val)!=values.end())
            return globalTamgu->Provideint(val);
        return aNOELEMENT;
    }
    
    Locking _lock(this);
    if (values.find(val)!=values.end())
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgumapis::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (auto& it : values) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }
    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
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
            return globalTamgu->Provideint(it.first);
    }
    return aNULL;
}



Exporting Tamgu* Tamgumapis::Push(Tamgu* k, Tamgu* v) {
    Locking _lock(this);
    long s = k->Integer();
    values[s] = v->String();
    return aTRUE;
}

Exporting Tamgu* Tamgumapis::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgumapis::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgumapis::String() {
    Locking _lock(this);
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
    res << "}";
    return res.str();
}

Exporting string Tamgumapis::JSonString() {
    Locking _lock(this);
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
    res << "}";
    return res.str();
}


Exporting long Tamgumapis::Integer() {
    Locking _lock(this);
    return values.size();
}

Exporting double Tamgumapis::Float() {
    Locking _lock(this);
    return values.size();
}

Exporting BLONG Tamgumapis::Long() {
    Locking _lock(this);
    return values.size();
}

Exporting bool Tamgumapis::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgumapis::Size() {
    Locking _lock(this);
    return values.size();
}


Exporting Tamgu*  Tamgumapis::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }
        
        if (ke->Type() == Tamgumapis::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgumapis*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyinteger()] = itr->Valuestring();
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

            for (auto& it : kvect->values) {
                values[nb] = it->String();
                nb++;
            }
            return aTRUE;
        }
        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);
        Locking* _lock = _getlock(this);
        values.clear();
        if (ke->Type() == Tamgumapis::idtype)
            values = ((Tamgumapis*)ke)->values;
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyinteger()] = itr->Valuestring();
            itr->Release();
        }
        ke->Release();
        _cleanlock(_lock);
        return aTRUE;
    }
    Locking* _lock = _getlock(this);
    values[idx->Integer()] = ke->String();
    _cleanlock(_lock);
    return aTRUE;
}


Exporting Tamgu* Tamgumapis::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {

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
            hmap<long,string>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideint(it->first));
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
        Tamgumapis* kmap = new Tamgumapis;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        hmap<long,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        hmap<long, string>::iterator itr= values.end();
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

    long skey = key->Integer();
    key->Release();

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    return kval;
}

Exporting Tamgu* Tamgumapis::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgumapis* m = (Tamgumapis*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    hmap<long,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgumapis::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgumapis* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapis;
        hmap<long, string> keys;

        for (auto& it : values)
            keys[it.first] = it.second;
            
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyinteger();
            if (keys.find(v) == keys.end())
                keys[v]=itr->Valuestring();
            else {
                if (values[v]==itr->Valuestring())
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
        res = (Tamgumapis*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringXor(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapis::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapis* res;
    if (itself)
        res = this;
    else
        res = (Tamgumapis*)Atom(true);

    if (b->isMapContainer()) {
        hmap<long,string>::iterator it;

        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapis*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringOr(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapis::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapis* res;
    if (b->isMapContainer()) {
        hmap<long,string>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapis;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            if (values.find(itr->Keyinteger()) != values.end() && values[itr->Keyinteger()]==v)
                res->values[itr->Keyinteger()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapis*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringAnd(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapis::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapis* res;
    if (b->isMapContainer()) {
        hmap<long,string>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapis;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keyinteger());
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
        res = (Tamgumapis*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second += v;
    return res;
}

Exporting Tamgu* Tamgumapis::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgumapis * res;
    if (b->isMapContainer()) {
        hmap<long,string>::iterator it;

        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgumapis;
        string v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            it = values.find(itr->Keyinteger());
            if (it != values.end()) {
                res->values[it->first] = StringMinus(it->second, v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgumapis*)Atom(true);

    string v = b->String();
    for (auto& it : res->values)
        it.second = StringMinus(it.second,v);
    return res;
}

Exporting Tamgu* Tamgumapis::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Locking _lock(this);
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    hmap<long, string>::iterator it;
    
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
    
    if (testcond) {
        if (a == aBREAK)
            return this;
        return a;
    }

    a->Releasenonconst();
    return this;

}
