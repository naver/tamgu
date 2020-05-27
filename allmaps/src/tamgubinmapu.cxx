/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubinmapu.cxx
 Date       : 2017/09/01
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
//#prime
#include "tamgubinmapu.h"
#include "tamgulist.h"
#include "tamguivector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<binmapuMethod>  Tamgubinmapu::methods;
Exporting hmap<string, string> Tamgubinmapu::infomethods;
Exporting basebin_hash<unsigned long> Tamgubinmapu::exported;

Exporting short Tamgubinmapu::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubinmapu::AddMethod(TamguGlobal* global, string name, binmapuMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    infomethods[name] = infos;
    exported[idname] = arity;
}

bool Tamgubinmapu::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    infomethods.clear();
    exported.clear();


    Tamgubinmapu::idtype = global->Getid("binmapu");

    Tamgubinmapu::AddMethod(global, "clear", &Tamgubinmapu::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgubinmapu::AddMethod(global, "invert", &Tamgubinmapu::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgubinmapu::AddMethod(global, "find", &Tamgubinmapu::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgubinmapu::AddMethod(global, "items", &Tamgubinmapu::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgubinmapu::AddMethod(global, "join", &Tamgubinmapu::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgubinmapu::AddMethod(global, "test", &Tamgubinmapu::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgubinmapu::AddMethod(global, "keys", &Tamgubinmapu::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgubinmapu::AddMethod(global, "values", &Tamgubinmapu::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgubinmapu::AddMethod(global, "pop", &Tamgubinmapu::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgubinmapu::AddMethod(global, "merge", &Tamgubinmapu::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    global->newInstance[Tamgubinmapu::idtype] = new Tamgubinmapu(global);
    #ifdef OLDBACKCOMP
    global->newInstance[global->Getid("iuprimemap")] = new Tamgubinmapu(global);

    global->RecordMethods(global->Getid("iuprimemap"), Tamgubinmapu::exported);
    #endif
    global->RecordMethods(Tamgubinmapu::idtype, Tamgubinmapu::exported);

    return true;
}


Exporting TamguIteration* Tamgubinmapu::Newiteration(bool direction) {
    return new TamguIterationbinmapu(this, direction);
}


Exporting Tamgu* Tamgubinmapu::in(Tamgu* context, Tamgu* a, short idthread) {
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


Exporting Tamgu* Tamgubinmapu::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    basebinn_hash<wstring>::iterator it;

    wstring val = callfunc->Evaluate(0, context, idthread)->UString();

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



Exporting Tamgu* Tamgubinmapu::Push(Tamgu* k, Tamgu* v) {
    locking();
    ushort s = k->Short();
    values[s] = v->UString();
    unlocking();
    return aTRUE;
}


Exporting Tamgu* Tamgubinmapu::Pop(Tamgu* kkey) {
    ushort k = kkey->Short();
    Locking _lock(this);
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgubinmapu::Clear() {
    Locking _lock(this);
    values.clear();
}



Exporting string Tamgubinmapu::String() {
    Locking _lock(this);
    stringstream res;
    basebinn_hash<wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << it->first << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgubinmapu::JSonString() {
    Locking _lock(this);
    stringstream res;
    basebinn_hash<wstring>::iterator it;
    res << "{";
    bool beg = true;
    string sx;
    wstring w;
    for (it = values.begin(); it != values.end(); it++) {
        if (beg == false)
            res << ",";
        beg = false;
        res << '"' << it->first << '"' << ":";
        w = it->second;
        sc_unicode_to_utf8(sx, w);
        jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgubinmapu::Integer() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting double Tamgubinmapu::Float() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting BLONG Tamgubinmapu::Long() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}


Exporting bool Tamgubinmapu::Boolean() {
    Locking _lock(this);
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgubinmapu::Size() {
    locking();
    long sz = values.size();
    unlocking();
    return sz;
}



Exporting Tamgu*  Tamgubinmapu::Put(Tamgu* idx, Tamgu* ke, short idthread) {


    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Type()) {
            Doublelocking _lock(this, ke);
            values = ((Tamgubinmapu*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values[itr->Keyshort()] = itr->Valueustring();
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Doublelocking _lock(this, ke);
            values.clear();
            long nb = 0;
            for (long it = 0; it < ke->Size(); ++it) {
                values[nb] = ke->getustring(it);
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
                values[nb] = (*it)->UString();
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        locking();
        values.clear();
        if (ke->Type() == Tamgubinmapu::idtype) {
            Tamgubinmapu* kmap = (Tamgubinmapu*)ke;
            //We copy all values from ke to this
            basebinn_hash<wstring>::iterator it;
            for (it = kmap->values.begin(); it != kmap->values.end(); it++)
                values[it->first] = it->second;
        }
        else {
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values[itr->Keyshort()] = itr->Valueustring();
            itr->Release();
        }
        unlocking();
        ke->Release();
        return aTRUE;
    }

    locking();
    values[idx->Getshort(idthread)] = ke->UString();
    unlocking();
    return aTRUE;
}

Exporting Tamgu* Tamgubinmapu::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {


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
            basebinn_hash<wstring>::iterator it;
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
        Tamgubinmapu* kmap = new Tamgubinmapu;
        Tamgu* key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        ushort vleft = key->Short();
        ushort  vright = keyright->Short();
        basebinn_hash<wstring>::iterator it = values.find(vleft);
        if (it == values.end() && key != aNULL) {
            key->Release();
            return kmap;
        }
        if (key == aNULL)
            it = values.begin();
        key->Release();
        basebinn_hash<wstring>::iterator itr = values.end();
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

    ushort skey = idx->Getshort(idthread);

    locking();
    if (!values.check(skey)) {
        unlocking();
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;

    }
    
    contextualpattern = globalTamgu->Provideustring(values.get(skey));
    unlocking();
    return contextualpattern;
}

Exporting Tamgu* Tamgubinmapu::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgubinmapu* m = (Tamgubinmapu*)a;

    Doublelocking _lock(this, m);
    if (m->values.size() != values.size())
        return aFALSE;
    basebinn_hash<wstring>::iterator it = m->values.begin();
    while (it != m->values.end()) {
        if (!values.check(it->first))
            return aFALSE;
        if (it->second != values.get(it->first))
            return aFALSE;
        it++;
    }
    return aTRUE;
}

Exporting Tamgu* Tamgubinmapu::xorset(Tamgu* b, bool itself) {

    Doublelocking _lock(this, b);


    Tamgubinmapu* res;
    basebinn_hash<wstring>::iterator it;

    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapu;
        hmap<short, wstring> keys;

        for (it = values.begin(); it != values.end(); it++)
            keys[it->first] = it->second;

        short v;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keyshort();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valueustring();
            else {
                if (values[v] == itr->Valueustring())
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
        res = (Tamgubinmapu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringXor(it->second, v);
    return res;
}

Exporting Tamgu* Tamgubinmapu::orset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    basebinn_hash<wstring>::iterator it;
    Tamgubinmapu* res;
    if (itself)
        res = this;
    else
        res = (Tamgubinmapu*)Atom(true);

    if (b->isMapContainer()) {
        res->Merging(b);
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamgubinmapu::andset(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    basebinn_hash<wstring>::iterator it;
    Tamgubinmapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapu;
        wstring v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
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
        res = (Tamgubinmapu*)Atom(true);

    wstring v = b->UString();
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringAnd(it->second, v);
    return res;
}

Exporting Tamgu* Tamgubinmapu::plus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapu* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapu;
        wstring v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
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
        res = (Tamgubinmapu*)Atom(true);

    wstring v = b->UString();
    basebinn_hash<wstring>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second += v;
    return res;
}

Exporting Tamgu* Tamgubinmapu::minus(Tamgu* b, bool itself) {
    Doublelocking _lock(this, b);

    Tamgubinmapu * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);

        res = new Tamgubinmapu;
        wstring v;
        ushort ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueustring();
            ikey = itr->Keyshort();
            if (values.check(ikey)) {
                res->values[ikey] = StringMinus(values[ikey], v);
            }
        }
        itr->Release();
        return res;
    }

    if (itself)
        res = this;
    else
        res = (Tamgubinmapu*)Atom(true);

    wstring v = b->UString();
    basebinn_hash<wstring>::iterator it;
    for (it = values.begin(); it != values.end(); it++)
        it->second = StringMinus(it->second, v);
    return res;
}




Exporting Tamgu* Tamgubinmapu::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    locking();
    basebinn_hash<wstring>::iterator it;
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
