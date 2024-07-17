/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgua_mapss.cxx
 Date       : 2018/11/27
 Purpose    : map implementation
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"

#include "tamguint.h"
#include "tamgua_mapss.h"
#include "tamgulist.h"
#include "tamgusvector.h"


//We need to declare once again our local definitions.
Exporting basebin_hash<a_mapssMethod>  Tamgua_mapss::methods;
static ThreadLock classlock;

Exporting short Tamgua_mapss::idtype = 0;

//-------------------------------------------------------------------------
//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgua_mapss::AddMethod(TamguGlobal* global, string name, a_mapssMethod func, unsigned long arity, string infos) {
    short idname = global->Getid(name);
    methods[idname] = func;
    if (global->infomethods.find(idtype) != global->infomethods.end() &&
            global->infomethods[idtype].find(name) != global->infomethods[idtype].end())
    return;

    global->infomethods[idtype][name] = infos;
    global->RecordArity(idtype, idname, arity);
}


void Tamgua_mapss::Setidtype(TamguGlobal* global) {
  Locking lock(classlock);
  if (Tamgua_mapss::methods.isEmpty())
    Tamgua_mapss::InitialisationModule(global,"");
}

bool Tamgua_mapss::InitialisationModule(TamguGlobal* global, string version) {
    methods.clear();
    
    


    Tamgua_mapss::idtype = global->Getid("a_mapss");

    Tamgua_mapss::AddMethod(global, "clear", &Tamgua_mapss::MethodClear, P_NONE, "clear(): clear the container.");
    
    Tamgua_mapss::AddMethod(global, "invert", &Tamgua_mapss::MethodInvert, P_NONE, "invert(): return a map with key/value inverted.");
    Tamgua_mapss::AddMethod(global, "find", &Tamgua_mapss::MethodFind, P_ONE, "find(value): test if a value belongs to the map and return 'true' or the corresponding keys.");


    Tamgua_mapss::AddMethod(global, "items", &Tamgua_mapss::MethodItems, P_NONE, "items(): Return a vector of {key:value} pairs.");


    Tamgua_mapss::AddMethod(global, "join", &Tamgua_mapss::MethodJoin, P_TWO, "join(string sepkey,string sepvalue): Produce a string representation for the container.");
    Tamgua_mapss::AddMethod(global, "test", &Tamgua_mapss::MethodTest, P_ONE, "test(key): Test if key belongs to the map container.");
    Tamgua_mapss::AddMethod(global, "keys", &Tamgua_mapss::MethodKeys, P_NONE, "keys(): Return the map container keys as a vector.");
    Tamgua_mapss::AddMethod(global, "values", &Tamgua_mapss::MethodValues, P_NONE, "values(): Return the map container values as a vector.");
    Tamgua_mapss::AddMethod(global, "pop", &Tamgua_mapss::MethodPop, P_ONE, "pop(key): Erase an element from the map");
    Tamgua_mapss::AddMethod(global, "merge", &Tamgua_mapss::MethodMerge, P_ONE, "merge(v): Merge v into the vector.");

    Tamgua_mapss::AddMethod(global, "check", &Tamgua_mapss::MethodCheck, P_NONE, "check(): check consistence.");
    Tamgua_mapss::AddMethod(global, "compact", &Tamgua_mapss::MethodCompact, P_NONE, "compact(): remove empty chunks.");

    global->newInstance[Tamgua_mapss::idtype] = new Tamgua_mapss(global);
    global->RecordCompatibilities(Tamgua_mapss::idtype);

    return true;
}


Exporting TamguIteration* Tamgua_mapss::Newiteration(bool direction) {
    return new TamguIterationa_mapss(this, direction);
}


Exporting Tamgu* Tamgua_mapss::in(Tamgu* context, Tamgu* a, short idthread) {
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

Exporting Tamgu* Tamgua_mapss::MethodFind(Tamgu* context, short idthread, TamguCall* callfunc) {
    //Three cases along the container type...
    //It is a Boolean, we expect false or true
    //It is an integer, we expect a position in v
    //It is a container, we are looking for all positions...

    TamguIterationa_mapss it(this, false);

    string val = callfunc->Evaluate(0, context, idthread)->String();

    if (context->isBoolean()) {
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                return aTRUE;
        }
        return aFALSE;
    }

    if (context->isVectorContainer()) {
        Tamgusvector* v = (Tamgusvector*)Selectasvector(context);
        Locking _lock(v);
        for (it.Begin(); it.End() != aTRUE; it.Next()) {
            if (it.second.value() == val)
                v->values.push_back(it.first);
        }
        return v;
    }

    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (it.second.value() == val)
            return globalTamgu->Providestring(it.first);
    }

    return aNULL;

}

