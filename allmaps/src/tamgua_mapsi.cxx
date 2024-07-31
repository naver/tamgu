/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapsi.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapsi.h"
#include "tamgulist.h"
#include "tamgusvector.h"
#include "tamguivector.h"

//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapsiMethod>  Tamgua_mapsi::methods;
static ThreadLock classlock;

Exporting short Tamgua_mapsi::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapsi::AddMethod(TamguGlobal* global, string name, a_mapsiMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapsi::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgua_mapsi::methods.isEmpty())
    Tamgua_mapsi::InitialisationModule(global,"");
}

bool Tamgua_mapsi::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapsi::idtype = global->Getid("a_mapsi");

    Tamgua_mapsi::AddMethod(global, "clear", &Tamgua_mapsi::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapsi::AddMethod(global, "invert", &Tamgua_mapsi::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapsi::AddMethod(global, "find", &Tamgua_mapsi::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapsi::AddMethod(global, "items", &Tamgua_mapsi::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapsi::AddMethod(global, "join", &Tamgua_mapsi::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapsi::AddMethod(global, "test", &Tamgua_mapsi::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapsi::AddMethod(global, "keys", &Tamgua_mapsi::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapsi::AddMethod(global, "values", &Tamgua_mapsi::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapsi::AddMethod(global, "sum", &Tamgua_mapsi::MethodSum, P_NONE, "sum(): return the sum of elements.");
    Tamgua_mapsi::AddMethod(global, "product", &Tamgua_mapsi::MethodProduct, P_NONE, "product(): return the product of elements.");
    Tamgua_mapsi::AddMethod(global, "pop", &Tamgua_mapsi::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapsi::AddMethod(global, "merge", &Tamgua_mapsi::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapsi::AddMethod(global, "check", &Tamgua_mapsi::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapsi::AddMethod(global, "compact", &Tamgua_mapsi::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapsi::idtype] = new Tamgua_mapsi(global);
    global->RecordCompatibilities(Tamgua_mapsi::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapsi::Newiteration(bool direction) {
    return new TamguIterationa_mapsi(this, direction);
}


Exporting Tamgu* Tamgua_mapsi::in(Tamgu* context, Tamgu* a, short idthread) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    string val = a->String();

    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Locking _lock(v);
        if (values.check(val))
            v->values.push_back(val);

        return v;
    }

    if (context->isString()) {
        if (values.check(val))
            return globalTamgu->Providestring(val);
        return aNOELEMENT;
    }
    
    if (values.check(val))
        return aTRUE;

    return aFALSE;

}

Exporting Tamgu* Tamgua_mapsi::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapsi it(this, false);

    long val = callfunc->Evaluate(0, context, idthread)->Integer();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second == val)
            return globalTamgu->Providestring(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapsi::Push(Tamgu* k, Tamgu* v) {
    string s = k->String();
    values.set(s, v->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapsi::Pop(Tamgu* kkey) {
    string k = kkey->String();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapsi::Clear() {
    values.clear();
}

Exporting string Tamgua_mapsi::String() {
    stringstream res;
    TamguIterationa_mapsi it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;stringing(res, sx);
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapsi::JSonString() {
    stringstream res;
    TamguIterationa_mapsi it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;jstringing(res, sx);
        res << ":";
        res << it.second;
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapsi::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapsi::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapsi::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapsi::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapsi::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapsi::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapsi::idtype) {
            values = ((Tamgua_mapsi*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next())
                values.set(itr->Keystring(),  itr->Valueinteger());
            itr->Release();
            return aTRUE;
        }

        if (ke->isVectorContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            long nb = 0;
            char buff[20];
            for (long it = 0; it < ke->Size(); ++it) {
                sprintf_s(buff, 20, "%ld", nb);
                values.set(buff, ke->getinteger(it));
                nb++;
            }
            return aTRUE;
        }

        if (ke->Type() == a_list) {
            Locking _lock((TamguObject*)ke);
            Tamgulist* kvect = (Tamgulist*)ke;
            values.clear();
            long nb = 0;
            list<Tamgu*>::iterator it;
            char buff[20];
            for (it = kvect->values.begin(); it != kvect->values.end(); it++) {
                sprintf_s(buff, 20, "%ld", nb);
                values.set(buff, (*it)->Integer());
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapsi::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapsi*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next())
                values.set(itr->Keystring(),  itr->Valueinteger());
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    values.set(idx->Getstring(idthread),  ke->Integer());
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapsi::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapsi it(this, false);
            for (it.Begin(); it.End() == aFALSE; it.Next())
                vect->Push(globalTamgu->Providestring(it.first));
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
        TamguIterationa_mapsi it(this, false);
        Tamgua_mapsi* kmap = new Tamgua_mapsi;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapsi itr(this, false);

        if (key == aNULL)
            itr.Begin();
        
        key->Release();
        itr.end = true;
        if (keyright != aNULL) {
            itr.find(vright);
            if (itr.End()) {
                if (keyright != ((TamguIndex*)idx)->right)
                    keyright->Release();
                return kmap;
            }
        }
        if (keyright != ((TamguIndex*)idx)->right)
            keyright->Release();
        for (; itr.End() == aFALSE; itr.Next()) {
            kmap->values.set(it.first, it.second);
            if (it.compare(itr))
                return kmap;
        }

        if (!itr.end)
            kmap->values.clear();
        return kmap;

    }

    string skey = idx->Getstring(idthread);

    if (!values.check(skey)) {
        if (globalTamgu->erroronkey)
            return globalTamgu->Returnerror("Wrong index", idthread);
        return aNOELEMENT;
    }
    
    long vl;
    values.get(skey, vl);
    return globalTamgu->Provideint(vl);
}

Exporting Tamgu* Tamgua_mapsi::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapsi* m = (Tamgua_mapsi*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapsi it(m, false);
    long vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        if (!values.check(it.first)) {
            _cleanlocktamgu(_lock);
            return aFALSE;
        }
        values.get(it.first, vl);
        if (it.second != vl) {
            _cleanlocktamgu(_lock);
            return aFALSE;
        }
    }
    _cleanlocktamgu(_lock);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapsi::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlocktamgu(b);

    Tamgua_mapsi* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        hmap<string, long> keys;
        
        TamguIterationa_mapsi it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second;
        
        string v;
        long vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valueinteger();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl == itr->Valueinteger())
                        keys.erase(v);
                }
            }
        }
        itr->Release();
        
        for (auto& a : keys)
            res->values.set(a.first, a.second);
        
        
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    TamguIterationa_mapsi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second ^ v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapsi::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapsi* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapsi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second | v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapsi::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapsi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v;
        long vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl == v)
                    res->values.set(ikey, v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    TamguIterationa_mapsi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second & v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapsi::plus(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);
    
    
    Tamgua_mapsi* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v;
        string ikey;
        long vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl + v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapsi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second + v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapsi::minus(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v;
        string ikey;
        long vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl - v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapsi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second - v);
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapsi::multiply(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v, vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl * v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    long v = b->Integer();
    TamguIterationa_mapsi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second * v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapsi::divide(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v, vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                itr->Release();
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl / v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapsi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  it.second / v);
    _cleanlocktamgu(_lock);
    return res;
    
}

Exporting Tamgu* Tamgua_mapsi::mod(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v, vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            if (v == 0) {
                itr->Release();
                res->Release();
                return globalTamgu->Returnerror("Error: Divided by 0");
            }
            
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl % v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);;
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    long v = b->Integer();
    if (v == 0) {
        res->Release();
        return globalTamgu->Returnerror("Error: Divided by 0");
    }
    TamguIterationa_mapsi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second % v);
    _cleanlocktamgu(_lock);;
    return res;
}

Exporting Tamgu* Tamgua_mapsi::shiftright(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v, vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl >> v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);;
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    TamguIterationa_mapsi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second >> v);
    _cleanlocktamgu(_lock);;
    return res;
}

Exporting Tamgu* Tamgua_mapsi::shiftleft(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        long v, vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valueinteger();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, vl << v);
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);;
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    TamguIterationa_mapsi it(this, false);
    long v = b->Integer();
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first, it.second << v);
    _cleanlocktamgu(_lock);;
    return res;
}

Exporting Tamgu* Tamgua_mapsi::power(Tamgu* b, bool itself) {
    Locking* _lock =  _getlocktamgu(b);

    
    Tamgua_mapsi * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapsi;
        double v;
        long vl;
        string ikey;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuefloat();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                res->values.set(ikey, pow(vl, v));
            }
        }
        itr->Release();
        _cleanlocktamgu(_lock);;
        return res;
    }
    
    if (itself)
        res = this;
    else
        res = (Tamgua_mapsi*)Atom(true);
    
    double v = b->Float();
    TamguIterationa_mapsi it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next())
        values.set(it.first,  pow(it.second, v));
    _cleanlocktamgu(_lock);;
    return res;
}



Exporting Tamgu* Tamgua_mapsi::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<string> keys;
    
    TamguIterationa_mapsi itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        keys.push_back(itr.first);
    
    
    Tamgu* a;
    for (long i = 0; i < keys.size(); i++) {
        var->storevalue(keys[i]);

        a = ins->instructions.vecteur[1]->Eval(context, aNULL, idthread);

        //Continue does not trigger needInvestigate
        if (a->needInvestigate()) {
            if (a == aBREAK)
                break;
            return a;
        }

        a->Release();
    }

    return this;
}

double Tamgua_mapsi::Sum() {
    double v = 0;
    TamguIterationa_mapsi itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v += itr.second;
    return v;
}

double Tamgua_mapsi::Product() {
    double v = 1;
    TamguIterationa_mapsi itr(this, false);
    for (itr.Begin(); itr.End() != aTRUE; itr.Next())
        v *= itr.second;
    return v;
}
