/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapis.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamguprimemapis.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapisMethod>  Tamguprimemapis::methods;
static ThreadLock classlock;

Exporting short Tamguprimemapis::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapis::AddMethod(TamguGlobal* global, string name,primemapisMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguprimemapis::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamguprimemapis::methods.isEmpty())
    Tamguprimemapis::InitialisationModule(global,"");
}

bool Tamguprimemapis::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    
    Tamguprimemapis::idtype = global->Getid("primemapis");

    Tamguprimemapis::AddMethod(global, "clear", &Tamguprimemapis::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapis::AddMethod(global, "invert", &Tamguprimemapis::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapis::AddMethod(global, "find", &Tamguprimemapis::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemapis::AddMethod(global, "items", &Tamguprimemapis::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemapis::AddMethod(global, "join", &Tamguprimemapis::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapis::AddMethod(global, "test", &Tamguprimemapis::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapis::AddMethod(global, "keys", &Tamguprimemapis::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapis::AddMethod(global, "values", &Tamguprimemapis::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapis::AddMethod(global, "pop", &Tamguprimemapis::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapis::AddMethod(global, "merge", &Tamguprimemapis::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->returnindextypes[Tamguprimemapis::idtype] = a_string;
    global->newInstance[Tamguprimemapis::idtype] = new Tamguprimemapis(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("isprimemap")] = new Tamguprimemapis(global);

    global->RecordCompatibilities(global->Getid("isprimemap"));
    #endif
    global->RecordCompatibilities(Tamguprimemapis::idtype);

    return true;
}


Exporting TamguIteration* Tamguprimemapis::Newiteration(bool direction) {
    return new TamguIterationprimemapis(this, direction);
}


Exporting Tamgu* Tamguprimemapis::in(Tamgu* context, Tamgu* a, short idthread) {
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
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Provideint(val);
        }
        unlocking();
        return aNOELEMENT;;
    }
    
    locking();
    if (values.find(val)!=values.end()) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;

}


Exporting Tamgu* Tamguprimemapis::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<long, string>::iterator it;
    
    string val = callfunc->Evaluate(0, context, idthread)->String();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                v->values.push_back(it->first);
        }
        return v;
    }
    
    Locking _lock(this);
    for (it = values.begin(); it != values.end(); it++) {
        if (it->second == val)
            return globalTamgu->Provideint(it->first);
    }

    return aNULL;

}



Exporting Tamgu* Tamguprimemapis::Push(Tamgu* k, Tamgu* v) {
    locking();
    long s = k->Integer();
    values[s] = v->String();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapis::Pop(Tamgu* kkey) {
    long k = kkey->Integer();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamguprimemapis::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamguprimemapis::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long,string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        stringing(res, it->second);
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapis::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<long,string>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":";
        jstringing(res, it->second);
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapis::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamguprimemapis::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamguprimemapis::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamguprimemapis::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemapis::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamguprimemapis::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        
        if (ke->Type() == Tamguprimemapis::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapis*)ke)->values;
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                values[nb] = (*it)->String();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapis::idtype) {
            Tamguprimemapis* kmap = (Tamguprimemapis*)ke;
            //We copy all values from ke to this
            prime_hash<long,string>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyinteger()] = itr->Valuestring();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }

    locking();
    values[idx->Getinteger(idthread)] = ke->String();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapis::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            prime_hash<long,string>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideint(it->first));
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
        Tamguprimemapis* kmap = new Tamguprimemapis;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        long vleft = key->Integer();
        long vright = keyright->Integer();
        prime_hash<long,string>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<long, string>::iterator itr= values.end();
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

Exporting Tamgu* Tamguprimemapis::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemapis* m = (Tamguprimemapis*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<long,string>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapis::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamguprimemapis* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapis;
        hmap<long, string> keys;

        prime_hash<long, string>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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

    prime_hash<long, string>::iterator it;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapis*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringXor(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapis::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,string>::iterator it;
    Tamguprimemapis* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapis*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapis*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringOr(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapis::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,string>::iterator it;
    Tamguprimemapis* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapis;
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
        res = (Tamguprimemapis*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringAnd(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapis::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,string>::iterator it;
    Tamguprimemapis* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapis;
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
        res = (Tamguprimemapis*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapis::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<long,string>::iterator it;
    Tamguprimemapis * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapis;
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
        res = (Tamguprimemapis*)Atom(true);

    string v = b->String();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second,v);
    return res;
}

Exporting Tamgu* Tamguprimemapis::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    prime_hash<long, string>::iterator it;
    
    Tamgu* a;
    vector<long> keys;

    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);

    a = aNULL;
    bool testcond = false;

    long sz = keys.size();
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