Exporting Tamgu* Tamgua_mapss::Push(Tamgu* k, Tamgu* v) {
    string s = k->String();
    string vl = v->String();
    values.set(s, vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapss::Pop(Tamgu* kkey) {
    string k = kkey->String();
    if (values.check(k)) {
        values.erase(k);
        return aTRUE;
    }
    return aFALSE;
}

Exporting void Tamgua_mapss::Clear() {
    values.clear();
}

Exporting string Tamgua_mapss::String() {
    stringstream res;
    TamguIterationa_mapss it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;stringing(res, sx);
        res << ":";
        sx = it.second.value();stringing(res, sx);
    }
    res << "}";
    return res.str();
}

Exporting string Tamgua_mapss::JSonString() {
    stringstream res;
    TamguIterationa_mapss it(this, false);
    res << "{";
    bool beg = true;
    string sx;
    for (it.Begin(); it.End() != aTRUE; it.Next()) {
        if (beg == false)
            res << ",";
        beg = false;
        sx = it.first;jstringing(res, sx);
        res << ":";
        sx = it.second.value();jstringing(res, sx);
    }
    res << "}";
    return res.str();
}


Exporting long Tamgua_mapss::Integer() {
    
    return values.size();
}

Exporting double Tamgua_mapss::Float() {
    
    return values.size();
}

Exporting BLONG Tamgua_mapss::Long() {
    
    return values.size();
}

Exporting bool Tamgua_mapss::Boolean() {
    
    if (values.size() == 0)
        return false;
    return true;
}


//Basic operations
Exporting long Tamgua_mapss::Size() {
    
    return values.size();
}


Exporting Tamgu*  Tamgua_mapss::Put(Tamgu* idx, Tamgu* ke, short idthread) {
    string vl;
    if (!idx->isIndex()) {
        if (ke == this)
            return aTRUE;

        if (ke->isNULL()) {
            Clear();
            return aTRUE;
        }

        if (ke->Type() == Tamgua_mapss::idtype) {
            values = ((Tamgua_mapss*)ke)->values;
            return aTRUE;
        }

        if (ke->isMapContainer()) {
            Locking _lock((TamguObject*)ke);
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() == aFALSE; itr->Next()) {
                vl = itr->Valuestring();
                values.set(itr->Keystring(),  vl);
            }
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
                vl = ke->getstring(it);
                values.set(buff, vl);
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
                vl = (*it)->String();
                values.set(buff, vl);
                nb++;
            }
            return aTRUE;
        }

        ke = ke->Map(idthread);
        if (!ke->isMapContainer())
            return globalTamgu->Returnerror("Wrong map initialization", idthread);

        if (ke->Type() == Tamgua_mapss::idtype) {
            //We copy all values from ke to this
            values = ((Tamgua_mapss*)ke)->values;
        }
        else {
            values.clear();
            TamguIteration* itr = ke->Newiteration(false);
            for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
                vl = itr->Valuestring();
                values.set(itr->Keystring(),  vl);
            }
            itr->Release();
        }

        ke->Release();
        return aTRUE;
    }

    vl = ke->String();
    values.set(idx->Getstring(idthread),  vl);
    return aTRUE;
}

