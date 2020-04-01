/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguprimemapuf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamguuvector.h"
#include "tamguustring.h"
//#prime
#include "tamguprimemapuf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<primemapufMethod>  Tamguprimemapuf::methods;
Exporting hmap<string, string> Tamguprimemapuf::infomethods;
Exporting bin_hash<unsigned long> Tamguprimemapuf::exported;

Exporting short Tamguprimemapuf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamguprimemapuf::AddMethod(TamguGlobal* global, string name,primemapufMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamguprimemapuf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();

    
    Tamguprimemapuf::idtype = global->Getid("primemapuf");

    Tamguprimemapuf::AddMethod(global, "clear", &Tamguprimemapuf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamguprimemapuf::AddMethod(global, "invert", &Tamguprimemapuf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamguprimemapuf::AddMethod(global, "find", &Tamguprimemapuf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamguprimemapuf::AddMethod(global, "items", &Tamguprimemapuf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamguprimemapuf::AddMethod(global, "join", &Tamguprimemapuf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamguprimemapuf::AddMethod(global, "test", &Tamguprimemapuf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamguprimemapuf::AddMethod(global, "keys", &Tamguprimemapuf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamguprimemapuf::AddMethod(global, "values", &Tamguprimemapuf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamguprimemapuf::AddMethod(global, "sum", &Tamguprimemapuf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamguprimemapuf::AddMethod(global, "product", &Tamguprimemapuf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamguprimemapuf::AddMethod(global, "pop", &Tamguprimemapuf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamguprimemapuf::AddMethod(global, "merge", &Tamguprimemapuf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamguprimemapuf::idtype] = new Tamguprimemapuf(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ufprimemap")] = new Tamguprimemapuf(global);

    global->RecordMethods(global->Getid("ufprimemap"), Tamguprimemapuf::exported);
    #endif
    global->RecordMethods(Tamguprimemapuf::idtype, Tamguprimemapuf::exported);

    return true;
}


Exporting TamguIteration* Tamguprimemapuf::Newiteration(bool direction) {
    return new TamguIterationprimemapuf(this, direction);
}


Exporting Tamgu* Tamguprimemapuf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...
    
    wstring val = a->UString();

    if (context->isVectorContainer()) {
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
        Doublelocking _lock(this, v);
        if (values.find(val)!=values.end())
            v->values.push_back(val);

        return v;
    }

    if (context->isString()) {
        locking();
        if (values.find(val)!=values.end()) {
            unlocking();
            return globalTamgu->Provideustring(val);
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


Exporting Tamgu* Tamguprimemapuf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    prime_hash<wstring, double>::iterator it;
    
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
        Tamguuvector* v = (Tamguuvector*)Selectauvector(context);
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
            return globalTamgu->Provideustring(it->first);
    }

    return aNULL;

}



Exporting Tamgu* Tamguprimemapuf::Push(Tamgu* k, Tamgu* v) {
    locking();
    wstring s = k->UString();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapuf::Pop(Tamgu* kkey) {
    wstring k = kkey->UString();
    Locking _lock(this);
    if (values.find(k) != values.end()) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamguprimemapuf::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamguprimemapuf::String() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,double>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
        res << ":" << it->second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamguprimemapuf::JSonString() {
    Locking _lock(this);
    stringstream res;
    prime_hash<wstring,double>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        w = it->first;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
        res << ":" << it->second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamguprimemapuf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamguprimemapuf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamguprimemapuf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamguprimemapuf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamguprimemapuf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


#ifndef swprintf_s
#define swprintf_s(a,b,c,d) swprintf(a,b,c,d)
    #endif


Exporting Tamgu*  Tamguprimemapuf::Put(Tamgu* idx, Tamgu* ke, short idthread) {

    
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        
        if (ke->Type() == Tamguprimemapuf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamguprimemapuf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyustring()] = itr->Valuefloat();
            itr->Release();
            return aTRUE;
        }

        wchar_t buff[20];
        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                swprintf_s(buff, 20, L"%d", nb);
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
            list<Tamgu*>::iterator it;
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                swprintf_s(buff, 20, L"%d", nb);
                values[buff] = (*it)->Float();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamguprimemapuf::idtype) {
            Tamguprimemapuf* kmap = (Tamguprimemapuf*)ke;
            //We copy all values from ke to this
            prime_hash<wstring,double>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyustring()] = itr->Valuefloat();
            itr->Release();
        }
        ke->Release();
        unlocking();
        return aTRUE;
    }

    locking();
    values[idx->Getustring(idthread)] = ke->Float();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamguprimemapuf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            prime_hash<wstring,double>::iterator it;
            for (it = values.begin(); it != values.end(); it++)
                vect->Push(globalTamgu->Provideustring(it->first));
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
        Tamguprimemapuf* kmap = new Tamguprimemapuf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        wstring vleft = key->UString();
        wstring vright = keyright->UString();
        prime_hash<wstring,double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        prime_hash<wstring, double>::iterator itr= values.end();
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

    wstring skey = idx->Getustring(idthread);

    Tamgu* kval = Value(skey);
    if (kval == aNOELEMENT) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    return kval;
}

Exporting Tamgu* Tamguprimemapuf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamguprimemapuf* m = (Tamguprimemapuf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    prime_hash<wstring,double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (values.find(it->first) == values.end())
            return aFALSE;
        if (it->second != values[it->first])
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamguprimemapuf::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);

    
    Tamguprimemapuf* res;
    
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        hmap<wstring, double> keys;

        prime_hash<wstring, double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;
            
        wstring v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyustring();
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
        res = (Tamguprimemapuf*)Atom(true);

    prime_hash<wstring,double>::iterator it;
    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleXor(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapuf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf* res;
    if (itself)
        res = this;
    else
        res = (Tamguprimemapuf*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleOr(it->second, v);
    return res;

}

Exporting Tamgu* Tamguprimemapuf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (values.find(itr->Keyustring()) != values.end() && values[itr->Keyustring()]==v)
                res->values[itr->Keyustring()] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapuf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleAnd(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapuf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamguprimemapuf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Tamgu* Tamguprimemapuf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Tamgu* Tamguprimemapuf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Tamgu* Tamguprimemapuf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            it = values.find(itr->Keyustring());
            if (it != values.end()) {
                res->values[it->first] = (long)it->second  % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamguprimemapuf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Tamgu* Tamguprimemapuf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftright(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapuf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        long v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftleft(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapuf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    prime_hash<wstring,double>::iterator it;
    Tamguprimemapuf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamguprimemapuf;
        double v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            it = values.find(itr->Keyustring());
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
        res = (Tamguprimemapuf*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}

Exporting Tamgu* Tamguprimemapuf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    locking();
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);

    
    prime_hash<wstring, double>::iterator it;
    
    Tamgu* a;
    vector<wstring> keys;

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

