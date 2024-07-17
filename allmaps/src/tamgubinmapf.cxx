/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubinmapf.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgubinmapf.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<binmapfMethod>  Tamgubinmapf::methods;
static ThreadLock classlock;

Exporting short Tamgubinmapf::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubinmapf::AddMethod(TamguGlobal* global, string name, binmapfMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgubinmapf::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgubinmapf::methods.isEmpty())
    Tamgubinmapf::InitialisationModule(global,"");
}

bool Tamgubinmapf::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgubinmapf::idtype = global->Getid("binmapf");

    Tamgubinmapf::AddMethod(global, "clear", &Tamgubinmapf::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgubinmapf::AddMethod(global, "invert", &Tamgubinmapf::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgubinmapf::AddMethod(global, "find", &Tamgubinmapf::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgubinmapf::AddMethod(global, "items", &Tamgubinmapf::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgubinmapf::AddMethod(global, "join", &Tamgubinmapf::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgubinmapf::AddMethod(global, "test", &Tamgubinmapf::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgubinmapf::AddMethod(global, "keys", &Tamgubinmapf::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgubinmapf::AddMethod(global, "values", &Tamgubinmapf::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgubinmapf::AddMethod(global, "sum", &Tamgubinmapf::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgubinmapf::AddMethod(global, "product", &Tamgubinmapf::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgubinmapf::AddMethod(global, "pop", &Tamgubinmapf::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgubinmapf::AddMethod(global, "merge", &Tamgubinmapf::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->returnindextypes[Tamgubinmapf::idtype] = a_float;
    global->newInstance[Tamgubinmapf::idtype] = new Tamgubinmapf(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("ifprimemap")] = new Tamgubinmapf(global);

    global->RecordCompatibilities(global->Getid("ifprimemap"));
    #endif
    global->RecordCompatibilities(Tamgubinmapf::idtype);

    return true;
}


Exporting TamguIteration* Tamgubinmapf::Newiteration(bool direction) {
    return new TamguIterationbinmapf(this, direction);
}


Exporting Tamgu* Tamgubinmapf::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    ushort val = a->Short();

    if (context->isVectorContainer()) {
        Tamguivector* v = (Tamguivector*)Selectaivector(context);
        Doublelocking _lock(this, v);
        if (values.check(val))
            v->values.push_back(val);

        return v;
    }

    if (context->isNumber()) {
        locking();
        if (values.check(val)) {
            unlocking();
            return globalTamgu->Provideint(val);
        }
        unlocking();
        return aNOELEMENT;;
    }
    
    locking();
    if (values.check(val)) {
        unlocking();
        return aTRUE;
    }
    unlocking();
    return aFALSE;

}


Exporting Tamgu* Tamgubinmapf::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<double>::iterator it;

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



Exporting Tamgu* Tamgubinmapf::Push(Tamgu* k, Tamgu* v) {
    locking();
    ushort s = k->Short();
    values[s] = v->Float();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgubinmapf::Pop(Tamgu* kkey) {
    ushort k = kkey->Short();
    Locking _lock(this);
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgubinmapf::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgubinmapf::String() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<double>::iterator it;
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

Exporting string Tamgubinmapf::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<double>::iterator it;
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


Exporting long Tamgubinmapf::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamgubinmapf::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamgubinmapf::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamgubinmapf::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgubinmapf::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamgubinmapf::Put(Tamgu* idx, Tamgu* ke, short idthread) {


    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgubinmapf::idtype) {
            Doublelocking _lock(this, ke);
            values = ((Tamgubinmapf*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyshort()] = itr->Valuefloat();
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
        if (ke->Type() == Tamgubinmapf::idtype) {
            Tamgubinmapf* kmap = (Tamgubinmapf*)ke;
            //We copy all values from ke to this
            basebin_hash<double>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyshort()] = itr->Valuefloat();
            itr->Release();
        }
        unlocking();
        ke->Release();
        return aTRUE;
    }

    locking();
    values[idx->Getshort(idthread)] = ke->Float();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgubinmapf::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            basebin_hash<double>::iterator it;
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
        Tamgubinmapf* kmap = new Tamgubinmapf;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort vright = keyright->Short();
        basebin_hash<double>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<double>::iterator itr = values.end();
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

    ushort skey = idx->Getshort(idthread);

    locking();
    if (!values.check(skey)) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    
    contextualpattern = globalTamgu->Providefloat(values.get(skey));
    unlocking();
    return contextualpattern;
}

Exporting Tamgu* Tamgubinmapf::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgubinmapf* m = (Tamgubinmapf*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<double>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (!values.check(it->first))
            return aFALSE;
        if (it->second != values.get(it->first))
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgubinmapf::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgubinmapf* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        hmap<short, double> keys;

        basebin_hash<double>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;

        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyfloat();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuefloat();
            else {
                if (values[v] == itr->Valuefloat())
                    keys.erase(v);
            }
        }
        itr->Release();

        for (auto& a : keys)
            res->values[a.first] = a.second;
        return res;
    }


    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    BLONG v = b->Long();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleXor(it->second, v);
    return res;
}

Exporting Tamgu* Tamgubinmapf::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf* res;
    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    BLONG v = b->Long();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleOr(it->second, v);
    return res;

}

Exporting Tamgu* Tamgubinmapf::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey) && values.get(ikey) == v)
                res->values[ikey] = v;
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    BLONG v = b->Long();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleAnd(it->second, v);
    return res;
}

Exporting Tamgu* Tamgubinmapf::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values.get(ikey) + v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    double v = b->Float();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamgubinmapf::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values.get(ikey) - v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    double v = b->Float();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Tamgu* Tamgubinmapf::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values.get(ikey) * v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    double v = b->Float();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Tamgu* Tamgubinmapf::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = values.get(ikey) / v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    double v = b->Float();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Tamgu* Tamgubinmapf::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        long v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }

            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = (long)values.get(ikey) % v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = (long)it->second % v;
    return res;
}

Exporting Tamgu* Tamgubinmapf::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        long v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = (long)values.get(ikey) >> v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    long v = b->Integer();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftright(it->second, v);
    return res;
}

Exporting Tamgu* Tamgubinmapf::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        long v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = (long)values.get(ikey) << v;
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    basebin_hash<double>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = DoubleShiftleft(it->second, v);
    return res;
}

Exporting Tamgu* Tamgubinmapf::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapf * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapf;
        double v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = pow(values[ikey], v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapf*)Atom(true);

    double v = b->Float();
    basebin_hash<double>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}




Exporting Tamgu* Tamgubinmapf::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    locking();
    basebin_hash<double>::iterator it;
    vector<short> keys;
    for (it=values.begin(); it != values.end(); it++)
        keys.push_back(it->first);
    
    
    Tamgu* a;
    for (long i = 0; i < keys.size(); i++) {
        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            unlocking();
            return a;
        }

        a->Release();
    }

    unlocking();
    return this;
}