Exporting Tamgu* Tamgua_mapss::Eval(Tamgu* contextualpattern, Tamgu* idx, short idthread) {
    if (!idx->isIndex()) {
        //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
                if (contextualpattern->isMapContainer())
            return this;
        
       //particular case, the contextualpattern is a vector, which means that we expect a set of keys
        //as a result
        if (contextualpattern->isVectorContainer() || contextualpattern->Type() == a_list) {
            Tamgu* vect = contextualpattern->Newinstance(idthread);
            TamguIterationa_mapss it(this, false);
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
        TamguIterationa_mapss it(this, false);
        Tamgua_mapss* kmap = new Tamgua_mapss;
        key = ((TamguIndex*)idx)->left->Eval(aNULL, aNULL, idthread);
        Tamgu* keyright = ((TamguIndex*)idx)->right->Eval(aNULL, aNULL, idthread);
        string vleft = key->String();
        string vright = keyright->String();
        it.find(vleft);
        if (it.End() && key != aNULL) {
            key->Release();
            return kmap;
        }
        
        TamguIterationa_mapss itr(this, false);

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
    
    atomic_string vl;
    values.get(skey, vl);
    string res = vl.value();
    return globalTamgu->Providestring(res);
}

Exporting Tamgu* Tamgua_mapss::same(Tamgu* a) {

    if (a->Type() != idtype)
        return Mapcompare(this, a, NULL);

    Tamgua_mapss* m = (Tamgua_mapss*)a;

    Locking* _lock = _getlocktamgu(m);
    
    if (m->values.size() != values.size())
        return aFALSE;
    
    TamguIterationa_mapss it(m, false);
    atomic_string vl;
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

Exporting Tamgu* Tamgua_mapss::plus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapss* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapss;
        string v;
        string ikey;
        atomic_string vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = vl.value() + v;
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
        res = (Tamgua_mapss*)Atom(true);
    
    string v = b->String();
    string vl;
    TamguIterationa_mapss it(this, false);
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first,  vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapss::minus(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapss * res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapss;
        string v;
        string ikey;
        atomic_string vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                v = StringMinus(vl.value(), v);
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
        res = (Tamgua_mapss*)Atom(true);
    
    string v = b->String();
    TamguIterationa_mapss it(this, false);
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringMinus(it.second.value(), v);
        values.set(it.first, vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapss::xorset(Tamgu* b, bool itself) {
    
    Locking* _lock = _getlocktamgu(b);
    
    Tamgua_mapss* res;
    
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapss;
        hmap<string, string> keys;
        
        TamguIterationa_mapss it(this, false);
        for (it.Begin(); it.End() == aFALSE; it.Next())
            keys[it.first] = it.second.value();
        
        string v;
        atomic_string vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Keystring();
            if (keys.find(v) == keys.end())
                keys[v] = itr->Valuestring();
            else {
                if (values.check(v)) {
                    values.get(v, vl);
                    if (vl.value() == itr->Valuestring())
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
        res = (Tamgua_mapss*)Atom(true);
    
    TamguIterationa_mapss it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringXor(it.second.value(), v);
        values.set(it.first, vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapss::orset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);

    
    Tamgua_mapss* res;
    if (itself)
        res = this;
    else
        res = (Tamgua_mapss*)Atom(true);
    
    if (b->isMapContainer()) {
        res->Merging(b);
        _cleanlocktamgu(_lock);
        return res;
    }
    
    TamguIterationa_mapss it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = it.second.value() + v;
        values.set(it.first, vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapss::andset(Tamgu* b, bool itself) {
    Locking* _lock = _getlocktamgu(b);
    
    
    Tamgua_mapss* res;
    if (b->isMapContainer()) {
        TamguIteration* itr = b->Newiteration(false);
        
        res = new Tamgua_mapss;
        string v;
        string ikey;
        atomic_string vl;
        for (itr->Begin(); itr->End() != aTRUE; itr->Next()) {
            v = itr->Valuestring();
            ikey = itr->Keystring();
            if (values.check(ikey)) {
                values.get(ikey, vl);
                if (vl.value() ==  v)
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
        res = (Tamgua_mapss*)Atom(true);
    
    TamguIterationa_mapss it(this, false);
    string v = b->String();
    string vl;
    for (it.Begin(); it.End() == aFALSE; it.Next()) {
        vl = StringAnd(it.second.value(), v);
        values.set(it.first, vl);
    }
    _cleanlocktamgu(_lock);
    return res;
}

Exporting Tamgu* Tamgua_mapss::Loopin(TamguInstruction* ins, Tamgu* context, short idthread) {
    Tamgu* var = ins->instructions.vecteur[0]->Instruction(0);
    var = var->Eval(context, aNULL, idthread);
    
    vector<string> keys;
    
    TamguIterationa_mapss itr(this, false);
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

