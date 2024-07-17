/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubinmapl.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgubinmapl.h"
#include "tamgulist.h"
#include "tamgulvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<binmaplMethod>  Tamgubinmapl::methods;
static ThreadLock classlock;

Exporting short Tamgubinmapl::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubinmapl::AddMethod(TamguGlobal* global, string name, binmaplMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgubinmapl::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgubinmapl::methods.isEmpty())
    Tamgubinmapl::InitialisationModule(global,"");
}

bool Tamgubinmapl::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgubinmapl::idtype = global->Getid("binmapl");

    Tamgubinmapl::AddMethod(global, "clear", &Tamgubinmapl::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgubinmapl::AddMethod(global, "invert", &Tamgubinmapl::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgubinmapl::AddMethod(global, "find", &Tamgubinmapl::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgubinmapl::AddMethod(global, "items", &Tamgubinmapl::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgubinmapl::AddMethod(global, "join", &Tamgubinmapl::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgubinmapl::AddMethod(global, "test", &Tamgubinmapl::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgubinmapl::AddMethod(global, "keys", &Tamgubinmapl::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgubinmapl::AddMethod(global, "values", &Tamgubinmapl::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgubinmapl::AddMethod(global, "sum", &Tamgubinmapl::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgubinmapl::AddMethod(global, "product", &Tamgubinmapl::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgubinmapl::AddMethod(global, "pop", &Tamgubinmapl::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgubinmapl::AddMethod(global, "merge", &Tamgubinmapl::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->returnindextypes[Tamgubinmapl::idtype] = a_long;
    global->newInstance[Tamgubinmapl::idtype] = new Tamgubinmapl(global);
    
    global->RecordCompatibilities(Tamgubinmapl::idtype);

    return true;
}


Exporting TamguIteration* Tamgubinmapl::Newiteration(bool direction) {
    return new TamguIterationbinmapl(this, direction);
}


Exporting Tamgu* Tamgubinmapl::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    ushort val = a->Short();

    if (context->isVectorContainer()) {
        Tamgulvector* v = (Tamgulvector*)Selectalvector(context);
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


Exporting Tamgu* Tamgubinmapl::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebin_hash<BLONG>::iterator it;

    BLONG val = callfunc->Evaluate(0, context, idthread)->Long();

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
            return globalTamgu->Provideint(it->first);
    }

    return aNULL;

}



Exporting Tamgu* Tamgubinmapl::Push(Tamgu* k, Tamgu* v) {
    locking();
    ushort s = k->Short();
    values[s] = v->Long();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgubinmapl::Pop(Tamgu* kkey) {
    ushort k = kkey->Short();
    Locking _lock(this);
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgubinmapl::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgubinmapl::String() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<BLONG>::iterator it;
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

Exporting string Tamgubinmapl::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebin_hash<BLONG>::iterator it;
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


Exporting long Tamgubinmapl::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamgubinmapl::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamgubinmapl::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamgubinmapl::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgubinmapl::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamgubinmapl::Put(Tamgu* idx, Tamgu* ke, short idthread) {


    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Type()) {
            Doublelocking _lock(this, ke);
            values = ((Tamgubinmapl*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);

            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyshort()] = itr->Valuelong();
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getinteger(it);
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
                values[nb] = (*it)->Long();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamgubinmapl::idtype) {
            Tamgubinmapl* kmap = (Tamgubinmapl*)ke;
            //We copy all values from ke to this
            basebin_hash<BLONG>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyshort()] = itr->Valuelong();
            itr->Release();
        }
        unlocking();
        ke->Release();
        return aTRUE;
    }

    locking();
    values[idx->Getshort(idthread)] = ke->Long();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgubinmapl::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            basebin_hash<BLONG>::iterator it;
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
        Tamgubinmapl* kmap = new Tamgubinmapl;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort vright = keyright->Short();
        basebin_hash<BLONG>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebin_hash<BLONG>::iterator itr = values.end();
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
    
    contextualpattern = globalTamgu->Providelong(values.get(skey));
    unlocking();
    return contextualpattern;
}

Exporting Tamgu* Tamgubinmapl::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgubinmapl* m = (Tamgubinmapl*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebin_hash<BLONG>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (!values.check(it->first))
            return aFALSE;
        if (it->second != values.get(it->first))
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgubinmapl::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgubinmapl* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
        hmap<short, BLONG> keys;

        basebin_hash<BLONG>::iterator it;
        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;

        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyshort();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuelong();
            else {
                if (values[v] == itr->Valuelong())
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
        res = (Tamgubinmapl*)Atom(true);

    basebin_hash<BLONG>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second ^ v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl* res;
    if (itself)
        res = this;
    else
        res = (Tamgubinmapl*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    long v = b->Integer();
    basebin_hash<BLONG>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second | v;
    return res;

}

Exporting Tamgu* Tamgubinmapl::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
        BLONG v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Tamgubinmapl*)Atom(true);

    long v = b->Integer();
    basebin_hash<BLONG>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second & v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
        BLONG v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Tamgubinmapl*)Atom(true);

    BLONG v = b->Long();
    basebin_hash<BLONG>::iterator it;
   for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
        BLONG v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Tamgubinmapl*)Atom(true);

    BLONG v = b->Long();
    basebin_hash<BLONG>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second -= v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::multiply(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
        BLONG v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Tamgubinmapl*)Atom(true);

    BLONG v = b->Long();
    basebin_hash<BLONG>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second *= v;
    return res;

}

Exporting Tamgu* Tamgubinmapl::divide(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
        BLONG v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuelong();
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
        res = (Tamgubinmapl*)Atom(true);

    BLONG v = b->Long();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    
    basebin_hash<BLONG>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second /= v;
    return res;

}

Exporting Tamgu* Tamgubinmapl::mod(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
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
        res = (Tamgubinmapl*)Atom(true);

    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    
    basebin_hash<BLONG>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second % v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::shiftright(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
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
        res = (Tamgubinmapl*)Atom(true);

    basebin_hash<BLONG>::iterator it;
    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second >> v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::shiftleft(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<BLONG>::iterator it;
    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
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
        res = (Tamgubinmapl*)Atom(true);

    long v = b->Integer();
    for (it = values.begin(); it != values.end(); it++)
        it->second = it->second << v;
    return res;
}

Exporting Tamgu* Tamgubinmapl::power(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    basebin_hash<BLONG>::iterator it;
    Tamgubinmapl * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapl;
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
        res = (Tamgubinmapl*)Atom(true);

    double v = b->Float();
    for (it = values.begin(); it != values.end(); it++)
        it->second = pow(it->second, v);
    return res;
}




Exporting Tamgu* Tamgubinmapl::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    locking();
    basebin_hash<BLONG>::iterator it;
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
