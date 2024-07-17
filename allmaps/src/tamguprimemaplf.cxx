/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemaplf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamguprimemaplf.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemaplfMethod>  Tamguprimemaplf::methods;
static ThreadLock classlock;

Exporting short Tamguprimemaplf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemaplf::AddMethod(TamguGlobal* global, string name,primemaplfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamguprimemaplf::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamguprimemaplf::methods.isEmpty())
    Tamguprimemaplf::InitialisationModule(global,"");
}

bool Tamguprimemaplf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    

    
    Tamguprimemaplf::idtype = global->Getid("primemaplf");

    Tamguprimemaplf::AddMethod(global, "clear", &Tamguprimemaplf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemaplf::AddMethod(global, "invert", &Tamguprimemaplf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemaplf::AddMethod(global, "find", &Tamguprimemaplf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemaplf::AddMethod(global, "items", &Tamguprimemaplf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemaplf::AddMethod(global, "join", &Tamguprimemaplf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemaplf::AddMethod(global, "test", &Tamguprimemaplf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemaplf::AddMethod(global, "keys", &Tamguprimemaplf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemaplf::AddMethod(global, "values", &Tamguprimemaplf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemaplf::AddMethod(global, "sum", &Tamguprimemaplf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemaplf::AddMethod(global, "product", &Tamguprimemaplf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemaplf::AddMethod(global, "pop", &Tamguprimemaplf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemaplf::AddMethod(global, "merge", &Tamguprimemaplf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->returnindextypes[Tamguprimemaplf::idtype] = a_float;
    global->newInstance[Tamguprimemaplf::idtype] = new Tamguprimemaplf(global);
    
    global->RecordCompatibilities(Tamguprimemaplf::idtype);

    return true;
}


Exporting TamguIteration* Tamguprimemaplf::Newiteration(bool direction) {
    return new TamguIterationprimemaplf(this, direction);
}


Exporting Tamgu* Tamguprimemaplf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    BLONG val = a->Long();

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

    if (context->isNumber()) {
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Providelong(val);
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


Exporting Tamgu* Tamguprimemaplf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<BLONG, double>::iterator it;
    
    double val = callfunc->Evaluate(0, context, idthread)->Float();
    
    if (context->isBoolean()) {
        Locking _lock(this);
        for (it = values.begin(); it != values.end(); it++) {
            if (it->second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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
            return globalTamgu->Providelong(it->first);
    }

    return aNULL;

}



Exporting Tamgu* Tamguprimemaplf::Push(Tamgu* k, Tamgu* v) {
    locking();
    BLONG s = k->Long();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemaplf::Pop(Tamgu* kkey) {
    BLONG k = kkey->Long();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamguprimemaplf::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamguprimemaplf::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,double>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":" << it->second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemaplf::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<BLONG,double>::iterator it;
    res << "{";
    bool beg = true;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":" << it->second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemaplf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamguprimemaplf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamguprimemaplf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamguprimemaplf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemaplf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamguprimemaplf::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        
        if (ke->Type() == Tamguprimemaplf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemaplf*)ke)->values;
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                values[nb] = (*it)->Float();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamguprimemaplf::idtype) {
            Tamguprimemaplf* kmap = (Tamguprimemaplf*)ke;
            //We copy all values from ke to this
            prime_hash<BLONG,double>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
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


Exporting Tamgu* Tamguprimemaplf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            prime_hash<BLONG,double>::iterator it;
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
        Tamguprimemaplf* kmap = new Tamguprimemaplf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        BLONG vleft = key->Long();
        BLONG vright = keyright->Long();
        prime_hash<BLONG,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<BLONG, double>::iterator itr= values.end();
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

Exporting Tamgu* Tamguprimemaplf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemaplf* m = (Tamguprimemaplf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<BLONG,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemaplf::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Tamguprimemaplf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        hmap<BLONG, double> keys;

        prime_hash<BLONG, double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
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
        res = (Tamguprimemaplf*)Atom(true);

    prime_hash<BLONG,double>::iterator it;
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleXor(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemaplf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemaplf*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleOr(it->second, v);
    return res;

}

Exporting Tamgu* Tamguprimemaplf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keylong()) != values.end() && values[itr->Keylong()]==v)
                res->values[itr->Keylong()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleAnd(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemaplf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemaplf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Tamgu* Tamguprimemaplf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Tamgu* Tamguprimemaplf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Tamgu* Tamguprimemaplf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Tamgu* Tamguprimemaplf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second>> v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemaplf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftright(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemaplf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftleft(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemaplf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<BLONG,double>::iterator it;
    Tamguprimemaplf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemaplf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keylong());
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
        res = (Tamguprimemaplf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemaplf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    prime_hash<BLONG, double>::iterator it;
    
    Tamgu* a;
    vector<BLONG> keys;

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

